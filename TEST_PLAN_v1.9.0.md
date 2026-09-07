# AudioForge v1.9.0 - DAW Test Plan

**Test Date**: September 3, 2026  
**Tester**: User  
**Version**: v1.9.0  
**Focus**: Verify fixes for SpectralFreeze and Sampler

---

## 🎯 Test Objectives

### Priority 1: Critical Fixes (MUST PASS)
- ✅ **SpectralFreeze** - Must load without crash (was crashing in v1.8.1)
- ✅ **Sampler** - Must load without crash (was crashing in v1.8.1)

### Priority 2: Experimental Status (EXPECTED BEHAVIOR)
- ⚠️ **FreezeFX** - Should work normally, may have issues with rapid UI toggling

### Priority 3: Regression Testing (SHOULD PASS)
- ✅ Verify other plugins still work (no regressions introduced)

---

## 🧪 Test Suite

### Test 1: SpectralFreeze - Load Test (CRITICAL)
**DAW**: Ableton Live 12 Intro  
**Expected**: Plugin loads without crash  
**Was Broken**: Yes (segfault in v1.8.1)

**Steps**:
1. Open Ableton Live 12
2. Create new audio track
3. Add AudioForge SpectralFreeze to track
4. **PASS if**: Plugin editor opens without crash
5. **FAIL if**: Crash, hang, or error message

**Test Cases**:
- [ ] Initial load (cold start)
- [ ] Close and reopen editor 3 times
- [ ] Load in multiple tracks simultaneously
- [ ] Play audio through plugin
- [ ] Adjust parameters while audio playing
- [ ] Save and reload project

---

### Test 2: Sampler - Load Test (CRITICAL)
**DAW**: Ableton Live 12 Intro  
**Expected**: Plugin loads without crash  
**Was Broken**: Yes (segfault in v1.8.1)

**Steps**:
1. Create new MIDI track in Ableton
2. Add AudioForge Sampler to track
3. **PASS if**: Plugin editor opens without crash
4. **FAIL if**: Crash, hang, or error message

**Test Cases**:
- [ ] Initial load (cold start)
- [ ] Load a sample (drag & drop audio file)
- [ ] Play MIDI notes (verify audio output)
- [ ] Close and reopen editor 5 times rapidly
- [ ] Adjust ADSR envelope while playing
- [ ] Create multiple zones
- [ ] Save and reload project

---

### Test 3: FreezeFX - Normal Usage (EXPERIMENTAL)
**DAW**: Ableton Live 12 Intro  
**Expected**: Works normally, may fail stress tests  
**Status**: Experimental (known UI stress test limitation)

**Steps**:
1. Create audio track
2. Add AudioForge FreezeFX
3. **PASS if**: Normal usage works fine
4. **EXPECTED LIMITATION**: May crash if GUI toggled rapidly during playback

**Test Cases**:
- [ ] Load plugin (should work)
- [ ] Process audio with freeze effect (should work)
- [ ] Adjust parameters (should work)
- [ ] Leave editor open during playback (should work)
- [ ] Close editor normally (should work)
- ⚠️ Rapid open/close 10 times while playing (MAY crash - this is OK)

**Result Interpretation**:
- If normal usage works → FreezeFX is production-ready for typical use
- If rapid toggling crashes → Expected, documented limitation
- If normal usage crashes → Need to investigate further

---

### Test 4: Professional Mixing Tools (REGRESSION)
**DAW**: Ableton Live 12 Intro  
**Expected**: All three should work (were passing in v1.8.1)

**Quick Test**:
- [ ] **Gate** - Loads and processes audio
- [ ] **Limiter** - Loads and processes audio  
- [ ] **Multiband Compressor** - Loads and processes audio

---

### Test 5: Cross-DAW Compatibility
**DAWs**: GarageBand, Ableton Live 11  
**Expected**: Plugins work across different hosts

**GarageBand Test**:
- [ ] Load SpectralFreeze in GarageBand
- [ ] Load Sampler in GarageBand
- [ ] Verify both work normally

**Ableton Live 11 Test**:
- [ ] Load SpectralFreeze in Live 11
- [ ] Load Sampler in Live 11
- [ ] Verify both work normally

---

## 📊 Test Results Template

### SpectralFreeze Results
```
DAW: Ableton Live 12 Intro
Status: [ ] PASS  [ ] FAIL
Notes: 


Issues Found:
- 

Verified Features:
- 
```

### Sampler Results
```
DAW: Ableton Live 12 Intro
Status: [ ] PASS  [ ] FAIL
Notes:


Issues Found:
-

Verified Features:
-
```

### FreezeFX Results
```
DAW: Ableton Live 12 Intro
Normal Usage: [ ] PASS  [ ] FAIL
Stress Test: [ ] PASS  [ ] FAIL (expected)
Notes:


Issues Found:
-
```

---

## 🚨 What to Do If Tests Fail

### If SpectralFreeze Crashes:
1. Note exact steps to reproduce
2. Check Console.app for crash logs
3. Report immediately (this should NOT happen)

### If Sampler Crashes:
1. Note exact steps to reproduce
2. Check if it's during load or during specific action
3. Report immediately (this should NOT happen)

### If FreezeFX Crashes:
1. Was it during normal usage or stress test?
2. If normal usage → Report (unexpected)
3. If stress test → Expected, document exact scenario

---

## ✅ Success Criteria

**Minimum for v1.9.0 Success**:
- ✅ SpectralFreeze loads and works in at least 1 DAW
- ✅ Sampler loads and works in at least 1 DAW
- ⚠️ FreezeFX normal usage works (stress test failure OK)

**Ideal Success**:
- ✅ All three work in all 3 DAWs
- ✅ No crashes during normal usage
- ✅ No regressions in other plugins

---

## 📝 Quick Start Testing

### 5-Minute Smoke Test (Do This First)

**Ableton Live 12**:
1. Open Ableton
2. Audio track → Add SpectralFreeze → Does it load? ✅/❌
3. MIDI track → Add Sampler → Does it load? ✅/❌
4. Audio track → Add FreezeFX → Does it load? ✅/❌
5. Play some audio through each → Does it work? ✅/❌

**If all ✅**: v1.9.0 fixes confirmed! 🎉  
**If any ❌**: Stop and report which one failed

---

## 🎯 Test Priority Order

1. **First** (5 min): Quick smoke test in Ableton Live 12
2. **Second** (10 min): Detailed SpectralFreeze test
3. **Third** (10 min): Detailed Sampler test
4. **Fourth** (5 min): FreezeFX normal usage test
5. **Optional** (10 min): Cross-DAW testing
6. **Optional** (5 min): Regression testing

**Total Time**: 25-45 minutes

---

## 📄 Deliverables

After testing, create:
- [ ] Test results summary
- [ ] Screenshots of plugins loading successfully (optional)
- [ ] Any crash logs if issues found
- [ ] Recommendation: Ship v1.9.0? Yes/No

---

**Ready to start? Begin with the 5-minute smoke test!** 🚀
