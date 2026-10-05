# AI Producer - Ableton Live Remote Script

**Autonomous music generation directly in Ableton Live!**

## What It Does

Press one trigger (MIDI note C3) and AI Producer:
1. ✅ Sets tempo to 128 BPM
2. ✅ Creates 4 MIDI tracks (Drums, Bass, Chords, Lead)
3. ✅ Generates complete 72-bar arrangement
4. ✅ Creates MIDI clips with musical content
5. ✅ Ready to play immediately!

---

This is a Live Remote Script (plain Python, run by Live itself), not a plugin. It is the "agent outside the plugin" counterpart to the AI Producer VST in `plugins/AIProducer`: instead of emitting MIDI from one plugin instance, it builds the tracks and clips directly in the Live session. It also accepts a small set of OSC commands on UDP port 9000 (`/ai_producer/generate`, `/live/song/set/tempo`, `/live/song/start_playing`, `/live/song/stop_playing`, `/live/song/create_midi_track`, `/live/track/set/volume`).

## Installation

Live loads Remote Scripts from its User Library, so this folder has to be copied (or symlinked) there:

```bash
# macOS
ln -s "$(pwd)/ableton/remote-scripts/AIProducer" \
      "$HOME/Music/Ableton/User Library/Remote Scripts/AIProducer"
```

Live only scans that folder at startup, so restart Live after installing.

### Activate in Ableton:

1. **Open Ableton Live 12**

2. **Open Preferences** (Cmd+,)

3. **Go to "Link, Tempo & MIDI" tab**

4. **In the "Control Surface" dropdown**:
   - Find an empty slot
   - Select: **"AIProducer"**

5. **Input/Output**: Set to **"None"** (or your MIDI keyboard if you want to trigger with C3)

6. **Click "Close"**

---

## How to Use

### Method 1: Trigger Script (Easiest)

With Live open and AIProducer active, run the trigger from a terminal. It sends the script an OSC message on UDP port 9000:

```bash
cd ableton/remote-scripts/AIProducer
python3 trigger_generate.py            # house
python3 trigger_generate.py techno     # house, techno, dnb, hiphop or ambient
```

To watch what the script does, follow Live's log:

```bash
tail -f ~/Library/Preferences/Ableton/Live\ 12.*/Log.txt
```

The script logs when it initializes!

### Method 2: MIDI Trigger (Recommended)

1. **Connect a MIDI keyboard** (or use your computer keyboard in MIDI mode)

2. **In Preferences**:
   - Control Surface: **AIProducer**
   - Input: **Your MIDI Keyboard**

3. **Press MIDI note C3 (middle C)** → Generates a house track!

The Input has to be set to your keyboard: Live only sends the script notes from its own input port.

### Method 3: Add Keyboard Shortcut (Advanced)

Edit `AIProducer.py` to add Cmd+G shortcut (requires more Live API knowledge)

---

## What Gets Generated

### Track 1: AI Drums
- Four-on-floor kick (every beat)
- Snare/clap on 2 and 4
- 8th-note hi-hats
- 72 bars

**Load**: Drum Rack or any drum sampler

### Track 2: AI Bass
- Root notes following chord progression
- Every beat
- One octave below middle C

**Load**: Wavetable, Analog, or any bass synth

### Track 3: AI Chords
- Full chord progression (i-VI-III-VII in A minor)
- Whole note pads
- Harmonic foundation

**Load**: Wavetable, Analog, or any pad synth

### Track 4: AI Lead
- Pentatonic melody
- Only during "drop" sections
- Higher register

**Load**: Wavetable, Analog, or any lead synth

---

## Track Structure (72 bars, ~2:15)

```
Bars 0-8:    Intro (minimal)
Bars 8-16:   Build (rising energy)
Bars 16-32:  Drop (full energy!) ⚡
Bars 32-40:  Breakdown (calm)
Bars 40-48:  Build (rising again)
Bars 48-64:  Drop (peak!) ⚡
Bars 64-72:  Outro (fade)
```

---

## Quick Start

1. **Open Ableton Live 12**
2. **Activate AI Producer** in Preferences
3. **Trigger generation** (MIDI C3 or run script)
4. **Load instruments** on the 4 tracks:
   - Track 1: Drum Rack
   - Track 2-4: Synths (Wavetable/Analog)
5. **Press SPACE** → Instant progressive house track!

---

## Customizing

Edit `MusicTheory.py` to change:
- BPM (line in `generate_house_track()`)
- Key (change root note)
- Scale (Major, Minor, Dorian, etc.)
- Chord progression
- Drum patterns

Edit `MIDIGenerator.py` to change:
- Note patterns
- Velocities
- Rhythms

---

## Troubleshooting

### Script doesn't appear in Control Surface list
```bash
# Check installation
ls -la ~/Music/Ableton/User\ Library/Remote\ Scripts/AIProducer/

# Should see:
# __init__.py
# AIProducer.py
# DeviceLoader.py
# EffectChains.py
# MIDIGenerator.py
# MusicTheory.py
# OSCServer.py
# trigger_generate.py
# README.md
```

Restart Ableton if needed.

### Check if script loaded
Look in Ableton's Log file:
```bash
tail -f ~/Library/Preferences/Ableton/Live\ 12.*/Log.txt
```

Should see: `[AI Producer] initialized!`

### No tracks created
- Make sure script is activated in Preferences
- Check log for errors. A `RemoteScriptError` when Live starts means the script did not load at all
- Try triggering again with `trigger_generate.py` or MIDI C3

---

## Tests

The script can be exercised without Live. The tests stand in a small fake for Live's API, load the script the way Live does, and check the tracks, clips and notes it asks for:

```bash
python3 -m unittest discover -s ableton/tests -v
```

They show that the script loads and makes sensible calls. They cannot show that Live accepts those calls, so a change still needs trying in Live.

---

## Manual Trigger (Development)

For testing, you can trigger manually by editing `AIProducer.py` and adding:

```python
def __init__(self, c_instance):
    # ... existing code ...

    # Auto-generate on load (for testing)
    self.song.add_tracks_listener(self._on_tracks_changed)

def _on_tracks_changed(self):
    # Trigger once when tracks change
    pass
```

Or use Live's built-in console.

---

## Next Steps

1. ✅ **Try it!** Activate and trigger
2. 🎹 **Load instruments** on the generated tracks
3. 🎵 **Press play** and hear your AI track!
4. 🎨 **Customize** the music theory/patterns
5. 🚀 **Extend** with AI API calls (v2.0)

---

## Version 2.0 Roadmap

- [ ] AI integration (OpenAI/Claude/Ollama)
- [ ] Auto-load instruments
- [x] More genres (Techno, DnB, Hip-Hop, Ambient)
- [ ] Variation control
- [ ] Export MIDI files
- [ ] Humanization
- [ ] Mixing/dynamics

---

**Built with ❤️ for Ableton Live**  
Part of the AudioForge project
