# 🧪 AI Producer - Testing Guide

## Quick Test in Your DAW

### Option 1: Ableton Live (Recommended for Quick Test)

1. **Open Ableton Live**

2. **Rescan Plugins** (if needed):
   - Preferences → Plug-ins → VST3 Plugin Custom Folder
   - Add: `~/Library/Audio/Plug-Ins/VST3/`
   - Click "Rescan"

3. **Create Test Setup**:
   ```
   Track 1: MIDI Track
   └─ AI Producer (loaded as instrument)
   
   Track 2: MIDI Track  
   └─ Drum Rack or Drum Synth
   └─ MIDI From: Track 1, Channel 10 (Drums)
   └─ Monitor: In
   
   Track 3: MIDI Track
   └─ Analog/Wavetable (Bass sound)
   └─ MIDI From: Track 1, Channel 1 (Bass)
   └─ Monitor: In
   
   Track 4: MIDI Track
   └─ Analog/Wavetable (Pad sound)
   └─ MIDI From: Track 1, Channel 2 (Chords)
   └─ Monitor: In
   
   Track 5: MIDI Track
   └─ Analog/Wavetable (Lead sound)
   └─ MIDI From: Track 1, Channel 3 (Lead)
   └─ Monitor: In
   ```

4. **Generate Track**:
   - Click on Track 1 (AI Producer)
   - In the plugin window, type: "Create a progressive house track"
   - Click "🎵 Generate Track"
   - Wait for status to say "Track generated!"

5. **Test Playback**:
   - Press Space to play
   - You should hear:
     - Drums (kick on every beat, snare on 2 & 4)
     - Bass (root notes)
     - Chords (harmonic foundation)
     - Lead (melody during drops)

6. **Check Arrangement**:
   - Enable MIDI recording on tracks 2-5
   - Press Record
   - Play through the track once
   - Stop - you'll see MIDI recorded on all tracks
   - View the MIDI clips to see the generated notes

---

### Option 2: Logic Pro

1. **Open Logic Pro**

2. **Create Project**:
   - Empty Project
   - Don't add any tracks yet

3. **Add AI Producer**:
   - Track → New Software Instrument Track
   - In Channel Strip, click Instrument slot
   - AU Instruments → AudioForge → AI Producer
   - (If not showing, run: `auval -a` in Terminal to validate)

4. **Add Receiving Tracks**:
   ```
   Track 1: AI Producer
   
   Track 2: Software Instrument (Drums)
   └─ Inspector → MIDI Input → Track 1, Channel 10
   └─ Load EXS24/Drum Kit
   
   Track 3: Software Instrument (Bass)
   └─ MIDI Input → Track 1, Channel 1
   └─ Load ES2 (Bass preset)
   
   Track 4: Software Instrument (Chords)
   └─ MIDI Input → Track 1, Channel 2
   └─ Load Alchemy (Pad preset)
   
   Track 5: Software Instrument (Lead)
   └─ MIDI Input → Track 1, Channel 3
   └─ Load Alchemy (Lead preset)
   ```

5. **Generate**:
   - Click Track 1
   - Open AI Producer plugin window
   - Type prompt, click Generate
   - Play transport

---

### Option 3: Reaper

1. **Open Reaper**

2. **Create Track**:
   - Insert → New Track
   - Track 1 → FX → Add → VST3 → AI Producer

3. **Route MIDI**:
   ```
   Track 1: AI Producer
   
   Track 2: New Track
   └─ FX: ReaSynth or any synth (Drums)
   └─ Input: MIDI → Track 1, All Channels → Channel 10
   
   Track 3: New Track
   └─ FX: ReaSynth (Bass)
   └─ Input: MIDI → Track 1 → Channel 1
   
   Track 4: New Track
   └─ FX: ReaSynth (Chords)
   └─ Input: MIDI → Track 1 → Channel 2
   
   Track 5: New Track
   └─ FX: ReaSynth (Lead)
   └─ Input: MIDI → Track 1 → Channel 3
   ```

4. **Generate & Play**:
   - Open AI Producer on Track 1
   - Generate track
   - Press Space to play

---

## What to Expect

### Timeline (72 bars @ 128 BPM ≈ 2:15 duration)

```
00:00 - 00:15  |  Intro (8 bars)
               |  • Drums start (four-on-floor kick)
               |  • Bass enters
               |  • Minimal energy
               |
00:15 - 00:30  |  Build 1 (8 bars)
               |  • Chords enter
               |  • Energy rising
               |  • Hats get more active
               |
00:30 - 01:00  |  Drop 1 (16 bars) ⚡
               |  • FULL ENERGY
               |  • Lead melody enters
               |  • All elements playing
               |  • Classic house drop
               |
01:00 - 01:15  |  Breakdown (8 bars)
               |  • Drums pull back
               |  • Chords remain
               |  • Calm, emotional section
               |
01:15 - 01:30  |  Build 2 (8 bars)
               |  • Tension rising again
               |  • Drums building
               |
01:30 - 02:00  |  Drop 2 (16 bars) ⚡
               |  • PEAK ENERGY
               |  • All elements
               |  • Lead melody
               |
02:00 - 02:15  |  Outro (8 bars)
               |  • Elements fade out
               |  • Ending
```

### What You'll Hear

**Drums (Channel 10)**:
- Kick: Every beat (1, 2, 3, 4)
- Snare/Clap: Beats 2 and 4
- Hi-hats: 8th notes throughout
- Open hats: Occasional accents

**Bass (Channel 1)**:
- Root notes: A (57 MIDI note)
- Pattern: Whole notes or half notes
- Following i-VI-III-VII progression

**Chords (Channel 2)**:
- Root + Fifth intervals
- Whole note sustains
- Providing harmonic foundation

**Lead (Channel 3)**:
- Only during DROP sections
- Pentatonic scale melody
- 8th note pattern
- Higher register (C5-C6)

---

## Verification Checklist

✅ **Loading**:
- [ ] Plugin shows up in DAW plugin list
- [ ] Plugin opens without crashes
- [ ] UI appears with dark blue background
- [ ] Text input box visible
- [ ] Generate button visible

✅ **Generation**:
- [ ] Can type in text box
- [ ] Generate button responds to click
- [ ] Status updates (shows "Generating...")
- [ ] Status changes to "Track generated!"
- [ ] No crashes during generation

✅ **MIDI Output**:
- [ ] Drums play on Channel 10
- [ ] Bass plays on Channel 1
- [ ] Chords play on Channel 2
- [ ] Lead plays on Channel 3
- [ ] All instruments in sync
- [ ] Arrangement follows structure (intro→build→drop→etc)

✅ **Musical Quality**:
- [ ] Drums sound rhythmic (not random)
- [ ] Bass follows chord progression
- [ ] Chords are harmonically correct
- [ ] Lead melody is musical
- [ ] BPM is around 128
- [ ] Track is 72 bars long (~2:15)

---

## Troubleshooting

### Plugin doesn't appear in DAW
```bash
# Verify installation
ls -lh ~/Library/Audio/Plug-Ins/VST3/ | grep "AI Producer"

# If not there, reinstall:
cp -r "/Users/kentino/Projects/Active/AudioForge/plugins/AIProducer/build/AIProducer_artefacts/Release/VST3/AI Producer.vst3" ~/Library/Audio/Plug-Ins/VST3/
```

### No MIDI output
- Check MIDI routing in your DAW
- Verify tracks are set to correct channels (10, 1, 2, 3)
- Enable input monitoring on receiving tracks
- Check that AI Producer is on MIDI channel "All" or "Multi"

### Plugin crashes on load
```bash
# Check build output for errors
tail -100 /Users/kentino/Projects/Active/AudioForge/plugins/AIProducer/build/AIProducer_artefacts/Release/build.log
```

### Generation does nothing
- Check status label in UI
- Current MVP uses instant generation (no waiting)
- If status doesn't change, check Console.app for errors

---

## Expected Console Output

If you check Console.app while testing, you might see:
```
AI Producer: Generating track...
AI Producer: Structure created - 72 bars
AI Producer: Generating drums...
AI Producer: Generating bass...
AI Producer: Generating chords...
AI Producer: Generating lead...
AI Producer: MIDI generation complete - 500+ notes
AI Producer: Ready to play
```

---

## Recording the Generated MIDI

To capture the MIDI for editing:

1. **Arm tracks 2-5 for recording**
2. **Enable MIDI input monitoring**
3. **Press Record in DAW**
4. **Let it play through once** (2:15)
5. **Stop**

Now you have MIDI clips on all tracks that you can:
- Edit in piano roll
- Quantize
- Transpose
- Duplicate sections
- Rearrange
- Export

---

## Next Steps After Testing

If everything works:
1. **Export the MIDI** to use in other projects
2. **Try different prompts** (when AI integration is added)
3. **Customize the generated MIDI** in piano roll
4. **Add your own instruments** with better sounds
5. **Report any bugs** you find

If something doesn't work:
1. Check Console.app for errors
2. Verify MIDI routing
3. Try rebuilding the plugin
4. Report the issue with details

---

## Success Criteria

✅ **Plugin loads without crashing**  
✅ **UI is responsive**  
✅ **Generate button works**  
✅ **MIDI plays back in DAW**  
✅ **All 4 instruments are audible**  
✅ **Arrangement structure is clear (intro/build/drop/etc)**  
✅ **Music sounds coherent (not random)**  

If all checkboxes pass → **AI Producer MVP is working!** 🎉

---

## Quick Command Reference

```bash
# Check if plugin is installed
ls -lh ~/Library/Audio/Plug-Ins/VST3/ | grep "AI Producer"

# Reinstall if needed
cp -r "/Users/kentino/Projects/Active/AudioForge/plugins/AIProducer/build/AIProducer_artefacts/Release/VST3/AI Producer.vst3" ~/Library/Audio/Plug-Ins/VST3/

# Check AU (for Logic)
ls -lh ~/Library/Audio/Plug-Ins/Components/ | grep "AI Producer"

# Validate AU (Logic users)
auval -v aufx AIPr AFrg

# Rebuild if needed
cd /Users/kentino/Projects/Active/AudioForge/plugins/AIProducer
cmake --build build --config Release
```

---

## Report Template

After testing, report results:

```
✅ **Working**:
- [ ] Plugin loads
- [ ] UI appears
- [ ] Generate button works
- [ ] MIDI outputs
- [ ] Music is coherent

❌ **Issues**:
- [ ] List any problems

🎵 **Feedback**:
- How does it sound?
- What should be improved?
- Feature requests?
```

---

**Now load it in your DAW and test!** 🚀
