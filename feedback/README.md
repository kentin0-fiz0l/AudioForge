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
4. **Listen again.** The rebuilt plugin goes back into a session, and the entry is marked `resolved` or gets a follow-up.

## The log

`log.jsonl` holds one entry per line:

| Field | Meaning |
|---|---|
| `time` | When it was recorded |
| `source` | `user` (something heard), `measurement` (a number from a meter or test) or `observation` (noticed while working) |
| `plugin` | The plugin it concerns, `all`, or `new plugin` for a gap. Entries captured from Live leave this out; the plugins in use are in `live` |
| `text` | The feedback itself |
| `status` | `open` or `resolved` |
| `live` | Live's state when it was captured, or `null` if Live was not reachable |

The first twelve entries were written by hand from one session, before the capture command existed, which is why their `live` field is empty.
