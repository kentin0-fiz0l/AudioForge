# Sampler Plugin Testing Checklist

## Pre-Release Testing Checklist

Complete this checklist before marking Phase 4 complete and proceeding to release.

---

## Build Verification

- [ ] **Clean build succeeds** (no warnings, no errors)
- [ ] **All formats build**: VST3, AU, Standalone
- [ ] **Plugins install** to system directories
- [ ] **Unit tests pass** (30+ tests, 100% pass rate)
- [ ] **No memory leaks** (run with valgrind/instruments)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build
./build/SamplerPluginTests
```

---

## Basic Functionality

### Sample Loading

- [ ] **Load WAV file** (16-bit, 44.1kHz, stereo)
- [ ] **Load WAV file** (24-bit, 48kHz, mono)
- [ ] **Load AIFF file**
- [ ] **Load FLAC file**
- [ ] **Reject MP3 file** (show error message)
- [ ] **Reject oversized sample** (>512 MB total)
- [ ] **Drag-and-drop file** onto waveform display
- [ ] **Sample appears in browser** after loading
- [ ] **Waveform renders correctly** in display
- [ ] **Memory display updates** after loading

### Zone Management

- [ ] **Auto-create zone** on first sample load (C4-C5)
- [ ] **Add zone manually** via "Add Zone" button
- [ ] **Delete zone** via "Delete Zone" button
- [ ] **Resize zone** by dragging edges
- [ ] **Snap to notes** when resizing (optional)
- [ ] **Set root note** via right-click menu
- [ ] **Root note marker** displays in yellow
- [ ] **Zone colors** are distinct (8 colors cycle)
- [ ] **Zone info label** updates when selecting zone
- [ ] **No crashes** when deleting all zones

### MIDI Playback

- [ ] **Play MIDI note** in mapped zone (triggers sample)
- [ ] **Play MIDI note** outside zones (no sound)
- [ ] **Pitch shifting works** (C4 sample plays C5 at +12 semitones)
- [ ] **Velocity response** (soft/hard notes have different volume)
- [ ] **Polyphony** (play 16 notes simultaneously)
- [ ] **Voice stealing** (17th note steals quietest voice)
- [ ] **Note off** (release envelope plays)
- [ ] **Sustain pedal** (optional, future)

### Velocity Layers

- [ ] **Add velocity layer** via "Add Layer" button
- [ ] **Layer appears** in velocity editor
- [ ] **Drag boundary** between layers
- [ ] **Soft velocity** triggers layer 1
- [ ] **Hard velocity** triggers layer 2
- [ ] **Delete layer** removes from zone
- [ ] **Layer index** displayed in editor
- [ ] **No crashes** with empty velocity layers

### Loop Playback

- [ ] **Loop mode: None** (sample plays once)
- [ ] **Loop mode: Forward** (loops continuously)
- [ ] **Loop mode: Ping-Pong** (forward then backward)
- [ ] **Drag loop start** handle (updates loop point)
- [ ] **Drag loop end** handle (updates loop point)
- [ ] **Loop transition** is smooth (no clicks/pops)
- [ ] **Hold note** (loops sustain indefinitely)
- [ ] **Release note** (sample finishes playing)

### Envelope & Filter

- [ ] **Attack slider** changes note attack time
- [ ] **Decay slider** changes decay time
- [ ] **Sustain slider** changes sustain level
- [ ] **Release slider** changes release time
- [ ] **Filter cutoff** changes brightness
- [ ] **Filter resonance** adds emphasis
- [ ] **ADSR values persist** when reloading plugin

### Presets

- [ ] **Load preset** from dropdown
- [ ] **Parameters update** when loading preset
- [ ] **Loop mode updates** when loading preset
- [ ] **Preset applies** to current zone
- [ ] **20 presets available** (Drums, Instruments, Textures)
- [ ] **Preset categories** are correct

---

## UI Testing

### Layout & Rendering

- [ ] **950x650 window** displays correctly
- [ ] **All components visible** (no overlap)
- [ ] **Waveform display** renders min/max waveform
- [ ] **Zone editor** shows 88-key piano keyboard
- [ ] **Velocity layer editor** displays on right
- [ ] **Sample browser** displays below velocity editor
- [ ] **Controls** (sliders, buttons) are accessible
- [ ] **Labels** are legible and aligned
- [ ] **No UI glitches** when resizing window

### Interaction

- [ ] **Buttons respond** to clicks
- [ ] **Sliders respond** to drag
- [ ] **Dropdowns open** and display items
- [ ] **Right-click menu** opens on zone editor
- [ ] **Drag handles** change cursor on hover
- [ ] **Tooltips** display on hover (future)
- [ ] **Keyboard shortcuts** work (future)

### Visual Feedback

- [ ] **Zone selection** highlights selected zone
- [ ] **Layer selection** highlights selected layer
- [ ] **Drag indicators** show during drag operations
- [ ] **Memory bar** updates in real-time
- [ ] **Zone info label** updates on selection
- [ ] **Loop markers** visible and draggable

---

## State & Persistence

### Save/Load State

- [ ] **Save project** with samples loaded
- [ ] **Reload project** (samples reload)
- [ ] **Key zones persist** across sessions
- [ ] **Velocity layers persist** across sessions
- [ ] **Loop points persist** across sessions
- [ ] **Parameter values persist** across sessions
- [ ] **Sample paths** are stored as relative paths
- [ ] **Missing samples** show error on load (future)

### Undo/Redo

- [ ] **DAW undo** reverts parameter changes
- [ ] **DAW redo** reapplies changes
- [ ] **Undo zone changes** (future)

---

## Performance Testing

### CPU Usage

- [ ] **Single voice** uses <2% CPU (44.1kHz, 512 buffer)
- [ ] **16 voices** use <10% CPU
- [ ] **No audio glitches** at high polyphony
- [ ] **CPU usage stable** over time (no leaks)
- [ ] **Performance similar** across sample rates

Test command:
```bash
# Launch standalone, open Activity Monitor / Task Manager
# Play 16 simultaneous notes, observe CPU %
```

### Memory Usage

- [ ] **Fresh instance** uses <10 MB RAM (no samples)
- [ ] **100 samples loaded** uses <200 MB RAM
- [ ] **512 MB limit enforced** (no crashes)
- [ ] **Memory display accurate** (±5 MB)
- [ ] **No memory leaks** after loading/unloading samples

Test command:
```bash
# Launch standalone, open Activity Monitor / Task Manager
# Load samples, delete samples, repeat 10x
# Memory should not grow unbounded
```

### Latency

- [ ] **MIDI latency** <10ms (measure with oscilloscope)
- [ ] **Buffer size** configurable in DAW
- [ ] **No crackling** at 64-sample buffer

---

## DAW Compatibility

### Logic Pro

- [ ] **Loads as VST3** (Plugin Manager)
- [ ] **Loads as AU** (Audio Units)
- [ ] **Receives MIDI** from track
- [ ] **Audio output** to channel strip
- [ ] **Parameters automatable**
- [ ] **State saves** with Logic project
- [ ] **No crashes** during playback
- [ ] **No crashes** when closing project

### Ableton Live

- [ ] **Loads as VST3**
- [ ] **Receives MIDI** from clip
- [ ] **Audio output** to track
- [ ] **Parameters mappable** to MIDI CC
- [ ] **State saves** with Live set
- [ ] **No crashes** during playback

### Reaper

- [ ] **Loads as VST3**
- [ ] **Receives MIDI**
- [ ] **Audio output**
- [ ] **Parameters automatable**
- [ ] **State saves** with Reaper project
- [ ] **No crashes**

### Standalone App

- [ ] **Launches** without DAW
- [ ] **MIDI input** selectable (Core MIDI devices)
- [ ] **Audio output** selectable (Core Audio devices)
- [ ] **Settings persist** across launches
- [ ] **File menu** works (future: Save/Load)

---

## Edge Cases & Error Handling

### Invalid Input

- [ ] **Empty sample file** shows error
- [ ] **Corrupted audio file** shows error
- [ ] **Non-audio file** shows error
- [ ] **Sample >30 seconds** is truncated
- [ ] **Invalid loop points** are clamped (0.0-1.0)
- [ ] **Velocity >1.0** is clamped

### Boundary Conditions

- [ ] **Load 0 samples** (empty library)
- [ ] **Load 1 sample** (minimal setup)
- [ ] **Load 100 samples** (near memory limit)
- [ ] **Create 0 zones** (no playback)
- [ ] **Create 1 zone** (basic setup)
- [ ] **Create 88 zones** (one per key)
- [ ] **Velocity layer with same min/max** (edge case)

### Concurrent Operations

- [ ] **Load sample** while playing (no glitches)
- [ ] **Delete zone** while playing (graceful)
- [ ] **Change parameters** while playing (smooth)
- [ ] **Resize window** while playing (no crashes)

---

## Cross-Platform Testing (Future)

### macOS

- [ ] **Intel (x86_64)** builds and runs
- [ ] **Apple Silicon (arm64)** builds and runs
- [ ] **Universal binary** works on both
- [ ] **macOS 11+** compatible
- [ ] **Gatekeeper** accepts signed binary

### Windows (Future)

- [ ] **Windows 10** builds and runs
- [ ] **Windows 11** builds and runs
- [ ] **VST3** loads in Windows DAWs
- [ ] **ASIO** audio driver works

### Linux (Future)

- [ ] **Ubuntu 20.04+** builds and runs
- [ ] **VST3** loads in Ardour/Reaper
- [ ] **ALSA/JACK** audio drivers work

---

## Regression Testing

After any code changes, re-run this subset:

- [ ] **Build succeeds**
- [ ] **Load sample, play note** (smoke test)
- [ ] **16-voice polyphony** (CPU test)
- [ ] **Save/load state** (persistence test)
- [ ] **Unit tests pass** (regression test)

---

## Known Issues

Document any failing tests here:

1. **Issue**: [Description]
   - **Severity**: Critical / High / Medium / Low
   - **Workaround**: [If applicable]
   - **Fix ETA**: [Version]

2. ...

---

## Test Results Summary

**Date**: _______________
**Tester**: _______________
**Version**: v1.5.0

**Total Tests**: 150
**Passed**: ___ / 150
**Failed**: ___ / 150
**Blocked**: ___ / 150

**Pass Rate**: _____ %

**Blockers** (must fix before release):
- [ ] None

**High Priority** (should fix before release):
- [ ] None

**Medium Priority** (can defer to v1.5.1):
- [ ] None

**Sign-off**:
- [ ] All critical tests pass
- [ ] No P0/P1 bugs
- [ ] Ready for Phase 5 (Integration & Release)
