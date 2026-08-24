# Video Tutorial Script: BasicSynth Sound Design Tutorial

**Target Length**: 12-15 minutes
**Audience**: Beginners to synthesis, intermediate producers
**Format**: Screen recording with hands-on sound design

---

## Pre-Production Checklist

**Recording Setup**:
- [ ] DAW with BasicSynth loaded on MIDI track
- [ ] MIDI keyboard connected (or piano roll ready)
- [ ] Headphones and monitoring setup
- [ ] Example MIDI clips:
  - Bass line (C1-C2 range)
  - Lead melody (C3-C5 range)
  - Pad chords (C2-C4 range)

---

## Video Structure

**Total Time**: ~13 minutes

1. **Intro** (0:00-0:45) - 45 seconds
2. **Synthesis Basics Explained** (0:45-2:00) - 75 seconds
3. **UI Walkthrough** (2:00-3:00) - 60 seconds
4. **Sound Design 1: Bass** (3:00-5:30) - 150 seconds
5. **Sound Design 2: Lead** (5:30-8:00) - 150 seconds
6. **Sound Design 3: Pad** (8:00-10:00) - 120 seconds
7. **Advanced Techniques** (10:00-12:00) - 120 seconds
8. **Outro** (12:00-13:00) - 60 seconds

---

## Script

### INTRO (0:00-0:45)

**[VISUAL: Title card - "BasicSynth: Sound Design Tutorial"]**

**NARRATION**:
> "What's up, producers! Today we're diving into BasicSynth, AudioForge's subtractive synthesizer. If you've never designed a sound from scratch, don't worry - I'll walk you through the entire process. We'll create a bass, a lead, and a pad, and I'll explain every step. By the end of this video, you'll understand how subtractive synthesis works and how to make your own sounds. Let's jump in!"

**[VISUAL: Fade to DAW with BasicSynth loaded]**

---

### PART 1: SYNTHESIS BASICS EXPLAINED (0:45-2:00)

**[VISUAL: BasicSynth UI with labeled sections]**

**NARRATION**:
> "Before we start, let's quickly understand what 'subtractive synthesis' means. You start with harmonically rich waveforms..."

**[VISUAL: Highlight oscillator section]**

**NARRATION**:
> "...then you subtract frequencies using a filter..."

**[VISUAL: Highlight filter section]**

**NARRATION**:
> "...and shape the volume over time with an envelope."

**[VISUAL: Highlight ADSR envelope]**

**NARRATION**:
> "That's it! Oscillators, filter, envelope. Start rich, carve it down. Let's see how this works in practice."

**[VISUAL: Play test note (C2) with default settings]**

---

### PART 2: UI WALKTHROUGH (2:00-3:00)

**[VISUAL: BasicSynth UI, full screen]**

**NARRATION**:
> "BasicSynth has three oscillators: Sine, Sawtooth, and Square. Each has a different harmonic content."

**[VISUAL: Toggle each oscillator on/off, play note, show waveform]**

**NARRATION**:
> "Sine is pure and smooth - great for sub-bass. Sawtooth is bright and buzzy with all harmonics - perfect for leads and bass. Square is hollow and nasal with only odd harmonics - classic for retro sounds."

**[VISUAL: Show filter controls]**

**NARRATION**:
> "The filter has Cutoff and Resonance. Cutoff removes high frequencies, and Resonance emphasizes the cutoff point."

**[VISUAL: Adjust cutoff from low to high, then resonance from 0 to 80%]**

**NARRATION**:
> "Finally, the ADSR envelope controls how the sound evolves over time. Attack, Decay, Sustain, Release - we'll use these to shape our sounds."

**[VISUAL: Show ADSR diagram]**

---

### PART 3: SOUND DESIGN 1 - BASS (3:00-5:30)

**[VISUAL: Reset BasicSynth to default, create MIDI note (C1)]**

**NARRATION**:
> "Let's make a bass sound. Bass needs power, so we'll use the Sawtooth oscillator for its rich harmonics."

**[VISUAL: Enable OSC 2 (Sawtooth), disable others]**

**[VISUAL: Play note - bright, buzzy sound]**

**NARRATION**:
> "Right now it's too bright. Let's use the filter to remove the highs and focus on the bass frequencies."

**[VISUAL: Lower filter cutoff to 500 Hz]**

**[VISUAL: Play note - darker, bass-like]**

**NARRATION**:
> "Much better! Now let's add some character with resonance."

**[VISUAL: Increase resonance to 40%]**

**[VISUAL: Play note - resonant bass with more presence]**

**NARRATION**:
> "Nice! Now the envelope. For a punchy bass, we want a fast attack and a short release."

**[VISUAL: Set Attack to 0.01s, Decay to 0.2s, Sustain to 50%, Release to 0.2s]**

**[VISUAL: Play staccato bass line]**

**NARRATION**:
> "There we go! Tight, punchy bass. Now let's layer in some sub-bass with the Sine oscillator for extra low-end power."

**[VISUAL: Enable OSC 1 (Sine) at 30% level, OSC 2 (Saw) at 70%]**

**[VISUAL: Play bass line - deeper, fuller]**

**NARRATION**:
> "Perfect! The sawtooth gives us mid-range punch, and the sine gives us that deep, club-rattling sub. That's how you make a professional bass sound."

**[VISUAL: Show final settings on screen:
- OSC 1 (Sine): 30%
- OSC 2 (Saw): 70%
- Cutoff: 500 Hz
- Resonance: 40%
- A: 0.01s, D: 0.2s, S: 50%, R: 0.2s]**

---

### PART 4: SOUND DESIGN 2 - LEAD (5:30-8:00)

**[VISUAL: Reset BasicSynth, create lead melody MIDI (C3-C5)]**

**NARRATION**:
> "Next, let's make a lead synth. Leads need to cut through the mix, so we'll use the Square oscillator combined with Sawtooth for brightness."

**[VISUAL: Enable OSC 2 (Saw) at 60%, OSC 3 (Square) at 40%]**

**[VISUAL: Play melody - bright, present]**

**NARRATION**:
> "Already sounding good! Now let's open up the filter to let the highs through."

**[VISUAL: Set cutoff to 5000 Hz]**

**NARRATION**:
> "And add some resonance to make it more aggressive."

**[VISUAL: Set resonance to 50%]**

**[VISUAL: Play melody - bright, cutting lead]**

**NARRATION**:
> "For the envelope, leads need a quick attack but a longer release so notes flow together."

**[VISUAL: Set Attack to 0.01s, Decay to 0.3s, Sustain to 70%, Release to 0.5s]**

**[VISUAL: Play legato melody]**

**NARRATION**:
> "Hear how the notes connect smoothly? That's the longer release at work."

**[VISUAL: Show envelope diagram while playing]**

**NARRATION**:
> "Now here's a pro tip: if you want an even fatter lead, duplicate this track and detune the second instance slightly."

**[VISUAL: Duplicate MIDI track, add second BasicSynth instance]**

**[VISUAL: Show both playing together - wider, richer sound]**

**NARRATION**:
> "That slight detuning creates a chorus effect, making the lead sound huge!"

**[VISUAL: Show final settings:
- OSC 2 (Saw): 60%
- OSC 3 (Square): 40%
- Cutoff: 5000 Hz
- Resonance: 50%
- A: 0.01s, D: 0.3s, S: 70%, R: 0.5s]**

---

### PART 5: SOUND DESIGN 3 - PAD (8:00-10:00)

**[VISUAL: Reset BasicSynth, create pad chord progression MIDI]**

**NARRATION**:
> "Finally, let's make a lush pad sound. Pads are all about smoothness and evolution, so we'll use the Sine oscillator."

**[VISUAL: Enable OSC 1 (Sine) at 100%]**

**[VISUAL: Play chord - pure, soft]**

**NARRATION**:
> "The key to a good pad is the envelope. We want a slow attack so the sound swells in gradually."

**[VISUAL: Set Attack to 2.0s]**

**[VISUAL: Play chord - slow swell]**

**NARRATION**:
> "Hear that swell? That's cinematic! Now let's add a long release so the sound lingers after we release the keys."

**[VISUAL: Set Release to 3.0s]**

**[VISUAL: Play chord, hold, then release - sound continues fading]**

**NARRATION**:
> "Beautiful! For the filter, we'll keep it relatively open but not completely."

**[VISUAL: Set cutoff to 3000 Hz, Resonance to 10%]**

**NARRATION**:
> "This gives us warmth without being too bright."

**[VISUAL: Play chord progression]**

**NARRATION**:
> "Now let's layer in the Sawtooth and Square at low levels for harmonic richness."

**[VISUAL: Enable OSC 2 (Saw) at 20%, OSC 3 (Square) at 15%, keep OSC 1 at 65%]**

**[VISUAL: Play progression - richer, more complex]**

**NARRATION**:
> "Now it's got body and depth! This is the foundation of ambient and cinematic pads."

**[VISUAL: Show final settings:
- OSC 1 (Sine): 65%
- OSC 2 (Saw): 20%
- OSC 3 (Square): 15%
- Cutoff: 3000 Hz
- Resonance: 10%
- A: 2.0s, D: 1.0s, S: 60%, R: 3.0s]**

---

### PART 6: ADVANCED TECHNIQUES (10:00-12:00)

**[VISUAL: Show all three sounds playing together - bass, lead, pad]**

**NARRATION**:
> "Now let's talk advanced techniques. First, automation. You can automate the filter cutoff to create sweeps and movement."

**[VISUAL: Draw automation on bass - cutoff moves from 300 Hz to 1200 Hz over 4 bars]**

**[VISUAL: Play bass with automation - evolving filter sweep]**

**NARRATION**:
> "Classic! You can also automate resonance for dramatic builds."

**[VISUAL: Show resonance automation from 20% to 80%]**

**NARRATION**:
> "Second technique: velocity. In your DAW, map MIDI velocity to the filter cutoff. Harder notes = brighter sound."

**[VISUAL: Show DAW MIDI mapping, demonstrate varying velocities]**

**NARRATION**:
> "This adds expressiveness - your synth responds to how hard you play."

**[VISUAL: Play lead melody with varied velocities]**

**NARRATION**:
> "And finally, effects. BasicSynth is meant to be simple, so layer it with effects."

**[VISUAL: Add Reverb to pad, CleanDelay to lead, Saturation to bass]**

**[VISUAL: Play full arrangement with effects]**

**NARRATION**:
> "Reverb on pads, delay on leads, saturation on bass - that's how you get professional synth sounds!"

---

### OUTRO (12:00-13:00)

**[VISUAL: Fade to recap screen showing 3 sound designs]**

**NARRATION**:
> "And that's BasicSynth! We made a punchy bass, a cutting lead, and a lush pad - all from the same simple synth. The key is understanding oscillators, filters, and envelopes."

**[VISUAL: Show text overlay:
- Bass: Saw + Sine, low cutoff, short envelope
- Lead: Saw + Square, high cutoff, moderate envelope
- Pad: Sine + layers, mid cutoff, long envelope]**

**NARRATION**:
> "Download the full user manual for more sound design recipes. In the next video, we'll cover SimpleComp and learn how to control dynamics like a pro."

**[VISUAL: End screen with:
- Next video: SimpleComp Tutorial
- Download: BasicSynth Manual
- Subscribe button]**

**NARRATION**:
> "If you enjoyed this, smash that like button and subscribe for more AudioForge tutorials. Thanks for watching, and keep creating!"

---

## B-Roll & Graphics

**Waveform Visualizations**:
- Sine wave (smooth curve)
- Sawtooth wave (jagged ramp)
- Square wave (rectangular)

**Parameter Overlays**:
- Show current values when adjusting:
  - "Cutoff: 500 Hz"
  - "Resonance: 40%"
  - "Attack: 2.0s"

**Sound Design Recap Cards** (end of each section):
```
BASS PRESET
─────────────
OSC 1 (Sine): 30%
OSC 2 (Saw): 70%
Cutoff: 500 Hz
Resonance: 40%
ADSR: 0.01 / 0.2 / 50% / 0.2
```

---

## Video Description Template

```
Learn sound design fundamentals with BasicSynth! In this tutorial, we create a bass sound, lead synth, and pad from scratch using subtractive synthesis.

🎹 What You'll Learn:
• Subtractive synthesis basics (oscillators, filter, envelope)
• How to make a punchy bass sound
• Designing a bright, cutting lead synth
• Creating lush, evolving pads
• Advanced techniques (automation, velocity, effects layering)

🎵 Sounds We'll Make:
1. Bass: Sawtooth + Sine, filtered and punchy
2. Lead: Sawtooth + Square, bright and aggressive
3. Pad: Layered sine waves with slow attack

📥 Download BasicSynth (free): https://github.com/username/AudioForge/releases

📚 BasicSynth User Manual: [link]

⏱️ Chapters:
0:00 - Introduction
0:45 - Subtractive Synthesis Explained
2:00 - UI Walkthrough
3:00 - Bass Sound Design
5:30 - Lead Synth Design
8:00 - Pad Sound Design
10:00 - Advanced Techniques
12:00 - Outro

🎬 Next: SimpleComp Tutorial - [link]

#basicsynth #sounddesign #synthesis #audioforge #musicproduction #tutorial
```

---

**Production Date**: TBD
**Last Updated**: 2026-08-23
