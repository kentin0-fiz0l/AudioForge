# AudioForge Screenshot Capture Checklist

**Date Started:** September 7, 2026  
**Status:** 4/22 Complete (18 remaining)

---

## ✅ Completed (4)

- [x] Gate
- [x] Limiter
- [x] MultibandCompressor
- [x] SimpleGain

---

## 📸 Mixing & Mastering Suite (5 remaining)

- [ ] **PanUtil** - Stereo panning utility with balance and width controls
- [ ] **SimpleEQ** - 3-band parametric EQ with high/low-pass filters
- [ ] **SimpleComp** - Compressor with auto makeup gain
- [ ] **Reverb** - Algorithmic reverb with freeze mode
- [ ] **CleanDelay** - Stereo delay with ping-pong mode

---

## 🎨 Creative Effects (3 remaining)

- [ ] **Saturation** - Multi-mode analog saturation
- [ ] **WaveShaper** - Waveshaping distortion with 5 curve modes
- [ ] **ChorusFlanger** - LFO-based modulation effects

---

## 🌈 Spectral Effects (2 remaining)

- [ ] **SpectralFreeze** - FFT-based spectral freezing (⚠️ Fixed in v1.9.0)
- [ ] **FreezeFX** - Advanced spectral freeze with blur (⚠️ Experimental)

---

## 🎹 Synthesizers (8 remaining)

- [ ] **BasicSynth** - Subtractive synthesizer with ADSR envelope
- [ ] **FMSynth** - 4-operator FM synthesis (20 presets)
- [ ] **WavetableSynth** - Wavetable synthesis (26 presets)
- [ ] **PadSynth** - Lush pad synthesis (20 presets)
- [ ] **DrumSynth** - Drum synthesis engine (30 presets)
- [ ] **OrganEmulator** - Hammond-style organ with drawbars (25 presets)
- [ ] **GranularEngine** - Real-time granular synthesis with MIDI
- [ ] **SamplerPlugin** - Multi-zone sampler (⚠️ Fixed in v1.9.0)

---

## 📝 Capture Tips

### Screenshot Settings
- **Method:** Cmd+Shift+4 → Space → Click window
- **Format:** PNG (default)
- **Target Width:** 1200px (optimized automatically)
- **Mode:** Light or Dark (be consistent!)

### Best Practices
1. **Show the plugin doing something interesting:**
   - EQ: Show frequency curve with a cut/boost
   - Compressor: Show gain reduction happening
   - Synth: Show interesting preset name
   - Effects: Show wet/dry mix engaged

2. **Window Positioning:**
   - Center the plugin window on screen
   - Make sure all controls are visible
   - No overlapping windows behind it

3. **Quality Check:**
   - Clear, readable text
   - No pixelation or blur
   - Consistent appearance with existing 4 screenshots

---

## 🚀 Quick Start

Run the automated capture script:

```bash
cd ~/Projects/Active/AudioForge
./capture-screenshots.sh
```

Or capture manually:
1. Open DAW (Logic Pro, Ableton, etc.)
2. Load plugin
3. Take screenshot (Cmd+Shift+4 + Space + Click)
4. Move to: `website/public/screenshots/plugins/[PluginName]/[pluginname]-main.png`
5. Optimize: `sips -Z 1200 [file].png`

---

## ⏱️ Time Estimates

- **Per Plugin:** ~12-15 minutes
  - Build/load: 2 min
  - Capture: 5 min
  - Optimize: 2 min
  - Update website data: 3 min

- **Batch of 5 plugins:** ~1 hour
- **All 18 plugins:** ~4-5 hours

---

## 📊 Progress Tracking

### Session 1: Mixing & Mastering (5 plugins, ~1 hour)
- [ ] PanUtil
- [ ] SimpleEQ
- [ ] SimpleComp
- [ ] Reverb
- [ ] CleanDelay

### Session 2: Effects (3 plugins, ~45 min)
- [ ] Saturation
- [ ] WaveShaper
- [ ] ChorusFlanger

### Session 3: Spectral (2 plugins, ~30 min)
- [ ] SpectralFreeze
- [ ] FreezeFX

### Session 4: Synthesizers Part 1 (4 plugins, ~1 hour)
- [ ] BasicSynth
- [ ] FMSynth
- [ ] WavetableSynth
- [ ] PadSynth

### Session 5: Synthesizers Part 2 (4 plugins, ~1 hour)
- [ ] DrumSynth
- [ ] OrganEmulator
- [ ] GranularEngine
- [ ] SamplerPlugin

---

## 🎯 After Capturing Screenshots

1. **Verify Quality:**
   ```bash
   ls -lh website/public/screenshots/plugins/*/
   # Each screenshot should be ~1-2 MB
   ```

2. **Update Website Data:**
   Edit `website/src/lib/plugins.ts` to add screenshot paths

3. **Test Website Locally:**
   ```bash
   cd website
   npm run dev
   # Visit http://localhost:3000 to verify
   ```

4. **Deploy:**
   ```bash
   cd website
   npm run build
   ./deploy.sh
   ```

---

**Last Updated:** September 7, 2026
