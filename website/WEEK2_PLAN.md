# Week 2 - Visual Assets Plan

## Objective
Replace mockup graphics with real plugin screenshots and visual proof to build trust and drive conversions.

---

## 📸 Task 1: Plugin Screenshots (Priority: HIGH)

### Plugins to Screenshot (22 total)

#### **Group A: Fixed in v1.9.0 (PRIORITY 1)** ⭐
- [ ] SpectralFreeze - Show "Fixed in v1.9.0" badge
- [ ] Sampler - Show "Fixed in v1.9.0" badge
- [ ] FreezeFX - Show "Experimental" badge

#### **Group B: Professional Mixing Tools (PRIORITY 2)**
- [ ] Gate v1.7.0
- [ ] Limiter v1.6.0
- [ ] MultibandCompressor v1.9.0

#### **Group C: Core Utilities (PRIORITY 3)**
- [ ] SimpleGain
- [ ] PanUtil
- [ ] SimpleEQ
- [ ] SimpleComp

#### **Group D: Effects (PRIORITY 4)**
- [ ] CleanDelay
- [ ] Reverb
- [ ] Saturation
- [ ] WaveShaper
- [ ] ChorusFlanger

#### **Group E: Synthesizers (PRIORITY 5)**
- [ ] BasicSynth
- [ ] FMSynth
- [ ] WavetableSynth
- [ ] PadSynth
- [ ] DrumSynth
- [ ] OrganEmulator
- [ ] GranularEngine

---

## 📐 Screenshot Specifications

### Size & Format
- **Format**: PNG (better quality than SVG for UI screenshots)
- **Resolution**: 1200x800 pixels (2x for retina)
- **Background**: Transparent or dark (matching website theme)
- **File naming**: `PluginName-screenshot-main.png`

### What to Capture
1. **Full UI** - Complete plugin interface
2. **Key Features** - Highlight unique controls
3. **Metering** - Show live meters if available
4. **Professional Look** - Clean, well-lit, no distractions

### Where to Save
- Directory: `website/public/screenshots/plugins/`
- Subdirectory per plugin: `website/public/screenshots/plugins/SpectralFreeze/`

---

## 🎨 Task 2: Before/After Waveforms (Priority: MEDIUM)

### Top 5 Plugins for Waveform Examples
1. **SpectralFreeze** - Show frozen vs. unfrozen spectrum
2. **Limiter** - Show peak limiting in action
3. **SimpleComp** - Show compression curve
4. **Saturation** - Show harmonic enhancement
5. **CleanDelay** - Show delay feedback

### Specifications
- **Format**: PNG
- **Size**: 800x200 pixels
- **Show**: Before (top) vs After (bottom)
- **File naming**: `PluginName-before-after.png`

---

## 🎥 Task 3: Demo Videos (Priority: LOW)

### Top 5 Plugins for Video Demos
1. **SpectralFreeze** - 15s demo of freeze effect
2. **Limiter** - 15s demo of mastering
3. **GranularEngine** - 15s demo of granular synthesis
4. **FreezeFX** - 15s demo of spectral freeze
5. **Sampler** - 15s demo of sampling workflow

### Specifications
- **Format**: MP4 (H.264)
- **Length**: 10-15 seconds each
- **Resolution**: 1280x720 (720p)
- **File size**: <5MB per video
- **Audio**: Include audio output (no narration needed)

---

## 🏷️ Task 4: Status Badges (Priority: HIGH)

### Badge Types Needed
1. **"Fixed in v1.9.0"** - Green badge (SpectralFreeze, Sampler)
2. **"Experimental"** - Orange/yellow badge (FreezeFX)
3. **"New"** - Blue badge (Gate, Limiter, MultibandCompressor)
4. **"Validated"** - Green checkmark (21 plugins)

### Implementation
- Create badge components in `website/src/components/Badge.tsx`
- Add to plugin cards in `PluginCard.tsx`
- Style with Tailwind CSS to match existing design

---

## 📊 Progress Tracking

### Estimated Time Breakdown
- [ ] Group A screenshots (3 plugins): 30 min
- [ ] Group B screenshots (3 plugins): 30 min
- [ ] Group C screenshots (4 plugins): 40 min
- [ ] Group D screenshots (5 plugins): 50 min
- [ ] Group E screenshots (7 plugins): 70 min
- [ ] Before/after waveforms (5): 60 min
- [ ] Status badges: 30 min
- [ ] Website integration: 60 min
- [ ] Deploy & test: 20 min

**Total**: ~6 hours

---

## 🎯 Quick Win Strategy

**Option 1: MVP Approach** (2 hours)
1. Screenshot Group A (3 fixed plugins) ✅
2. Create status badges ✅
3. Update website with Group A screenshots ✅
4. Deploy immediately for quick impact ✅

**Option 2: Complete Approach** (6 hours)
1. Screenshot all 22 plugins
2. Create 5 before/after waveforms
3. Add all status badges
4. Full website integration
5. Deploy complete update

---

## 🚀 Execution Order (Recommended: MVP First)

### Phase 1: Quick Win (2 hours)
1. Open Ableton Live 12
2. Screenshot SpectralFreeze, Sampler, FreezeFX
3. Create Badge component
4. Update PluginCard.tsx with badges
5. Add Group A screenshots to website
6. Deploy

### Phase 2: Complete (4 more hours)
7. Screenshot remaining 19 plugins
8. Create 5 before/after waveforms
9. Full gallery integration
10. Deploy final update

---

## ✅ Success Criteria

- [ ] All 22 plugins have real UI screenshots
- [ ] Top 5 plugins have before/after waveforms
- [ ] All plugins show correct status badges
- [ ] Screenshots display in plugin cards on website
- [ ] Mobile responsive screenshot display
- [ ] Deploy and verify in production

---

**Ready to start with Phase 1 (Quick Win)?**
