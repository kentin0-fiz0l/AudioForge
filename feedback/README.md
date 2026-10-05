# Feedback

What using the AudioForge plugins in real sessions has turned up, kept so it can drive fixes to existing plugins and ideas for new ones.

## How it works

1. **Capture.** While a track plays in Live, feedback is recorded together with what Live was doing at that moment: every track's level and devices, each device's settings, and the meters.

   ```bash
   # From the repository root, with Live open and AIProducer active
   python3 ableton/remote-scripts/AIProducer/live_control.py feedback "the hi-hat is too bright"
   ```

2. **Decide.** Each entry in `log.jsonl` points at a plugin, or at a gap that a new plugin would fill.
3. **Change.** The plugin is fixed or built, with a test that shows the problem first.
4. **Listen again.** The entry is marked `fixed` when the change is installed. The rebuilt plugin then goes back into a session, and the entry is marked `resolved` or gets a follow-up.

## The log

`log.jsonl` holds one entry per line:

| Field | Meaning |
|---|---|
| `time` | When it was recorded |
| `source` | `user` (something heard), `measurement` (a number from a meter or test) or `observation` (noticed while working) |
| `plugin` | The plugin it concerns, `all`, or `new plugin` for a gap. Entries captured from Live leave this out; the plugins in use are in `live` |
| `text` | The feedback itself |
| `status` | `open`, `fixed` (changed and installed, not yet listened to), `resolved` (listened to and accepted) or `noted` (a measurement kept for reference, with nothing to change) |
| `fix` | For a fixed entry: the pull request and a line on what changed |
| `live` | Live's state when it was captured, or `null` if Live was not reachable |

The first twelve entries were written by hand from one session, before the capture command existed, which is why their `live` field is empty.

## Levels

Several entries are about level, so the standard they led to is written down here.

- **Instruments.** The loudest single full-velocity note at default settings peaks at about -9 dBFS, and must fall between -12 and -6. Every factory patch is held to the same range. CI checks it on every build.
- **Effects.** At default settings an effect should leave a track's level where it was. CI reports what each one does to a test signal; it does not fail the build on it.
- **In a mix.** One note at -9 dBFS is not a whole part. Chords and a full drum pattern add up: four calibrated instruments playing a house loop with every fader at 0 dB peaked at -1.7 dBFS on the master. Expect to pull faders down a few dB, or to put a limiter on the master.
- **Velocity.** The level standard is measured at full velocity. A clip written at lower velocities plays quieter on any instrument that follows velocity, which DrumSynth has done since #34.
