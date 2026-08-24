# Video Tutorial Script: SimpleGain Walkthrough

**Target Length**: 8-10 minutes
**Audience**: Beginners to intermediate producers
**Format**: Screen recording with voiceover + practical examples

---

## Pre-Production Checklist

**Recording Setup**:
- [ ] DAW open (Reaper or Ableton Live)
- [ ] Audio files ready:
  - Vocal track (dry, unprocessed)
  - Drum bus (mixed drums)
  - Bass track
  - Full mix (for mastering demo)
- [ ] SimpleGain plugin installed and tested
- [ ] Microphone and audio interface ready

---

## Video Structure

**Total Time**: ~9 minutes

1. **Intro** (0:00-0:30) - 30 seconds
2. **UI Overview** (0:30-1:30) - 60 seconds
3. **Basic Operation** (1:30-2:30) - 60 seconds
4. **Use Case 1: Gain Staging** (2:30-4:00) - 90 seconds
5. **Use Case 2: Volume Automation** (4:00-5:30) - 90 seconds
6. **Use Case 3: Headroom Management** (5:30-6:30) - 60 seconds
7. **Use Case 4: A/B Level Matching** (6:30-8:00) - 90 seconds
8. **Tips & Tricks** (8:00-8:45) - 45 seconds
9. **Outro** (8:45-9:00) - 15 seconds

---

## Script

### INTRO (0:00-0:30)

**[VISUAL: Title card - "SimpleGain Walkthrough"]**

**NARRATION**:
> "Welcome back to the AudioForge tutorial series! Today we're diving into SimpleGain - a clean, transparent gain plugin that's essential for proper gain staging and mixing. In this video, I'll show you four practical ways to use SimpleGain in your productions. Let's get started!"

**[VISUAL: Fade to DAW with SimpleGain loaded]**

---

### PART 1: UI OVERVIEW (0:30-1:30)

**[VISUAL: SimpleGain plugin UI, full screen]**

**NARRATION**:
> "SimpleGain has a super clean interface with just three main elements. At the top, you've got the peak and RMS meters showing your audio levels in real-time."

**[VISUAL: Mouse hover over meters, they light up]**

**NARRATION**:
> "The big knob in the center controls your gain, ranging from minus 60 dB to plus 12 dB. And below that, you see the exact value in decibels."

**[VISUAL: Drag knob up and down, show value changing]**

**NARRATION**:
> "The meters are color-coded: green is safe, yellow means you're getting loud, and red means you're hitting the danger zone near clipping."

**[VISUAL: Play audio, meters respond - green → yellow → red]**

**NARRATION**:
> "Double-click the knob to reset to zero dB, which is unity gain - no change to the signal."

**[VISUAL: Double-click knob, it snaps to 0.0 dB]**

---

### PART 2: BASIC OPERATION (1:30-2:30)

**[VISUAL: Load vocal track, insert SimpleGain]**

**NARRATION**:
> "Let me show you the basics. I've got this vocal track here that's a bit too hot. Watch what happens when I play it."

**[VISUAL: Play vocal, meters hit red]**

**NARRATION**:
> "See how the meters are hitting the red zone? That's clipping territory. Let's fix that with SimpleGain."

**[VISUAL: Drag gain knob down to -6 dB]**

**NARRATION**:
> "I'll reduce the gain by 6 dB. Now when I play it back..."

**[VISUAL: Play vocal, meters stay in green-yellow range]**

**NARRATION**:
> "Much better! The peaks are now around minus 12 dB, which gives us plenty of headroom for processing."

**[VISUAL: Show gain knob at -6.0 dB]**

**NARRATION**:
> "For precise adjustments, hold Shift while dragging for fine control, or Ctrl-click to type an exact value."

**[VISUAL: Demonstrate Shift+drag, then Ctrl+click to type]**

---

### PART 3: USE CASE 1 - GAIN STAGING (2:30-4:00)

**[VISUAL: Show plugin chain: SimpleGain → Compressor → EQ]**

**NARRATION**:
> "Use case number one: gain staging. This is probably the most important use of SimpleGain. Here's the thing - compressors, saturators, and other dynamic processors sound different at different input levels."

**[VISUAL: Solo vocal track with processing chain]**

**NARRATION**:
> "I've got a compressor here, and I want it to hit the sweet spot around minus 12 to minus 6 dB peaks. Let's insert SimpleGain before the compressor."

**[VISUAL: Insert SimpleGain before compressor in chain]**

**NARRATION**:
> "Now I'll adjust SimpleGain until the peaks hit around minus 10 dB going into the compressor."

**[VISUAL: Play vocal, adjust SimpleGain, watch meter]**

**NARRATION**:
> "Perfect! Now my compressor is receiving the optimal input level, which means it'll sound consistent and predictable."

**[VISUAL: Toggle SimpleGain bypass, show how compressor behavior changes]**

**NARRATION**:
> "Watch what happens if I bypass SimpleGain - the compressor reacts totally differently because it's getting a hotter signal. This is why gain staging matters!"

**[VISUAL: Re-enable SimpleGain]**

**NARRATION**:
> "Always use SimpleGain at the start of your processing chain to set proper levels before hitting other plugins."

---

### PART 4: USE CASE 2 - VOLUME AUTOMATION (4:00-5:30)

**[VISUAL: Show vocal track with SimpleGain]**

**NARRATION**:
> "Use case two: volume automation. Here's a pro tip - don't automate your track fader for volume changes during a performance. Instead, use SimpleGain."

**[VISUAL: Show automation lane for SimpleGain gain parameter]**

**NARRATION**:
> "Why? Because your track fader is your final mix balance control. If you automate it for fade-ins or ducking, you lose that reference point."

**[VISUAL: Draw automation: fade in at start, slight dip in middle]**

**NARRATION**:
> "I'm going to automate the Gain parameter in SimpleGain. Here's a fade-in at the beginning..."

**[VISUAL: Play vocal with fade-in automation]**

**NARRATION**:
> "...and here I'm ducking the vocal slightly when the instrumental is busy."

**[VISUAL: Show automation dip, play that section]**

**NARRATION**:
> "Now my track fader stays clean at zero, and I can still make overall mix balance adjustments without messing up my automation."

**[VISUAL: Show fader at 0 dB while SimpleGain does the work]**

**NARRATION**:
> "This workflow keeps your mix organized and makes recall sessions way easier."

---

### PART 5: USE CASE 3 - HEADROOM MANAGEMENT (5:30-6:30)

**[VISUAL: Show master bus with multiple tracks, meters near 0 dB]**

**NARRATION**:
> "Use case three: creating headroom. Let's say your mix is clipping on the master bus, but you don't want to pull down every single fader. Here's the quick fix."

**[VISUAL: Master bus meters hitting red]**

**NARRATION**:
> "Insert SimpleGain on your master bus and reduce the gain by 3 to 6 dB. Instant headroom!"

**[VISUAL: Insert SimpleGain on master, reduce to -4 dB]**

**[VISUAL: Play mix, meters now in green zone]**

**NARRATION**:
> "Now my mix has breathing room for mastering or final processing. You can also use this technique on group buses like drums or vocals."

**[VISUAL: Show drum bus with SimpleGain reducing level]**

**NARRATION**:
> "Just remember - this doesn't fix a bad mix. If you need more than 6 dB of reduction, you probably need to rebalance your individual tracks."

---

### PART 6: USE CASE 4 - A/B LEVEL MATCHING (6:30-8:00)

**[VISUAL: Two versions of a mix on separate tracks]**

**NARRATION**:
> "Use case four: A/B level matching. Here's a critical mixing rule - louder always sounds better, even if it isn't. When comparing two versions, you need to match their levels first."

**[VISUAL: Solo Mix A, play it]**

**NARRATION**:
> "I've got two mix versions here. Mix A is hitting around minus 8 dB peaks."

**[VISUAL: Note the peak level: -8 dB]**

**NARRATION**:
> "Now let's hear Mix B."

**[VISUAL: Solo Mix B, play it - louder than Mix A]**

**NARRATION**:
> "Mix B is hitting minus 4 dB - it's 4 dB louder, which makes it sound 'better' even though that might just be the volume difference."

**[VISUAL: Insert SimpleGain on Mix B]**

**NARRATION**:
> "Let's insert SimpleGain on Mix B and reduce it by 4 dB to match Mix A's level."

**[VISUAL: Adjust SimpleGain to -4.0 dB]**

**NARRATION**:
> "Now both mixes are hitting the same peak level. Let's A/B them."

**[VISUAL: Toggle between Mix A and Mix B (muted), show they're level-matched]**

**NARRATION**:
> "Perfect! Now I can make an honest judgment about which mix actually sounds better, without being fooled by loudness."

**[VISUAL: Play both, toggle between them]**

**NARRATION**:
> "This technique works for comparing plugins, mixes, masters, or even reference tracks."

---

### PART 7: TIPS & TRICKS (8:00-8:45)

**[VISUAL: SimpleGain plugin UI]**

**NARRATION**:
> "Before we wrap up, here are three quick tips. Tip one: double-click the knob to reset to zero dB instantly - super handy when you're experimenting."

**[VISUAL: Double-click knob]**

**NARRATION**:
> "Tip two: use SimpleGain before dynamic processors like compressors and gates. This gives you consistent, predictable behavior."

**[VISUAL: Show plugin chain with SimpleGain first]**

**NARRATION**:
> "And tip three: watch the RMS meter, not just the peak meter. RMS gives you a better sense of perceived loudness, which is what your ears actually hear."

**[VISUAL: Highlight RMS meter during playback]**

**NARRATION**:
> "SimpleGain is simple, but mastering gain staging will level up your mixes more than any fancy plugin ever will."

---

### OUTRO (8:45-9:00)

**[VISUAL: Fade to title card]**

**NARRATION**:
> "And that's SimpleGain! In the next video, we'll explore BasicSynth and create some killer sounds. If you found this helpful, hit like and subscribe. Check out the full user manual in the description, and I'll see you in the next one!"

**[VISUAL: End screen with links to:
- Next video: BasicSynth Tutorial
- User Manual: SimpleGain-Manual.md
- Subscribe button]**

---

## B-Roll & Graphics

**Lower Third**:
- "SimpleGain by AudioForge"
- Use case labels: "Gain Staging", "Automation", "Headroom", "A/B Matching"

**On-Screen Text** (when demonstrating):
- "Double-click to reset to 0 dB"
- "Shift+drag for fine control"
- "Ctrl/Cmd+click to type value"

**Parameter Overlays** (when adjusting):
- Show current gain value in large text: "-6.0 dB"
- Show meter readings: "Peak: -12 dB | RMS: -18 dB"

---

## Video Description Template

```
Learn how to use SimpleGain, AudioForge's clean gain/volume control plugin, for professional gain staging, volume automation, headroom management, and A/B level matching.

🎯 Topics Covered:
• User interface overview
• Basic gain adjustment
• Gain staging for compressors and processors
• Volume automation without touching faders
• Creating headroom on mix buses
• A/B level matching for honest comparisons

📥 Download SimpleGain (free): https://github.com/username/AudioForge/releases

📚 SimpleGain User Manual: [link to manual]

🔗 AudioForge Links:
• Website: https://audioforge.fluxstudio.art
• Documentation: [link]
• GitHub: https://github.com/username/AudioForge

⏱️ Chapters:
0:00 - Introduction
0:30 - User Interface Overview
1:30 - Basic Operation
2:30 - Gain Staging for Processors
4:00 - Volume Automation Workflow
5:30 - Headroom Management
6:30 - A/B Level Matching
8:00 - Tips & Tricks
8:45 - Outro

🎬 Next: BasicSynth Tutorial - [link]

#simplegain #gainsg #audioforge #mixingtips #musicproduction #gainstaging
```

---

**Production Date**: TBD
**Last Updated**: 2026-08-23
