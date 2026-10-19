#include "SfzParser.h"

namespace
{
    using Opcodes = std::vector<std::pair<juce::String, juce::String>>;

    // Removes // line comments and /* block */ comments
    juce::String stripComments(const juce::String& text)
    {
        juce::String result;
        result.preallocateBytes(text.getNumBytesAsUTF8());

        auto p = text.getCharPointer();
        while (! p.isEmpty())
        {
            if (*p == '/' && p[1] == '/')
            {
                while (! p.isEmpty() && *p != '\n')
                    ++p;
            }
            else if (*p == '/' && p[1] == '*')
            {
                p += 2;
                while (! p.isEmpty() && ! (*p == '*' && p[1] == '/'))
                    ++p;
                if (! p.isEmpty())
                    p += 2;
            }
            else
            {
                result << juce::String::charToString(*p);
                ++p;
            }
        }
        return result;
    }

    bool isNameChar(juce::juce_wchar c)
    {
        return juce::CharacterFunctions::isLetterOrDigit(c) || c == '_';
    }

    // Whether an opcode ("name=") or a header ("<") starts at index i
    bool opcodeOrHeaderStartsAt(const juce::String& text, int i)
    {
        if (i >= text.length())
            return false;
        if (text[i] == '<')
            return true;

        int j = i;
        while (j < text.length() && isNameChar(text[j]))
            ++j;
        return j > i && j < text.length() && text[j] == '=';
    }

    float toFloat(const juce::String& value) { return value.trim().getFloatValue(); }
    int toInt(const juce::String& value) { return value.trim().getIntValue(); }

    void apply(SfzRegion& region, const juce::String& name, const juce::String& value,
               const juce::String& defaultPath, juce::StringArray& warnings)
    {
        const auto note = [&](int fallback) {
            const int n = Sfz::parseNote(value);
            if (n < -1)
            {
                warnings.addIfNotAlreadyThere("bad note " + name + "=" + value);
                return fallback;
            }
            return n;
        };

        if (name == "sample")
            region.sample = (defaultPath + value.trim()).replaceCharacter('\\', '/');
        else if (name == "lokey")            region.lokey = note(region.lokey);
        else if (name == "hikey")            region.hikey = note(region.hikey);
        else if (name == "key")              region.lokey = region.hikey = region.keycenter = note(region.keycenter);
        else if (name == "pitch_keycenter")  region.keycenter = note(region.keycenter);
        else if (name == "pitch_keytrack")   region.keytrack = toFloat(value);
        else if (name == "lovel")            region.lovel = toInt(value);
        else if (name == "hivel")            region.hivel = toInt(value);
        else if (name == "tune")             region.tune = toFloat(value);
        else if (name == "transpose")        region.transpose = toInt(value);
        else if (name == "volume")           region.volume = toFloat(value);
        else if (name == "amplitude")        region.amplitude = toFloat(value);
        else if (name == "pan")              region.pan = toFloat(value);
        else if (name == "amp_veltrack")     region.ampVeltrack = toFloat(value);
        else if (name == "ampeg_attack")     region.ampegAttack = toFloat(value);
        else if (name == "ampeg_decay")      region.ampegDecay = toFloat(value);
        else if (name == "ampeg_sustain")    region.ampegSustain = toFloat(value);
        else if (name == "ampeg_release")    region.ampegRelease = toFloat(value);
        else if (name == "offset")           region.offset = value.trim().getLargeIntValue();
        else if (name == "end")              region.end = value.trim().getLargeIntValue();
        else if (name == "loop_start" || name == "loopstart")  region.loopStart = value.trim().getLargeIntValue();
        else if (name == "loop_end" || name == "loopend")      region.loopEnd = value.trim().getLargeIntValue();
        else if (name == "loop_mode" || name == "loopmode")
        {
            const auto mode = value.trim();
            if (mode == "no_loop")               region.loopMode = SfzRegion::LoopMode::NoLoop;
            else if (mode == "one_shot")         region.loopMode = SfzRegion::LoopMode::OneShot;
            else if (mode == "loop_continuous")  region.loopMode = SfzRegion::LoopMode::Continuous;
            else if (mode == "loop_sustain")     region.loopMode = SfzRegion::LoopMode::Sustain;
            else warnings.addIfNotAlreadyThere("unknown loop_mode " + mode);
        }
        else if (name == "seq_length")       region.seqLength = juce::jmax(1, toInt(value));
        else if (name == "seq_position")     region.seqPosition = juce::jmax(1, toInt(value));
        else if (name == "lorand")           region.lorand = toFloat(value);
        else if (name == "hirand")           region.hirand = toFloat(value);
        else if (name == "trigger")
        {
            const auto trigger = value.trim();
            region.releaseTrigger = trigger == "release" || trigger == "release_key";
        }
        else if (name == "sw_lokey")         region.swLokey = note(region.swLokey);
        else if (name == "sw_hikey")         region.swHikey = note(region.swHikey);
        else if (name == "sw_last")          region.swLast = note(region.swLast);
        else if (name == "sw_default")       region.swDefault = note(region.swDefault);
        else if (name.endsWith("_label") || name == "ampeg_dynamic")
            ;  // Names for a player's UI, and when envelope times are read: nothing to do
        else
            warnings.addIfNotAlreadyThere("ignored opcode " + name);
    }
}

namespace Sfz
{
    int parseNote(const juce::String& text)
    {
        const auto s = text.trim().toLowerCase();
        if (s.isEmpty())
            return -2;

        if (s.containsOnly("-0123456789"))
            return juce::jlimit(-1, 127, s.getIntValue());

        static const int semitones[] = { 9, 11, 0, 2, 4, 5, 7 };  // a b c d e f g
        const auto letter = s[0];
        if (letter < 'a' || letter > 'g')
            return -2;

        int semitone = semitones[letter - 'a'];
        int i = 1;
        if (s[i] == '#')      { ++semitone; ++i; }
        else if (s[i] == 'b') { --semitone; ++i; }

        const auto octave = s.substring(i);
        if (octave.isEmpty() || ! octave.containsOnly("-0123456789"))
            return -2;

        return juce::jlimit(0, 127, (octave.getIntValue() + 1) * 12 + semitone);
    }

    juce::File resolveSample(const juce::File& sfzFile, const juce::String& samplePath)
    {
        const auto path = samplePath.replaceCharacter('\\', '/');
        if (juce::File::isAbsolutePath(path))
            return juce::File(path);
        return sfzFile.getParentDirectory().getChildFile(path);
    }

    SfzInstrument parse(const juce::String& source)
    {
        enum class Level { None, Control, Global, Master, Group, Region, Other };

        SfzInstrument instrument;
        const auto text = stripComments(source);

        Opcodes global, master, group, region;
        juce::String defaultPath;
        Level level = Level::None;

        const auto finishRegion = [&] {
            if (level != Level::Region)
                return;
            SfzRegion r;
            for (const auto* opcodes : { &global, &master, &group, &region })
                for (const auto& [name, value] : *opcodes)
                    apply(r, name, value, defaultPath, instrument.warnings);
            if (r.sample.isEmpty())
                instrument.warnings.add("a region with no sample");
            else
                instrument.regions.push_back(r);
            region.clear();
        };

        int i = 0;
        const int length = text.length();
        while (i < length)
        {
            const auto c = text[i];

            if (juce::CharacterFunctions::isWhitespace(c))
            {
                ++i;
                continue;
            }

            if (c == '<')
            {
                const int close = text.indexOfChar(i, '>');
                if (close < 0)
                    break;
                const auto header = text.substring(i + 1, close).trim().toLowerCase();
                i = close + 1;

                finishRegion();
                if (header == "control")      { level = Level::Control; defaultPath.clear(); }
                else if (header == "global")  { level = Level::Global; global.clear(); master.clear(); group.clear(); }
                else if (header == "master")  { level = Level::Master; master.clear(); group.clear(); }
                else if (header == "group")   { level = Level::Group; group.clear(); }
                else if (header == "region")  { level = Level::Region; }
                else
                {
                    level = Level::Other;  // <curve>, <effect>, <midi>: not used
                    instrument.warnings.addIfNotAlreadyThere("ignored header <" + header + ">");
                }
                continue;
            }

            if (c == '#')  // #define and #include aren't supported
            {
                const int eol = text.indexOfChar(i, '\n');
                instrument.warnings.addIfNotAlreadyThere("ignored " + text.substring(i, eol < 0 ? length : eol).trim());
                i = eol < 0 ? length : eol + 1;
                continue;
            }

            // An opcode: name=value, where the value runs to the end of the
            // line or the next opcode or header (sample names can have spaces)
            const int equals = text.indexOfChar(i, '=');
            if (equals < 0)
                break;
            const auto name = text.substring(i, equals).trim().toLowerCase();
            if (! name.containsOnly("abcdefghijklmnopqrstuvwxyz0123456789_"))
            {
                // Stray text: skip a word of it
                while (i < length && ! juce::CharacterFunctions::isWhitespace(text[i]))
                    ++i;
                instrument.warnings.addIfNotAlreadyThere("stray text near " + name.substring(0, 20));
                continue;
            }

            int end = equals + 1;
            while (end < length && text[end] != '\n' && text[end] != '\r' && text[end] != '<')
            {
                if (juce::CharacterFunctions::isWhitespace(text[end]))
                {
                    int next = end;
                    while (next < length && (text[next] == ' ' || text[next] == '\t'))
                        ++next;
                    if (opcodeOrHeaderStartsAt(text, next))
                        break;
                }
                ++end;
            }
            const auto value = text.substring(equals + 1, end).trim();
            i = end;

            switch (level)
            {
                case Level::Control:
                    if (name == "default_path")
                    {
                        defaultPath = value.replaceCharacter('\\', '/');
                        if (defaultPath.isNotEmpty() && ! defaultPath.endsWithChar('/'))
                            defaultPath << '/';
                    }
                    else
                        instrument.warnings.addIfNotAlreadyThere("ignored opcode " + name);
                    break;
                case Level::Global:  global.emplace_back(name, value); break;
                case Level::Master:  master.emplace_back(name, value); break;
                case Level::Group:   group.emplace_back(name, value); break;
                case Level::Region:  region.emplace_back(name, value); break;
                case Level::None:    global.emplace_back(name, value); break;  // Before any header
                case Level::Other:   break;
            }
        }

        finishRegion();
        return instrument;
    }
}
