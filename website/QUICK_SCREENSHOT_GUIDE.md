# Quick Screenshot Guide - Badge Plugins

**Goal**: Capture real screenshots for the 3 remaining badge plugins  
**Time**: 30 minutes (10 min per plugin)  
**Tool**: macOS Screenshot (Cmd+Shift+4)

---

## Setup (5 minutes)

1. **Open Ableton Live 12 Intro**
2. **Create new project** with 2 tracks:
   - Audio track (for SpectralFreeze, FreezeFX)
   - MIDI track (for Sampler)
3. **Load test audio**: Any audio file with content
4. **Set buffer**: 512 samples
5. **Window arrangement**: Plugin centered, clean background

---

## Screenshot Specs

- **Size**: Native resolution (will auto-optimize later)
- **Format**: PNG
- **Naming**: `{PluginName}-main.png`
- **Location**: Save to Desktop (we'll move them)
- **Capture method**: Cmd+Shift+4 → SPACE → Click plugin window

---

## Plugin 1: SpectralFreeze (10 minutes)

### Steps:
1. **Load** SpectralFreeze on audio track
2. **Configure**:
   - Leave at default settings initially
   - Start playback to see spectrum visualization
   - Adjust to show active state
3. **Screenshot**:
   - Press `Cmd+Shift+4`
   - Press `SPACE` (cursor becomes camera)
   - Click the SpectralFreeze window
   - File saves to Desktop as "Screenshot YYYY-MM-DD at HH.MM.SS.png"
4. **Rename** on Desktop to: `SpectralFreeze-main.png`

### What to Show:
- ✅ Full plugin UI visible
- ✅ Parameters at reasonable values (not all zero)
- ✅ Spectrum display active (if visible)
- ✅ Clean, professional look

---

## Plugin 2: Sampler (10 minutes)

### Steps:
1. **Load** Sampler on MIDI track
2. **Configure**:
   - Load a sample (drag any WAV file)
   - Show ADSR envelope active
   - Display waveform
3. **Screenshot**:
   - `Cmd+Shift+4` → `SPACE` → Click Sampler window
4. **Rename** to: `Sampler-main.png`

### What to Show:
- ✅ Full plugin UI with sample loaded
- ✅ Waveform visible
- ✅ ADSR envelope displayed
- ✅ Professional appearance

---

## Plugin 3: FreezeFX (10 minutes)

### Steps:
1. **Load** FreezeFX on audio track
2. **Configure**:
   - Default settings OK
   - Start playback
   - Show freeze effect active (if UI indicates state)
3. **Screenshot**:
   - `Cmd+Shift+4` → `SPACE` → Click FreezeFX window
4. **Rename** to: `FreezeFX-main.png`

### What to Show:
- ✅ Full plugin UI
- ✅ Controls visible and clear
- ✅ Any visualization active
- ✅ Professional look

---

## After Capture (5 minutes)

Once you have all 3 screenshots on Desktop:

```bash
# Run this command to move them to the right location:
cd ~/Desktop
mkdir -p ~/Projects/Active/AudioForge/website/temp-screenshots
mv SpectralFreeze-main.png ~/Projects/Active/AudioForge/website/temp-screenshots/
mv Sampler-main.png ~/Projects/Active/AudioForge/website/temp-screenshots/
mv FreezeFX-main.png ~/Projects/Active/AudioForge/website/temp-screenshots/

echo "✅ Screenshots ready for processing!"
```

Then tell me when ready, and I'll:
1. Optimize the screenshots
2. Move them to the website
3. Update the plugin data
4. Deploy!

---

## Quick Tips

### Screenshot Quality
- ✅ **Clean background** - No cluttered desktop visible
- ✅ **Good lighting** - Plugin UI clearly visible
- ✅ **Active state** - Show the plugin doing something, not idle
- ✅ **No overlays** - No macOS cursor, notifications, or tooltips

### Common Issues
- ❌ **Screenshot too small**: Make sure plugin window is large enough
- ❌ **Dark/unclear**: Increase screen brightness
- ❌ **Wrong window**: SPACE key must be pressed for window capture
- ❌ **Includes background**: Use SPACE for clean window capture (not drag selection)

### Speed Tips
1. **Load all 3 plugins first** in separate tracks
2. **Capture all 3 in sequence** without closing DAW
3. **Rename immediately** after each capture
4. **Total time**: 25-30 minutes for all 3

---

## Alternative: Use Existing Screenshots

If any of these plugins have screenshots elsewhere, we can use those instead. Check:

```bash
# Search for existing screenshots
find ~/Projects/Active/AudioForge -name "*SpectralFreeze*.png" -o -name "*Sampler*.png" -o -name "*FreezeFX*.png"
```

---

## Ready When You Are!

**Option A**: Capture the screenshots now (30 min)  
**Option B**: Skip screenshots, deploy with current state (badges + 4 real screenshots)  
**Option C**: Take a break and capture later

Let me know when you're ready or if you'd like me to check for existing screenshots first!
