# AudioForge Testing Quick Start Guide

## 🚀 Getting Started with Testing

### Prerequisites
✅ **37 AudioForge plugins installed** to `~/Library/Audio/Plug-Ins/VST3/`  
✅ **DAW ready** (Logic Pro, Ableton, Reaper, etc.)  
✅ **MIDI controller** (optional but recommended for MIDI Learn testing)

---

## Testing Workflow

### 1️⃣ **Open Your DAW**
Choose your preferred DAW and create a new project:
- **Logic Pro:** File → New Project → Empty Project
- **Ableton Live:** File → New Live Set
- **Reaper:** File → New Project

### 2️⃣ **Load AudioForge Plugins**

**For Synth Plugins (MIDI Generators):**
1. Create a new Software Instrument track
2. Load plugin: `AudioForge → [Plugin Name]`
3. Create a MIDI region
4. Play notes from keyboard or draw MIDI

**For Effect Plugins:**
1. Create an Audio track
2. Add plugin to insert slot
3. Play audio through the effect
4. Adjust parameters

**For MIDI Effect Plugins:**
1. Create a Software Instrument track
2. Insert MIDI Effect BEFORE the synth
3. Play MIDI notes
4. Observe MIDI transformation

---

## Recommended Testing Order

### Phase 1: Quick Smoke Test (30 minutes)
Test that all 37 plugins load without crashing:

```bash
# Run through each category quickly:
1. Load 5-10 plugins randomly
2. Verify each opens without error
3. Play a note or audio through each
4. Close and move to next plugin
```

**Minimum Test List:**
- BasicSynth, DrumSynth, ElectricPiano (synths)
- SimpleGain, SimpleEQ, Reverb (effects)
- MIDI Arpeggiator, XY Pad Controller (MIDI)

### Phase 2: Functional Testing (2-3 hours)
Test core features for each plugin type:

**Synths:** Play notes, adjust filters, test polyphony  
**Effects:** Process audio, adjust wet/dry, test bypass  
**MIDI Effects:** Transform MIDI, verify output correctness  

### Phase 3: MIDI Learn Testing (1-2 hours)
Test MIDI CC learning on 10+ plugins:

1. Load plugin with parameters
2. Click "Learn" button (if visible)
3. Send MIDI CC from controller
4. Verify parameter responds
5. Save and reload project
6. Verify bindings persist

### Phase 4: Edge Case Testing (1-2 hours)
Push plugins to extremes:

- Test minimum/maximum parameter values
- Rapid parameter changes
- High polyphony (20+ notes)
- Extreme processing (100% wet, max feedback)
- Save/load with extreme settings

---

## DAW-Specific Instructions

### Logic Pro X
```
1. Open Logic Pro
2. Create Software Instrument track
3. Click empty instrument slot
4. Navigate: Audio Units → AudioForge → [Plugin]
5. MIDI routing: Track → MIDI Effect → Synth
```

### Ableton Live
```
1. Open Ableton Live
2. Create MIDI track
3. Drag plugin from Browser:
   Plug-ins → VST3 → AudioForge → [Plugin]
4. MIDI routing: MIDI track → MIDI Effect → Instrument
```

### Reaper
```
1. Open Reaper
2. Create new track
3. Click FX button
4. Type "AudioForge" in search
5. Double-click plugin to load
```

---

## MIDI Controller Setup

### Generic MIDI Controller
```
1. Connect MIDI controller via USB
2. Open DAW preferences → MIDI
3. Enable controller in MIDI inputs
4. Set control mode to "Control"
5. Test: Move knob/fader, see CC messages
```

### Testing MIDI Learn Without Controller
If you don't have a physical MIDI controller, you can use:

**Option 1: Virtual MIDI (macOS)**
```bash
# Open Audio MIDI Setup
# Window → Show MIDI Studio
# Double-click "IAC Driver"
# Enable "Device is online"
# Create virtual MIDI bus
```

**Option 2: DAW's MIDI Monitor**
Most DAWs can generate MIDI CC for testing:
- Logic: Inspector → MIDI Effect → Modulator
- Ableton: MIDI Effects → MIDI CC Generator
- Reaper: Virtual MIDI Keyboard

---

## Common Issues & Solutions

### ❌ Plugin Doesn't Appear in DAW
**Solution:**
```bash
# Verify plugin is installed
ls ~/Library/Audio/Plug-Ins/VST3/*.vst3 | grep AudioForge

# Rescan plugins in DAW:
# Logic: Preferences → Plug-in Manager → Reset & Rescan
# Ableton: Preferences → Plug-ins → Rescan
# Reaper: Preferences → VST → Re-scan
```

### ❌ Plugin Crashes on Load
**Solution:**
1. Check Console.app for crash logs
2. Note the plugin name
3. Document in QA_TESTING_PLAN.md → Bug Tracker
4. Try rebuilding that specific plugin

### ❌ No Sound from Synth
**Solution:**
- Verify MIDI routing: MIDI track → Synth track
- Check if notes are being received (MIDI indicator)
- Verify synth parameters (filter not closed, volume up)
- Test with different octaves (C3, C4, C5)

### ❌ MIDI Learn Not Working
**Solution:**
- Verify MIDI input is enabled in DAW
- Check if controller sends CC (not notes)
- Try different CC numbers (CC1, CC7, CC74)
- Look for visual "Learn" mode indicator

---

## Recording Test Results

### Quick Method (Markdown Checklist)
Open `QA_TESTING_PLAN.md` and check off items:
```markdown
#### ✓ BasicSynth
- [x] Smoke: Loads in DAW ← Mark with 'x'
- [x] Functional: Plays notes, oscillator types work
- [ ] MIDI Learn: Can learn MIDI CC for filter cutoff
```

### Detailed Method (Bug Tracker)
For issues found, add to Bug Tracker section:
```markdown
### High Priority
- **BasicSynth**: Crashes when loading with >16 voices
  - Steps to reproduce: Load plugin, set voices to 32, play chord
  - Expected: Should handle 32 voices
  - Actual: Crashes DAW
```

---

## Example Testing Session (30 min)

**Goal:** Test 5 plugins across different categories

**Minute 0-5: BasicSynth**
1. Load in new MIDI track
2. Play C-major scale → ✓ Works
3. Test oscillator types → ✓ All 3 work
4. Learn MIDI CC to filter → ✓ CC1 controls cutoff
5. Save project → ✓ Bindings persist

**Minute 5-10: SimpleEQ**
1. Load on audio track
2. Play audio through → ✓ Passes signal
3. Boost high freq +6dB → ✓ Sounds brighter
4. Test bypass → ✓ Clean bypass
5. Learn CC to frequency → ✓ CC74 works

**Minute 10-15: MIDI Arpeggiator**
1. Load before synth in MIDI chain
2. Hold C-major chord → ✓ Arpeggios notes
3. Change pattern to Down → ✓ Pattern changes
4. Test triplet rates → ✓ Sync works
5. Learn CC to octave range → ✓ CC20 works

**Minute 15-20: Reverb**
1. Load on audio track
2. Adjust room size → ✓ Size changes
3. Test 100% wet → ✓ Full reverb
4. Test different algorithms → ✓ All 3 sound different
5. CPU usage check → ✓ Acceptable

**Minute 20-25: XY Pad Controller**
1. Load before synth
2. Drag finger on pad → ✓ Generates CC
3. Route to synth filter → ✓ Controls sound
4. Test 4-corner positions → ✓ Full range
5. Learn pad to external CC → ✓ Works

**Minute 25-30: Document Results**
- Update QA_TESTING_PLAN.md
- Note: 5/37 plugins tested, all passing
- No bugs found in this session
- Save and commit progress

---

## Next Steps After Testing

✅ **All Pass:** Move to documentation and release prep  
⚠️ **Minor Issues:** Document known issues, continue testing  
❌ **Critical Bugs:** Stop testing, fix issues, rebuild, retest  

---

## Testing Tips

💡 **Start Simple:** Test basic plugins first (SimpleGain, SimpleEQ)  
💡 **Use Templates:** Create DAW template with tracks pre-loaded  
💡 **Batch Test:** Load 5-10 plugins in one project for quick smoke tests  
💡 **Save Often:** DAW crash = lost test progress  
💡 **Take Notes:** Document weird behavior even if not a bug  
💡 **Compare:** A/B test with commercial plugins for quality check  

---

**Ready to start testing?** Open your DAW and begin with Phase 1: Quick Smoke Test!
