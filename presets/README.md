# AudioForge Preset Library

Comprehensive preset collection for all 14 AudioForge instrument plugins.

## 📁 Directory Structure

```
presets/
├── AcousticInstruments/
│   ├── ElectricPiano_Presets.json (8 presets)
│   ├── BrassSection_Presets.json (5 presets)
│   ├── StringEnsemble_Presets.json (5 presets)
│   └── AcousticBass_Presets.json (5 presets)
├── WorldInstruments/
│   ├── Sitar_Presets.json (5 presets)
│   ├── Koto_Presets.json (5 presets)
│   └── Shakuhachi_Presets.json (5 presets)
├── AnalogSynths/
│   ├── ClassicMonosynth_Presets.json (6 presets)
│   ├── Polysynth_Presets.json (5 presets)
│   └── FMSynth_Presets.json (5 presets)
└── DrumSynthesis/
    ├── AnalogKick_Presets.json (8 presets)
    ├── SnareSynth_Presets.json (6 presets)
    ├── HiHat_Presets.json (6 presets)
    └── TomSynth_Presets.json (6 presets)
```

## 📊 Preset Statistics

- **Total Plugins**: 14
- **Total Presets**: 80
- **Format**: JSON (human-readable and easily extensible)

### Breakdown by Pack

| Pack | Plugins | Presets |
|------|---------|---------|
| Acoustic Instruments | 4 | 23 |
| World Instruments | 3 | 15 |
| Analog Synths | 3 | 16 |
| Drum Synthesis | 4 | 26 |

## 🎵 Featured Presets by Plugin

### Acoustic Instruments Pack

**ElectricPiano** (8 presets)
- Classic Rhodes - Warm vintage electric piano
- Bell Piano - Bright, bell-like tone
- Tremolo Heaven - Heavy tremolo psychedelic vibes
- Dreamy Pad - Long sustain with chorus

**BrassSection** (5 presets)
- Big Band Stab - Punchy brass hit
- Smooth Jazz - Warm, mellow ensemble
- Fanfare - Bright, celebratory brass

**StringEnsemble** (5 presets)
- Lush Strings - Rich orchestral section
- Pizzicato - Plucked articulation
- Slow Swell - Gradual crescendo

**AcousticBass** (5 presets)
- Upright Jazz - Classic walking bass
- Slap Funk - Aggressive slap technique
- Smooth Fingerstyle - Warm fingered bass

### World Instruments Pack

**Sitar** (5 presets)
- Raga Bhairavi - Classic Indian raga
- Heavy Buzz - Maximum jawari bridge buzz
- Drone Meditation - Strong tanpura background

**Koto** (5 presets)
- In-sen Scale - Traditional Japanese tuning
- Tremolo Waves - Atmospheric textures
- Percussive Attack - Sharp rhythmic playing

**Shakuhachi** (5 presets)
- Gentle Breath - Soft, airy flute
- Meri Bend Down - Lowered pitch technique
- Breathy Meditation - Maximum breath noise

### Analog Synth Pack

**ClassicMonosynth** (6 presets)
- Fat Bass - Thick analog bass with filter sweep
- Screaming Lead - Bright aggressive lead
- Acid Bass - Classic 303-style acid
- Wobble Bass - LFO-modulated dubstep bass

**Polysynth** (5 presets)
- Supersaw Lead - Classic trance supersaw
- Warm Pad - Lush ambient pad
- Vintage String - Analog string machine

**FMSynth** (5 presets)
- Electric Piano - Classic FM EP
- Brass - Bright FM brass
- Bell - Metallic bell tone
- Bass - Fat FM bass
- Pad - Evolving FM pad

### Drum Synthesis Pack

**AnalogKick** (8 presets)
- 808 Classic - Iconic TR-808 kick
- 909 Punch - TR-909 punchy kick
- Deep Sub - Low-frequency sub kick
- Hard Techno - Distorted techno kick

**SnareSynth** (6 presets)
- 808 Snare - Classic TR-808
- 909 Snare - TR-909 punchy
- Fat Crack - Big, fat snare hit
- Rim Shot - Emphasis on attack

**HiHat** (6 presets)
- 808 Closed/Open - TR-808 styles
- 909 Closed/Open - TR-909 styles
- Tight Sizzle - Bright, short
- Washy - Long, open hat

**TomSynth** (6 presets)
- High/Mid/Low/Floor Tom - Complete tom kit
- Tight Tom - Short, tight hit
- Boomy Tom - Long, resonant

## 🔧 File Format

Each preset file is a JSON document with the following structure:

```json
{
  "plugin": "PluginName",
  "version": "1.0.0",
  "presets": [
    {
      "name": "Preset Name",
      "description": "Brief description of the sound",
      "parameters": {
        "parameterName": value,
        ...
      }
    }
  ]
}
```

### Parameter Value Ranges

- Most parameters are normalized to **0.0 - 1.0** range
- Some parameters use specific ranges (e.g., pitch in Hz, time in ms)
- All values are documented in the respective plugin manuals

## 💡 Usage Tips

1. **Start with presets** - Load a preset as a starting point, then tweak to taste
2. **Compare presets** - Try multiple presets on the same part to find the best fit
3. **Layer sounds** - Combine multiple plugins for rich, complex textures
4. **Save your edits** - When you modify a preset, save it as your own

## 🔄 Preset Loading (Coming Soon)

Future plugin updates will include:
- Built-in preset browser
- One-click preset loading
- Preset favorites/tagging
- User preset creation and sharing

## 📝 Contributing Custom Presets

To add your own presets:

1. Follow the JSON format above
2. Ensure all parameter names match the plugin's parameters
3. Keep values within valid ranges
4. Provide clear names and descriptions
5. Test presets thoroughly before sharing

## 🎯 Preset Philosophy

All presets are designed with these principles:

- **Musical First** - Every preset sounds good in a musical context
- **Variety** - Wide range of sonic possibilities per plugin
- **Labeled Clearly** - Names and descriptions communicate the sound
- **Production-Ready** - Mix-ready sounds that work immediately
- **Educational** - Presets demonstrate the plugin's capabilities

## 📚 See Also

- [AudioForge Plugin Collection](../README.md)
- [Plugin Documentation](../docs/)
- [Build Instructions](../BUILD.md)

---

**AudioForge Preset Library v1.0.0**  
80 hand-crafted presets for 14 professional instruments

Created with ❤️ for the AudioForge community
