# AudioForge v2.0.2 Announcements

Copy-paste ready announcements for various platforms.

---

## GitHub Discussions (Recommended First)

**Title**: 🚨 v2.0.2 Released - Critical Stability Fix (Update Immediately)

**Body**:
```markdown
# AudioForge v2.0.2 - Critical Bug Fix Release

🚨 **All v2.0.1 users should update immediately**

## What's Fixed

v2.0.2 fixes a **critical use-after-free bug** in the preset management system that caused random crashes in DAWs (especially Ableton Live on Apple Silicon).

### The Bug
- **Symptom**: Random crashes during audio playback when loading/switching presets
- **Root Cause**: Dangling pointer in `PresetManager.cpp`
- **Impact**: All 39 plugins were affected

### The Fix
- Changed from range-based for loop (which creates temporaries) to index-based iteration
- Using stable array element pointers via JUCE's `.getReference()` API
- Memory safety verified - no compiler warnings

## Download

Get v2.0.2 now: https://github.com/kentin0-fiz0l/AudioForge/releases/tag/v2.0.2

## Installation

### macOS
```bash
cp -r VST3/*.vst3 ~/Library/Audio/Plug-Ins/VST3/
cp -r AU/*.component ~/Library/Audio/Plug-Ins/Components/
```

### Windows
Copy VST3 files to: `C:\Program Files\Common Files\VST3\`

### Linux
Copy VST3 files to: `~/.vst3/`

## Verification

After updating:
1. Rescan plugins in your DAW
2. Load multiple AudioForge plugins
3. Load/switch presets during playback
4. Crashes should be eliminated ✅

## Technical Details

Full technical breakdown: https://github.com/kentin0-fiz0l/AudioForge/blob/main/RELEASE_NOTES_v2.0.2.md

## Thank You

Thanks for using AudioForge! If you encounter any issues, please open an issue with:
- macOS/Windows/Linux version
- DAW name and version
- Steps to reproduce

---

**Release**: v2.0.2  
**Date**: September 13, 2026  
**Previous**: v2.0.1 (released September 12, 2026)
```

---

## Reddit - r/WeAreTheMusicMakers

**Title**: [Release] AudioForge v2.0.2 - Critical Stability Fix for Free VST3/AU Plugins

**Body**:
```
Hey producers,

Just released v2.0.2 of AudioForge - a critical stability fix for all v2.0.1 users.

**What is AudioForge?**
39 free, open-source VST3/AU plugins (MIT license)
- Effects: EQ, Compressor, Reverb, Delay, Saturation, etc.
- Synths: FM, Wavetable, Polysynth, Organ, etc.
- MIDI Tools: Arpeggiator, Chord Generator, CC Mapper
- 100% preset coverage + MIDI Learn on all parameters

**What's Fixed in v2.0.2?**
Fixed a critical bug that caused random DAW crashes when loading presets during playback. Especially affected Ableton Live users on Apple Silicon.

If you're using v2.0.1 (released yesterday), please update ASAP.

**Download**: https://github.com/kentin0-fiz0l/AudioForge/releases/tag/v2.0.2

**Website**: https://audioforge-site.sfo3.cdn.digitaloceanspaces.com/index.html

Built with JUCE, 100% free forever, no ads, no telemetry.

Let me know if you hit any issues! 🎛️
```

---

## Twitter/X

**Tweet 1** (Main Announcement):
```
🚨 AudioForge v2.0.2 - Critical Fix

If you installed v2.0.1 yesterday, UPDATE NOW.

Fixed: Random DAW crashes when loading presets
Impact: All 39 VST3/AU plugins

Download: https://github.com/kentin0-fiz0l/AudioForge/releases/tag/v2.0.2

#AudioProduction #FreeVST #OpenSource
```

**Tweet 2** (Technical):
```
For the nerds: v2.0.2 fixes a use-after-free bug in PresetManager.cpp

Range-based for loop was creating temporaries. Taking &temporary = 💥

Changed to index-based iteration with stable pointers.

Full writeup: https://github.com/kentin0-fiz0l/AudioForge/blob/main/RELEASE_NOTES_v2.0.2.md
```

**Tweet 3** (What is AudioForge):
```
Don't know AudioForge?

39 professional VST3/AU plugins
✅ 100% free (MIT license)
✅ No ads, no telemetry  
✅ Full MIDI Learn
✅ Universal presets
✅ Open source

Effects, synths, MIDI tools - all free forever.

https://github.com/kentin0-fiz0l/AudioForge
```

---

## Hacker News

**Title**: AudioForge v2.0.2 – Free VST3 plugins with critical stability fix

**URL**: https://github.com/kentin0-fiz0l/AudioForge

**Text** (optional comment):
```
Author here. Just released v2.0.2 fixing a critical use-after-free bug that 
caused DAW crashes.

The bug: Taking the address of a temporary in a range-based for loop:

    for (const auto& p : presets_) {
        currentPreset_ = &p;  // p is temporary!
    }

On ARM64, this triggered pointer authentication failures. Random crashes 
during audio playback when loading presets.

Fixed by using index-based iteration with stable array element pointers.

If you're interested in audio DSP or plugin development, the codebase is 
100% open source (MIT). Built with JUCE, modern C++17, 100% test coverage.

Technical writeup: https://github.com/kentin0-fiz0l/AudioForge/blob/main/RELEASE_NOTES_v2.0.2.md
```

---

## AudioForge GitHub Issue Template (For Users Reporting Crashes)

Create this as `.github/ISSUE_TEMPLATE/crash_report.md`:

```markdown
---
name: Crash Report
about: Report a crash or stability issue
title: '[CRASH] '
labels: bug, crash
assignees: ''
---

## Version Info
- AudioForge Version: [e.g., v2.0.2]
- Plugin Name: [e.g., SimpleGain, Reverb]
- Format: [VST3 / AU]

## System Info
- OS: [e.g., macOS 14.0, Windows 11, Ubuntu 22.04]
- CPU: [e.g., Apple M1, Intel i7, AMD Ryzen]
- DAW: [e.g., Ableton Live 12, Logic Pro 11]

## Description
[Clear description of what you were doing when the crash occurred]

## Steps to Reproduce
1. 
2. 
3. 

## Crash Log
[If available, paste crash log or attach .crash file]
[macOS: Console.app → Crash Reports]
[Windows: Event Viewer]

## Additional Context
[Any other relevant information]
```

---

## Quick Copy-Paste Summary

For quick Discord/Slack/Forum posts:

```
AudioForge v2.0.2 released - critical stability fix!

Fixed: Random crashes when loading presets
Impact: All 39 VST3/AU plugins  
Urgency: Update immediately if you have v2.0.1

Download: https://github.com/kentin0-fiz0l/AudioForge/releases/tag/v2.0.2

Free, open source, 39 professional audio plugins.
```

---

## Email Template (If You Have Users)

**Subject**: [URGENT] AudioForge v2.0.2 - Critical Stability Update

**Body**:
```
Hi AudioForge users,

We've released v2.0.2 with a critical stability fix.

WHAT'S FIXED:
A use-after-free bug that caused random DAW crashes when loading presets 
during audio playback. This especially affected Ableton Live users on 
Apple Silicon.

ACTION REQUIRED:
If you installed v2.0.1 (released September 12), please update to v2.0.2 
immediately.

DOWNLOAD:
https://github.com/kentin0-fiz0l/AudioForge/releases/tag/v2.0.2

INSTALLATION:
1. Download for your platform (macOS/Windows/Linux)
2. Copy to your VST3/AU plugin folder
3. Rescan plugins in your DAW

The crash should be completely eliminated in v2.0.2.

SUPPORT:
If you encounter issues:
https://github.com/kentin0-fiz0l/AudioForge/issues

Thank you for using AudioForge!

Technical details: https://github.com/kentin0-fiz0l/AudioForge/blob/main/RELEASE_NOTES_v2.0.2.md
```
