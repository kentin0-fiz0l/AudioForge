# Screenshot Capture Session 1 - Interactive Checklist

**Date**: September 1, 2026  
**Duration**: 2 hours (7 minutes per plugin)  
**Plugins**: SimpleGain, SimpleEQ, SimpleComp, CleanDelay, Saturation, Reverb  
**Output**: 30 professional screenshots

---

## Pre-Session Setup (5 minutes)

### macOS Screenshot Configuration

```bash
# Set PNG format (if not already done)
defaults write com.apple.screencapture type png

# Disable shadows for cleaner captures
defaults write com.apple.screencapture disable-shadow -bool true

# Apply changes
killall SystemUIServer
```

**Screenshot Shortcuts**:
- **Cmd+Shift+4 → Space**: Capture window (no shadow)
- **Cmd+Shift+4 → Drag**: Capture selection area

### DAW Setup

**Logic Pro X** (Recommended):
1. Open new empty project
2. Create one Audio track
3. Import `test-audio/pink-noise-10s.wav` to track
4. Set Dark theme: `Logic Pro → Preferences → Display → Appearance: Dark`
5. Buffer size: 512 samples (stable meters)

**Reaper** (Alternative):
1. Open new empty project
2. Insert new track
3. Drag `test-audio/pink-noise-10s.wav` to track
4. Theme: `Options → Themes → Default_6.0`
5. Buffer size: 512 samples

---

## Plugin 1: SimpleGain (7 minutes)

**Test Audio**: `pink-noise-10s.wav` (already loaded)  
**Time**: 17:25 - 17:32

### Screenshot 1: Default State
- [ ] Load SimpleGain on track
- [ ] **Reset to defaults** (`Initialize Preset`)
- [ ] **Stop playback** (silent meters)
- [ ] **Parameters**: Gain = 0.0 dB, all defaults
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/SimpleGain-01-default.png`

### Screenshot 2: Active Processing
- [ ] **Start playback** (Loop enabled)
- [ ] **Set Gain**: -6.0 dB
- [ ] **Wait 3 seconds** (meters stabilize at ~-18 dBFS)
- [ ] **Verify**: Input meter at -12 dBFS, output at -18 dBFS
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/SimpleGain-02-active.png`

### Screenshot 3: Extreme Settings
- [ ] **Set Gain**: +12.0 dB (boost)
- [ ] **Meters**: Output clipping (red indicators)
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/SimpleGain-03-extreme.png`

### Screenshot 4: In-DAW Context
- [ ] **Zoom DAW out** (show mixer + plugin)
- [ ] **Frame**: Plugin window (center) + 2-3 tracks visible
- [ ] **Capture**: Cmd+Shift+4 → Drag selection
- [ ] **Save as**: `raw/SimpleGain-04-daw.png`

### Screenshot 5: Detail Shot
- [ ] **Focus**: Gain meter display (large VU meter)
- [ ] **Set playback** (meters active)
- [ ] **Capture**: Cmd+Shift+4 → Drag around meter only
- [ ] **Save as**: `raw/SimpleGain-05-detail.png`

**✓ SimpleGain Complete (5 screenshots)**

---

## Plugin 2: SimpleEQ (7 minutes)

**Test Audio**: `pink-noise-10s.wav` (same track)  
**Time**: 17:32 - 17:39

### Screenshot 1: Default State
- [ ] **Unload SimpleGain**, load SimpleEQ
- [ ] **Reset to defaults** (flat EQ curve)
- [ ] **Stop playback**
- [ ] **Parameters**: All bands at 0.0 dB, filters off
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/SimpleEQ-01-default.png`

### Screenshot 2: Active Processing
- [ ] **Start playback**
- [ ] **Band 1 (Low)**: Bell, +3.0 dB at 100 Hz, Q=1.0
- [ ] **Band 2 (Mid)**: Bell, +2.0 dB at 1000 Hz, Q=0.7
- [ ] **Band 3 (High)**: Shelf, -2.0 dB at 8000 Hz
- [ ] **HP Filter**: Enabled at 80 Hz
- [ ] **Wait**: EQ curve visible on graph
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/SimpleEQ-02-active.png`

### Screenshot 3: Extreme Settings
- [ ] **Band 1**: +10.0 dB at 80 Hz (bass boost)
- [ ] **Band 2**: +10.0 dB at 3000 Hz (mid boost)
- [ ] **Band 3**: +10.0 dB at 12000 Hz (treble boost)
- [ ] **Result**: Exaggerated smiley-face EQ curve
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/SimpleEQ-03-extreme.png`

### Screenshot 4: In-DAW Context
- [ ] **Zoom DAW out**
- [ ] **Capture**: Plugin + mixer context
- [ ] **Save as**: `raw/SimpleEQ-04-daw.png`

### Screenshot 5: Detail Shot
- [ ] **Focus**: EQ curve graph display
- [ ] **Capture**: Cmd+Shift+4 → Drag around graph only
- [ ] **Save as**: `raw/SimpleEQ-05-detail.png`

**✓ SimpleEQ Complete (5 screenshots)**

---

## Plugin 3: SimpleComp (7 minutes)

**Test Audio**: `pink-noise-10s.wav`  
**Time**: 17:39 - 17:46

### Screenshot 1: Default State
- [ ] **Unload SimpleEQ**, load SimpleComp
- [ ] **Reset to defaults**
- [ ] **Stop playback**
- [ ] **Parameters**: Ratio 1:1 (no compression), Threshold 0 dB
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/SimpleComp-01-default.png`

### Screenshot 2: Active Processing
- [ ] **Start playback**
- [ ] **Threshold**: -10.0 dB
- [ ] **Ratio**: 4:1
- [ ] **Attack**: 10 ms
- [ ] **Release**: 100 ms
- [ ] **Verify**: Gain Reduction meter showing 4-6 dB GR
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/SimpleComp-02-active.png`

### Screenshot 3: Extreme Settings
- [ ] **Threshold**: -20.0 dB (very low)
- [ ] **Ratio**: 20:1 (limiter mode)
- [ ] **Attack**: 0.1 ms (instant)
- [ ] **Release**: 10 ms (fast)
- [ ] **Result**: 10+ dB gain reduction (heavy squash)
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/SimpleComp-03-extreme.png`

### Screenshot 4: In-DAW Context
- [ ] **Zoom DAW out**
- [ ] **Capture**: Plugin + mixer context
- [ ] **Save as**: `raw/SimpleComp-04-daw.png`

### Screenshot 5: Detail Shot
- [ ] **Focus**: Gain Reduction meter (showing GR needle/bar)
- [ ] **Capture**: Cmd+Shift+4 → Drag around GR meter
- [ ] **Save as**: `raw/SimpleComp-05-detail.png`

**✓ SimpleComp Complete (5 screenshots)**

---

## Plugin 4: CleanDelay (7 minutes)

**Test Audio**: Switch to `test-tone-440hz.wav` (clearer delay tails)  
**Time**: 17:46 - 17:53

### Screenshot 1: Default State
- [ ] **Import** `test-audio/test-tone-440hz.wav` to track
- [ ] **Load CleanDelay**
- [ ] **Reset to defaults**
- [ ] **Stop playback**
- [ ] **Parameters**: Delay 500ms, Feedback 20%, Mix 30%
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/CleanDelay-01-default.png`

### Screenshot 2: Active Processing
- [ ] **Start playback** (tone + delays audible)
- [ ] **Delay Time**: 250 ms (quarter note at 120 BPM)
- [ ] **Feedback**: 50%
- [ ] **Mix**: 40%
- [ ] **Filters**: HP at 200 Hz, LP at 8000 Hz
- [ ] **Verify**: Delay waveform visible on display
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/CleanDelay-02-active.png`

### Screenshot 3: Extreme Settings
- [ ] **Delay Time**: 2000 ms (long delay)
- [ ] **Feedback**: 90% (infinite repeats)
- [ ] **Mix**: 100% (fully wet)
- [ ] **Result**: Cascading delay buildup
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/CleanDelay-03-extreme.png`

### Screenshot 4: In-DAW Context
- [ ] **Zoom DAW out**
- [ ] **Capture**: Plugin + mixer context
- [ ] **Save as**: `raw/CleanDelay-04-daw.png`

### Screenshot 5: Detail Shot
- [ ] **Focus**: Delay time display or waveform visualization
- [ ] **Capture**: Cmd+Shift+4 → Drag around display
- [ ] **Save as**: `raw/CleanDelay-05-detail.png`

**✓ CleanDelay Complete (5 screenshots)**

---

## Plugin 5: Saturation (7 minutes)

**Test Audio**: `pink-noise-10s.wav` (shows harmonic distortion well)  
**Time**: 17:53 - 18:00

### Screenshot 1: Default State
- [ ] **Switch back** to `pink-noise-10s.wav`
- [ ] **Load Saturation**
- [ ] **Reset to defaults**
- [ ] **Stop playback**
- [ ] **Parameters**: Drive 0 dB, Mix 50%, Type: Soft Clip
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/Saturation-01-default.png`

### Screenshot 2: Active Processing
- [ ] **Start playback**
- [ ] **Drive**: 12 dB (moderate saturation)
- [ ] **Type**: Tape (warm analog)
- [ ] **Mix**: 60%
- [ ] **Tone**: Slight high-frequency rolloff
- [ ] **Verify**: Harmonic content visible on meters
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/Saturation-02-active.png`

### Screenshot 3: Extreme Settings
- [ ] **Drive**: 30 dB (maximum distortion)
- [ ] **Type**: Hard Clip (aggressive)
- [ ] **Mix**: 100%
- [ ] **Result**: Heavy distortion, visible clipping
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/Saturation-03-extreme.png`

### Screenshot 4: In-DAW Context
- [ ] **Zoom DAW out**
- [ ] **Capture**: Plugin + mixer context
- [ ] **Save as**: `raw/Saturation-04-daw.png`

### Screenshot 5: Detail Shot
- [ ] **Focus**: Saturation type selector or waveform display
- [ ] **Capture**: Cmd+Shift+4 → Drag around control
- [ ] **Save as**: `raw/Saturation-05-detail.png`

**✓ Saturation Complete (5 screenshots)**

---

## Plugin 6: Reverb (7 minutes)

**Test Audio**: `test-tone-440hz.wav` (clean reverb tails)  
**Time**: 18:00 - 18:07

### Screenshot 1: Default State
- [ ] **Switch to** `test-tone-440hz.wav`
- [ ] **Load Reverb**
- [ ] **Reset to defaults**
- [ ] **Stop playback**
- [ ] **Parameters**: Room Size 0.5, Decay 1.5s, Mix 30%
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/Reverb-01-default.png`

### Screenshot 2: Active Processing
- [ ] **Start playback**
- [ ] **Room Size**: 0.7 (Hall)
- [ ] **Decay Time**: 3.5s (long reverb)
- [ ] **Damping**: 0.6 (some high-frequency absorption)
- [ ] **Mix**: 50%
- [ ] **Width**: 100% (full stereo)
- [ ] **Verify**: Reverb tail audible
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/Reverb-02-active.png`

### Screenshot 3: Extreme Settings
- [ ] **Room Size**: 1.0 (Cathedral)
- [ ] **Decay Time**: 10.0s (infinite reverb)
- [ ] **Mix**: 100% (fully wet, no dry signal)
- [ ] **Result**: Massive ambient wash
- [ ] **Capture**: Cmd+Shift+4 → Space → Click plugin
- [ ] **Save as**: `raw/Reverb-03-extreme.png`

### Screenshot 4: In-DAW Context
- [ ] **Zoom DAW out**
- [ ] **Capture**: Plugin + mixer context
- [ ] **Save as**: `raw/Reverb-04-daw.png`

### Screenshot 5: Detail Shot
- [ ] **Focus**: Room size visualization or decay time display
- [ ] **Capture**: Cmd+Shift+4 → Drag around display
- [ ] **Save as**: `raw/Reverb-05-detail.png`

**✓ Reverb Complete (5 screenshots)**

---

## Session 1 Complete! 🎉

**Total Captured**: 30 screenshots  
**Time Elapsed**: ~2 hours  
**Next Step**: Optimize and deploy

### Post-Session: Optimization (5 minutes)

```bash
cd ~/Projects/Active/AudioForge/docs/screenshots

# Optimize all screenshots
./optimize-screenshots.sh

# Output:
# - web/ (720p, optimized for web)
# - optimized/ (2K, compressed)
# - gallery-index.md (auto-generated index)
```

### Verify Screenshot Quality

**Quick Visual Check**:
1. Open `raw/` folder in Finder
2. View as Icons (large size)
3. Verify for each plugin:
   - [ ] All 5 screenshots present
   - [ ] UI elements sharp and readable
   - [ ] Meters showing activity (screenshots 2-3)
   - [ ] No desktop clutter visible
   - [ ] Consistent plugin window size

**File Size Check**:
```bash
# Raw screenshots should be 2-5 MB each
ls -lh raw/*.png

# Total size should be ~100-150 MB for 30 screenshots
du -sh raw/
```

### Deploy to Website (10 minutes)

```bash
# Copy web-optimized versions to FluxStudio website
cp web/*.png ~/Projects/Active/FluxStudio/website/public/screenshots/audioforge/

# Update gallery page
# (Edit gallery component to reference new screenshots)
```

---

## Session 2 Preview

**Next**: Instruments (SimpleGain, WavetableSynth, FMSynth)  
**Duration**: 2 hours  
**Output**: 15 screenshots  
**Test Audio**: `midi-test-pattern.mid` (create in DAW first)

---

**Status**: ✅ READY TO CAPTURE  
**Start Time**: Now (17:25)  
**Estimated Completion**: 19:30 (2 hours)

**Let's capture professional marketing assets! 📸**
