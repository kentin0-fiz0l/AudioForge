# Screenshot Capture Checklist

Quick reference for capturing all 50 plugin screenshots.

**Goal**: 5 screenshots per plugin × 10 plugins = 50 total

---

## Setup

- [ ] DAW prepared (clean UI, tooltips off)
- [ ] All 10 plugins installed
- [ ] Test audio files ready
- [ ] Screenshot directories created

**Save to**: `website/public/screenshots/<Plugin>/<Plugin>-<#>-<description>.png`

---

## 1. SimpleGain ✅

- [ ] `SimpleGain-1-default.png` - Gain 0.0 dB, no signal
- [ ] `SimpleGain-2-reducing.png` - Gain -6.0 dB, active meters
- [ ] `SimpleGain-3-boosting.png` - Gain +12.0 dB, active meters
- [ ] `SimpleGain-4-fine-control.png` - Gain -3.2 dB, precise value
- [ ] `SimpleGain-5-bypass-compare.png` - Gain +6.0 dB, bypass visible

---

## 2. PanUtil ✅

- [ ] `PanUtil-1-default.png` - Center, 100% width
- [ ] `PanUtil-2-panned-left.png` - Pan -50%, 100% width
- [ ] `PanUtil-3-wide-stereo.png` - Center, 150% width
- [ ] `PanUtil-4-narrow-mono.png` - Center, 0% width (mono)
- [ ] `PanUtil-5-creative-panning.png` - Pan +30%, 120% width

---

## 3. BasicSynth ✅

- [ ] `BasicSynth-1-default.png` - All oscillators off, defaults
- [ ] `BasicSynth-2-bass-sound.png` - Saw+Sine, filter 500Hz, low note
- [ ] `BasicSynth-3-lead-sound.png` - Saw+Square, filter 2kHz, mid note
- [ ] `BasicSynth-4-pad-sound.png` - Sine, filter 800Hz, slow envelope, chord
- [ ] `BasicSynth-5-filter-sweep.png` - All osc on, filter 1.2kHz, high res

---

## 4. CleanDelay ✅

- [ ] `CleanDelay-1-default.png` - 1/4 note, 30% feedback, 25% mix
- [ ] `CleanDelay-2-vocal-delay.png` - 1/4 note, 40% FB, HPF 200Hz
- [ ] `CleanDelay-3-ping-pong.png` - 1/8 note, 50% FB, ping-pong ON
- [ ] `CleanDelay-4-rhythmic.png` - 1/16 dotted, 60% FB, 40% mix
- [ ] `CleanDelay-5-dub-delay.png` - 1/2 note, 70% FB, HPF 150Hz

---

## 5. SimpleEQ ✅

- [ ] `SimpleEQ-1-default.png` - All flat, filters off
- [ ] `SimpleEQ-2-vocal-eq.png` - HPF 100Hz, cut 300Hz, boost 3kHz & 10kHz
- [ ] `SimpleEQ-3-bass-eq.png` - Boost 60Hz, cut 250Hz, LPF 8kHz
- [ ] `SimpleEQ-4-surgical-cut.png` - -8dB @ 1.2kHz (Q=5.0)
- [ ] `SimpleEQ-5-presence-boost.png` - +5dB @ 4kHz, +3dB @ 12kHz

---

## 6. SimpleComp ✅

- [ ] `SimpleComp-1-default.png` - -20dB, 2:1, no GR
- [ ] `SimpleComp-2-vocal-compression.png` - -18dB, 3:1, 3-5dB GR
- [ ] `SimpleComp-3-aggressive.png` - -12dB, 8:1, 8-10dB GR
- [ ] `SimpleComp-4-subtle-glue.png` - -24dB, 1.5:1, 2-3dB GR
- [ ] `SimpleComp-5-limiting.png` - -6dB, 20:1, 6-8dB GR

---

## 7. WaveShaper ✅

- [ ] `WaveShaper-1-default.png` - Soft curve, 0% drive
- [ ] `WaveShaper-2-gentle-warmth.png` - Soft, 30% drive
- [ ] `WaveShaper-3-hard-clipping.png` - Hard, 80% drive
- [ ] `WaveShaper-4-fold-distortion.png` - Fold, 90% drive
- [ ] `WaveShaper-5-parallel.png` - Hard, 70% drive, 50% mix

---

## 8. Saturation ✅

- [ ] `Saturation-1-default.png` - Tape, 0% drive, 0% tone
- [ ] `Saturation-2-tape-warmth.png` - Tape, 25% drive, -5% tone
- [ ] `Saturation-3-tube-overdrive.png` - Tube, 60% drive, 0% tone
- [ ] `Saturation-4-transistor-punch.png` - Transistor, 40% drive, +10% tone
- [ ] `Saturation-5-master-bus.png` - Tape, 15% drive, -3% tone

---

## 9. ChorusFlanger ✅

- [ ] `ChorusFlanger-1-default.png` - Chorus, 0.5Hz, 30% depth
- [ ] `ChorusFlanger-2-classic-chorus.png` - Chorus, 0.8Hz, 40% depth
- [ ] `ChorusFlanger-3-fast-chorus.png` - Chorus, 2.0Hz, 50% depth
- [ ] `ChorusFlanger-4-jet-flanger.png` - Flanger, 0.3Hz, 80% depth, 70% FB
- [ ] `ChorusFlanger-5-subtle-flange.png` - Flanger, 0.5Hz, 40% depth, 30% FB

---

## 10. Reverb ✅

- [ ] `Reverb-1-default.png` - 50% size, 50% damp, 20% mix
- [ ] `Reverb-2-small-room.png` - 25% size, 40% damp, 15% mix
- [ ] `Reverb-3-large-hall.png` - 90% size, 60% damp, 35% mix, 40ms pre
- [ ] `Reverb-4-bright-plate.png` - 60% size, 20% damp, 25% mix, 30ms pre
- [ ] `Reverb-5-dark-chamber.png` - 70% size, 80% damp, 30% mix, 20ms pre

---

## Progress Tracker

**Completed**: _____ / 50 screenshots

**Time Estimate**: 2.5-3 hours total

---

## Quick Capture Steps

1. Load plugin in DAW
2. Set parameters from checklist
3. Start audio playback (if needed)
4. Wait for meters to stabilize
5. Capture: `Cmd+Shift+4` → `Spacebar` → Click window (macOS)
6. Save to `website/public/screenshots/<Plugin>/`
7. Check off in list above

---

## After Completion

- [ ] All 50 screenshots captured
- [ ] Filenames correct
- [ ] Optimize images (optional): `pngquant --quality=80-100 *.png`
- [ ] Update website gallery
- [ ] Commit to repository

---

**Reference**: See `docs/SCREENSHOT-GUIDE.md` for detailed instructions.

**Last Updated**: 2026-08-25
