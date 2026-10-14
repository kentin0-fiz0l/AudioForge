# Render host

A small command-line host that renders audio offline through one VST3: a MIDI file through an instrument, or a WAV file through an effect, into a 24-bit WAV. It's for making music with AudioForge's own plugins without a DAW in the loop (the [Vowbound](../../../Vowbound/arena/assets/audio/score/make_score.py) arena's score is written as notes in Python and played this way), and for hearing what a plugin does to a part.

## Build

```bash
cmake -S tools/render-host -B build/render-host -DCMAKE_BUILD_TYPE=Release
cmake --build build/render-host --parallel 8
```

## Run

```bash
RH=build/render-host/RenderHost_artefacts/Release/RenderHost
V=~/Library/Audio/Plug-Ins/VST3

# What a plugin has: its programs, and each parameter's value and what it means
$RH $V/StringEnsemble.vst3 --list

# What a value means: set it, then list
$RH $V/StringEnsemble.vst3 --param "String Type=0.67" --list    # Cello

# Notes through an instrument
$RH $V/PadSynth.vst3 --midi chords.mid --param "Attack=0.75" --out pad.wav

# Audio through an effect
$RH $V/ShimmerReverb.vst3 --in pad.wav --param "Mix=0.5" --tail 6 --out pad-shimmer.wav
```

| Option | What it does |
|---|---|
| `--midi <file.mid>` | Notes to play: every track, with the file's tempo map |
| `--in <file.wav>` | Audio for the plugin's inputs; render at its sample rate |
| `--program <n>` | Factory program to start from |
| `--param <name>=<value>` | Set a parameter by name (any case) to a normalised value, 0 to 1; repeat as needed |
| `--rate <hz>` | Sample rate, 48000 by default |
| `--tail <seconds>` | How long to keep going after the last note or the end of the input, 3 by default |
| `--gain <dB>` | Gain on the output |
| `--list` | Print programs and parameters instead of rendering |

It prints the file's length and loudest peak. Values are normalised because that's what a plugin takes from a host; `--list` shows the value each one means (a choice parameter's 0.34 might be "1", the second choice).
