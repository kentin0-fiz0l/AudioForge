# Screenshot Capture - Quick Reference Card

**Keep this open during Session 1** 📋

---

## Screenshot Shortcuts

| Action | Shortcut |
|--------|----------|
| **Capture window** | `Cmd+Shift+4` → `Space` → Click |
| **Capture selection** | `Cmd+Shift+4` → Drag area |
| **Cancel capture** | `Esc` |

---

## File Naming Convention

**Format**: `[PluginName]-[Number]-[Type].png`

| Number | Type | Description |
|--------|------|-------------|
| `01` | `default` | Reset plugin, stop playback |
| `02` | `active` | Playing audio, typical settings |
| `03` | `extreme` | Creative/aggressive settings |
| `04` | `daw` | In-context (DAW + plugin) |
| `05` | `detail` | Close-up of unique feature |

**Examples**:
- `SimpleGain-01-default.png`
- `SimpleEQ-02-active.png`
- `Reverb-03-extreme.png`

---

## 5-Step Capture Pattern (Every Plugin)

### 1️⃣ Default State
- ✅ Reset to defaults (`Initialize Preset`)
- ✅ **STOP playback** (silent meters)
- ✅ Capture window
- ✅ Save: `[Plugin]-01-default.png`

### 2️⃣ Active Processing
- ✅ **START playback** (loop enabled)
- ✅ Adjust to musical settings
- ✅ Wait 3 seconds (meters stabilize)
- ✅ Capture window
- ✅ Save: `[Plugin]-02-active.png`

### 3️⃣ Extreme Settings
- ✅ Set creative/aggressive parameters
- ✅ Show plugin's full range
- ✅ Capture window
- ✅ Save: `[Plugin]-03-extreme.png`

### 4️⃣ In-DAW Context
- ✅ Zoom DAW out (show mixer)
- ✅ Frame: plugin + 2-3 tracks
- ✅ **Drag selection** capture
- ✅ Save: `[Plugin]-04-daw.png`

### 5️⃣ Detail Shot
- ✅ Focus on unique visual element
- ✅ **Drag selection** around element
- ✅ Capture close-up
- ✅ Save: `[Plugin]-05-detail.png`

---

## Parameter Quick Settings

### SimpleGain
- **Active**: -6 dB (meters at -18 dBFS)
- **Extreme**: +12 dB (clipping)

### SimpleEQ
- **Active**: +3 dB @ 100 Hz, +2 dB @ 1 kHz, -2 dB shelf @ 8 kHz
- **Extreme**: +10 dB on all bands (smiley curve)

### SimpleComp
- **Active**: -10 dB threshold, 4:1 ratio, 5 dB GR
- **Extreme**: -20 dB threshold, 20:1 ratio, 10+ dB GR

### CleanDelay
- **Active**: 250 ms, 50% feedback, 40% mix
- **Extreme**: 2000 ms, 90% feedback, 100% mix

### Saturation
- **Active**: 12 dB drive, Tape mode, 60% mix
- **Extreme**: 30 dB drive, Hard Clip, 100% mix

### Reverb
- **Active**: 0.7 size (Hall), 3.5s decay, 50% mix
- **Extreme**: 1.0 size (Cathedral), 10s decay, 100% mix

---

## Test Audio Guide

| Plugin | Audio File | Why |
|--------|------------|-----|
| SimpleGain | `pink-noise-10s.wav` | Even spectrum, stable meters |
| SimpleEQ | `pink-noise-10s.wav` | Shows EQ curve clearly |
| SimpleComp | `pink-noise-10s.wav` | Consistent dynamics for GR |
| CleanDelay | `test-tone-440hz.wav` | Clear delay tails |
| Saturation | `pink-noise-10s.wav` | Shows harmonic distortion |
| Reverb | `test-tone-440hz.wav` | Clean reverb tails |

---

## Quality Checklist (Before Moving to Next Plugin)

- [ ] **5 files present** (01-05)
- [ ] **UI sharp** (no blur, readable text)
- [ ] **Meters active** (screenshots 2-3)
- [ ] **Clean frame** (no desktop clutter)
- [ ] **Correct naming** (`[Plugin]-[Num]-[Type].png`)

---

## Progress Tracker

**Session 1**: Core Processing Suite (2 hours)

- [ ] **SimpleGain** (7 min) - 5 screenshots
- [ ] **SimpleEQ** (7 min) - 5 screenshots
- [ ] **SimpleComp** (7 min) - 5 screenshots
- [ ] **CleanDelay** (7 min) - 5 screenshots
- [ ] **Saturation** (7 min) - 5 screenshots
- [ ] **Reverb** (7 min) - 5 screenshots

**Total**: 30 screenshots

---

## After Capture

```bash
# Optimize all screenshots
cd ~/Projects/Active/AudioForge/docs/screenshots
./optimize-screenshots.sh

# Check output
ls -lh web/  # Should show 30 files, ~200-400 KB each
```

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| **Screenshot has shadow** | Run: `defaults write com.apple.screencapture disable-shadow -bool true && killall SystemUIServer` |
| **Meters not showing** | Wait 3+ seconds after starting playback |
| **Plugin window too small** | Resize plugin before capture (if resizable) |
| **DAW UI cluttered** | Hide dock: `Cmd+Opt+D`, fullscreen DAW: `Ctrl+Cmd+F` |
| **File size too large** | Don't worry, optimization script will compress |

---

## Time Tracking

| Plugin | Start | End | Duration | ✓ |
|--------|-------|-----|----------|---|
| SimpleGain | 17:25 | 17:32 | 7 min | |
| SimpleEQ | 17:32 | 17:39 | 7 min | |
| SimpleComp | 17:39 | 17:46 | 7 min | |
| CleanDelay | 17:46 | 17:53 | 7 min | |
| Saturation | 17:53 | 18:00 | 7 min | |
| Reverb | 18:00 | 18:07 | 7 min | |
| **Optimization** | 18:07 | 18:12 | 5 min | |

**Total**: ~2 hours

---

**YOU'VE GOT THIS! 📸**

Capture → Optimize → Deploy → Grow
