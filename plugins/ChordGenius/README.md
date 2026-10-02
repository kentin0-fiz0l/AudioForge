# 🎵 ChordGenius - Chord Progression Generator Plugin

**Status**: Core engine complete, UI in progress

## What It Does

ChordGenius is a VST3/AU plugin that generates chord progressions for songwriters and producers.

### Features

✅ **Completed**:
- Chord progression generator
- 8 genre templates (Pop, Rock, Jazz, Blues, EDM, Emotional, Happy, Sad)
- Music theory engine (scales, roman numerals, chord construction)
- Key/scale support (Major and Minor)
- Chord suggestions

🚧 **In Progress**:
- JUCE UI (chord display, buttons)
- MIDI output
- CMakeLists.txt build system

## Architecture

```
ChordGenius/
├── Source/
│   ├── ChordEngine.h/.cpp     ✅ Complete
│   ├── PluginProcessor.h/.cpp  🚧 Next
│   └── PluginEditor.h/.cpp     🚧 Next
└── CMakeLists.txt              🚧 Next
```

## Chord Engine API

```cpp
ChordEngine engine;

// Set key
engine.setKey(60, true);  // C Major

// Generate progression
auto chords = engine.generateProgression("pop", 4);
// Returns: I - V - vi - IV (C - G - Am - F)

// Suggest next chords
auto suggestions = engine.suggestNextChords(chords);
// Returns: smart suggestions based on theory
```

## Genre Templates

| Genre | Progression | Vibe |
|-------|-------------|------|
| **pop** | I - V - vi - IV | Classic pop |
| **rock** | I - IV - V - I | Rock standard |
| **jazz** | ii - V - I - vi | Jazz turnaround |
| **blues** | I - IV - I - V | 12-bar blues |
| **edm** | i - VI - III - VII | Minor EDM |
| **emotional** | vi - IV - I - V | Emotional/sad |
| **happy** | I - IV - I - V | Upbeat |
| **sad** | i - VII - VI - VII | Dark/melancholic |

## Next Steps

1. Create PluginProcessor (JUCE audio processor)
2. Create PluginEditor (UI with buttons and chord display)
3. Add MIDI output functionality
4. Create CMakeLists.txt
5. Build and test!

## Building

```bash
cd ~/Projects/Active/AudioForge/plugins/ChordGenius
cmake -B build
cmake --build build
```

## Installation

```bash
cp -r build/ChordGenius_artefacts/VST3/ChordGenius.vst3 \
  ~/Library/Audio/Plug-Ins/VST3/
```

---

**ChordGenius** - Never face writer's block again! 🎼✨
