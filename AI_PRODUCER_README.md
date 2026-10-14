# Track Sketch (plugins/AIProducer)

**A plugin that writes a whole arrangement (drums, bass, chords and lead) as MIDI and plays it in time with your DAW.**

Formats: VST3, AU, Standalone. The source lives in `plugins/AIProducer`; the plugin was called AI Producer before 2.1.1, and hosts still find saved sessions under the new name.

Track Sketch has no AI in it: it writes from musical rules, so the same settings always give the same track and it works offline. It is separate from the **AI Producer Remote Script** for Ableton Live (`ableton/remote-scripts/AIProducer`), which is where the AI work happens. A later version is planned to let the Remote Script send Track Sketch a plan to play.

## Quick start

1. Put Track Sketch on a MIDI track. Make four more MIDI tracks with instruments and route Track Sketch's output to them by channel:

   | Channel | Part |
   |---|---|
   | 10 | Drums (General MIDI notes: kick 36, snare 38, clap 39, hats 42 and 46, crash 49) |
   | 1 | Bass |
   | 2 | Chords |
   | 3 | Lead |

2. Choose a **Genre**, **Key**, **Scale** and **Length**, and switch parts on or off.
3. Set your DAW's tempo to the one shown under the controls, then press play from bar 1.
4. Press **New Variation** for a different chord progression and lead with the same settings.

The track is laid out along the song timeline from bar 1, so stopping, looping and moving the playhead all behave as you would expect; sounding notes are released when the transport stops or jumps. Nothing plays while the transport is stopped. Any control change rebuilds the track within a fifth of a second, including host automation, and the controls are saved with the session.

### In Ableton Live

Live can take MIDI from another track but cannot pick out one channel of it, so use one copy of Track Sketch per part:

1. Make four MIDI tracks (Sketch Drums, Sketch Bass, Sketch Chords, Sketch Lead), put Track Sketch on each, and set each one's **Part** to its part.
2. Make four instrument tracks. On each, set **MIDI From** to its Sketch track, the box below to **Track Sketch**, and **Monitor** to **In**.
3. Give all four copies the same Genre, Key, Scale, Length and Variation. The same settings always write the same notes, so the copies play together.

The AI Producer Remote Script can do step 2 for you: `python3 live_control.py /live/track/set/input Bass "Sketch Bass" "Track Sketch"`.

## Controls

| Control | Values |
|---|---|
| Genre | House (124 BPM), Techno (130), Trance (138), Hip-Hop (90), Trap (140), Drum & Bass (174) |
| Key | C to B |
| Scale | Minor, Major |
| Length | 8-bar loop, 32 bars (intro, build, drop), Full song (72 bars: intro, build, drop, breakdown, build, drop, outro) |
| Drums, Bass, Chords, Lead | On or off |
| Variation | 1 to 100: picks the chord progression (four per scale) and writes the lead phrases |
| Part | All, or just Drums, Bass, Chords or Lead (for hosts such as Live that cannot split MIDI by channel) |

Each genre has its own drum pattern, bass rhythm and lead rhythm. Chords are triads built from the scale, one per bar, following the progression; the bass plays each chord's root; the lead is a phrase in scale steps above the chord, with an answering phrase every fourth bar.

## How the arrangement builds

Which parts play depends mostly on each section's energy (`MIDIGenerator::partPlays`):

| Section | Plays |
|---|---|
| Intro | Kick, hats, chords |
| Build | Adds backbeat and bass; the last bar is a rising snare roll |
| Drop | Everything, with a crash on the first beat |
| Breakdown | No kick or bass: hats, chords and the lead over them |
| Outro | Hats and chords |

## Code

| File | What it does |
|---|---|
| `TrackStructure.h` | The plan for a track: genre, tempo, key, progression, sections and parts |
| `Styles.h/.cpp` | The genres, and `build()`, which turns the controls into a plan |
| `MusicTheory.h/.cpp` | Scales, scale degrees and Roman-numeral progressions |
| `MIDIGenerator.h/.cpp` | Writes a plan as notes (message thread) and plays them block by block (audio thread) |
| `PluginProcessor.h/.cpp` | Parameters, rebuilding when they change, saved state, transport-following playback |
| `PluginEditor.h/.cpp` | The controls |

The audio thread never builds a track or waits for one: a new track is swapped in under a spin lock that the audio thread only tries to take, and freed on the message thread.

## Tests

`tests/Plugins/AIProducerTests.cpp`: every genre and length gives a well-formed track (sorted, no overlapping notes, everything released); bass, chords and lead stay in key for all 24 keys; the same settings give the same track; the arrangement builds as described above; playback follows the transport; controls rebuild the track and survive save and load; damaged saved state is ignored.

```bash
cmake -S tests -B build/tests -DCMAKE_BUILD_TYPE=Release
cmake --build build/tests --parallel
./build/tests/TestRunner
```
