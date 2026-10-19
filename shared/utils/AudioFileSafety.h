#pragma once

#include <juce_audio_formats/juce_audio_formats.h>
#include <filesystem>

#if JUCE_MAC
 #include <sys/mount.h>
#elif JUCE_LINUX || JUCE_BSD
 #include <sys/vfs.h>
#elif JUCE_WINDOWS
 // From <windows.h>, which isn't included here: it would clash with JUCE's names
 extern "C" __declspec(dllimport) unsigned int __stdcall GetDriveTypeW(const wchar_t*);
#endif

namespace AudioForge {
namespace FileSafety {

/**
 * Checks for files named by something the user didn't write themselves: a
 * saved project, an SFZ instrument, a preset from elsewhere.
 *
 * A file on another machine is refused unless it's in a library folder the
 * user approved (approvedRoots): on Windows, opening one sends the user's
 * credentials to that host, and anywhere it tells the host what was opened.
 * That covers \\host\share paths, mapped network drives, mounted shares and
 * automounts (/net). A file whose header claims an impossible sample rate or
 * channel count is refused before anything is allocated from those numbers.
 */

constexpr double minSampleRate = 8000.0;
constexpr double maxSampleRate = 384000.0;
constexpr int maxChannels = 8;

/** Whether a path names a file on another machine. */
inline bool isNetworkPath(const juce::String& path)
{
    const auto p = path.trimStart();
    return p.startsWith("\\\\") || p.startsWith("//") || p.startsWith("\\/") || p.startsWith("/\\")
        || p.contains("://");
}

/** `file`'s path as std::filesystem takes it (wide on Windows, so any name works). */
inline std::filesystem::path fsPath(const juce::File& file)
{
   #if JUCE_WINDOWS
    return std::filesystem::path(file.getFullPathName().toWideCharPointer());
   #else
    return std::filesystem::path(file.getFullPathName().toStdString());
   #endif
}

/**
 * Whether `file` is on another machine: a network path, or on a mounted
 * network volume. The volume is asked only of a path that's already mounted
 * or local: an automount's folder (/net, /Network) is refused by its name,
 * since looking inside it would mount it.
 */
inline bool isRemote(const juce::File& file)
{
    const auto path = file.getFullPathName();
    if (isNetworkPath(path))
        return true;
   #if JUCE_MAC
    if (path.startsWith("/net/") || path.startsWith("/Network/"))
        return true;
   #elif JUCE_LINUX || JUCE_BSD
    if (path.startsWith("/net/"))
        return true;
   #endif

    auto existing = file;  // the volume is asked of the deepest folder that exists
    while (! existing.exists() && existing.getParentDirectory() != existing)
        existing = existing.getParentDirectory();

   #if JUCE_MAC
    struct statfs fs;
    return statfs(existing.getFullPathName().toRawUTF8(), &fs) == 0 && (fs.f_flags & MNT_LOCAL) == 0;
   #elif JUCE_LINUX || JUCE_BSD
    struct statfs fs;
    if (statfs(existing.getFullPathName().toRawUTF8(), &fs) != 0)
        return false;
    switch ((unsigned long) fs.f_type)
    {
        case 0x6969:      // NFS
        case 0x517B:      // SMB
        case 0xFF534D42:  // CIFS
        case 0xFE534D42:  // SMB2
        case 0x73757245:  // Coda
        case 0x5346414F:  // AFS
        case 0x01021997:  // 9p
            return true;
        default:
            return false;
    }
   #elif JUCE_WINDOWS
    const auto drive = path.substring(0, 3);  // "Z:\"
    return drive.length() == 3 && drive[1] == ':' && GetDriveTypeW(drive.toWideCharPointer()) == 4;  // DRIVE_REMOTE
   #else
    return false;
   #endif
}

/**
 * Where the user's approved library folders are listed, one per line. Tests
 * point it elsewhere.
 */
inline juce::File& rootsFile()
{
    static juce::File file = []
    {
        auto settings = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
       #if JUCE_MAC
        settings = settings.getChildFile("Application Support");  // JUCE gives ~/Library itself
       #endif
        return settings.getChildFile("AudioForge").getChildFile("library-roots.txt");
    }();
    return file;
}

/**
 * The library folders the user approved: those in rootsFile, and those in
 * AUDIOFORGE_LIBRARY_ROOTS (separated as PATH is, for the render host).
 */
inline juce::StringArray approvedRoots()
{
    juce::StringArray roots;
    if (rootsFile().existsAsFile())
        roots.addLines(rootsFile().loadFileAsString());
   #if JUCE_WINDOWS
    roots.addTokens(juce::SystemStats::getEnvironmentVariable("AUDIOFORGE_LIBRARY_ROOTS", {}), ";", {});
   #else
    roots.addTokens(juce::SystemStats::getEnvironmentVariable("AUDIOFORGE_LIBRARY_ROOTS", {}), ":", {});
   #endif
    roots.trim();
    roots.removeEmptyStrings();
    return roots;
}

/** Whether `file` is in an approved library folder (only its name is read). */
inline bool isApproved(const juce::File& file)
{
    const auto path = file.getFullPathName();
    for (const auto& root : approvedRoots())
    {
        if (! juce::File::isAbsolutePath(root))
            continue;
        const auto folder = juce::File(root).getFullPathName();
       #if JUCE_WINDOWS || JUCE_MAC
        const bool inside = path.equalsIgnoreCase(folder) || path.startsWithIgnoreCase(folder + juce::File::getSeparatorString());
       #else
        const bool inside = path == folder || path.startsWith(folder + juce::File::getSeparatorString());
       #endif
        if (inside && ! path.contains(juce::String("..") + juce::File::getSeparatorString()))
            return true;
    }
    return false;
}

/** Adds `folder` to the approved library folders (the user chose a file in it). */
inline void approve(const juce::File& folder)
{
    if (isApproved(folder))
        return;
    rootsFile().getParentDirectory().createDirectory();
    rootsFile().appendText(folder.getFullPathName() + "\n");
}

/** Why `file` isn't loaded from where it is, or an empty string if it may be. */
inline juce::String refusal(const juce::File& file)
{
    const auto path = file.getFullPathName();
    if (path.contains("://"))
        return "URLs aren't loaded: " + path;
    if (isApproved(file) || ! isRemote(file))
        return {};
    return "network files load only from an approved library folder (choose one by hand, or list it in "
           + rootsFile().getFullPathName() + "): " + path;
}

/**
 * The same for a path as a saved state or an SFZ wrote it (`named`), once
 * resolved: a network name is refused as written, before resolving can make
 * it look local.
 */
inline juce::String refusal(const juce::String& named, const juce::File& resolved)
{
    if (isNetworkPath(named) && ! isApproved(resolved))
        return "network paths aren't loaded: " + named;
    return refusal(resolved);
}

/** Why a reader's header can't be trusted, or an empty string if it can. */
inline juce::String checkReader(const juce::AudioFormatReader& reader)
{
    if (! (reader.sampleRate >= minSampleRate && reader.sampleRate <= maxSampleRate))
        return "unsupported sample rate " + juce::String(reader.sampleRate) + " Hz";
    if (reader.numChannels < 1 || reader.numChannels > (unsigned int) maxChannels)
        return "unsupported channel count " + juce::String(reader.numChannels);
    if (reader.lengthInSamples <= 0)
        return "no audio";
    return {};
}

constexpr juce::int64 maxTextBytes = 16 * 1024 * 1024;

/**
 * Reads a text file (an SFZ instrument) into `text`, or says in `error` why
 * not. Only a regular file is read (not a device or a pipe), and no more than
 * maxBytes of it: anything longer is refused rather than cut short.
 */
inline bool readText(const juce::File& file, juce::String& text, juce::String& error,
                     juce::int64 maxBytes = maxTextBytes)
{
    std::error_code ec;
    if (! std::filesystem::is_regular_file(fsPath(file), ec))
    {
        error = "not a regular file: " + file.getFullPathName();
        return false;
    }
    juce::FileInputStream in(file);
    if (! in.openedOk())
    {
        error = "can't open " + file.getFullPathName();
        return false;
    }
    juce::MemoryBlock data;
    in.readIntoMemoryBlock(data, (juce::pointer_sized_int) (maxBytes + 1));
    if ((juce::int64) data.getSize() > maxBytes)
    {
        error = "larger than " + juce::String(maxBytes / (1024 * 1024)) + " MB: " + file.getFullPathName();
        return false;
    }
    text = juce::String::createStringFromData(data.getData(), (int) data.getSize());
    return true;
}

} // namespace FileSafety
} // namespace AudioForge
