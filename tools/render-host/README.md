# Render host

A small command-line host that renders audio offline through one VST3: a MIDI file through an instrument, or a WAV file through an effect, into a 32-bit float WAV (so a hot part never clips before it's mixed). It's for making music with AudioForge's own plugins without a DAW in the loop (the [Vowbound](../../../Vowbound/arena/assets/audio/score/make_score.py) arena's score is written as notes in Python and played this way), and for hearing what a plugin does to a part.

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

# A saved state: a sampled instrument for the Sampler, a room for the convolution reverb
$RH $V/Sampler.vst3 --state violins.xml --midi melody.mid --out violins.wav
$RH $V/ConvolutionReverb.vst3 --state hall.xml --param "Mix=0.35" --in violins.wav --tail 4 --out violins-hall.wav

# A filter sweep: Filter Cutoff from closed to open over 8 seconds and back by 12
$RH $V/Polysynth.vst3 --midi pad.mid --automate "Filter Cutoff=0:0.1,8:0.9,12:0.3" --out sweep.wav
```

| Option | What it does |
|---|---|
| `--midi <file.mid>` | Notes to play: every track, with the file's tempo map |
| `--in <file.wav>` | Audio for the plugin's inputs; render at its sample rate |
| `--program <n>` | Factory program to start from |
| `--state <file.xml>` | Give the plugin this XML as its saved state, before any `--param` |
| `--param <name>=<value>` | Set a parameter by name (any case) to a normalised value, 0 to 1; repeat as needed |
| `--automate "<name>=<t>:<v>,<t>:<v>,..."` | Move a parameter during the render; repeat for more parameters |
| `--rate <hz>` | Sample rate, 48000 by default |
| `--tail <seconds>` | How long to keep going after the last note or the end of the input, 3 by default |
| `--gain <dB>` | Gain on the output |
| `--list` | Print programs and parameters instead of rendering |

It prints the file's length and loudest peak. Values are normalised because that's what a plugin takes from a host; `--list` shows the value each one means (a choice parameter's 0.34 might be "1", the second choice).

### State

`--state` reads an XML file and hands it to the plugin as though a DAW were restoring a saved set: the XML is what the plugin's own `getStateInformation` writes, and the root element has to be the one that plugin expects, or it ignores the state. Parameters the XML leaves out keep their defaults, and `--param` settings are applied after it, so they win. Paths in it are best absolute; a relative one is taken from the directory RenderHost runs in.

The two plugins that take files this way:

```xml
<!-- Sampler: an SFZ instrument -->
<Parameters sfz="/Users/me/vsco2/ViolinEnsSusVib.sfz"/>

<!-- ConvolutionReverb: an impulse response, mono or stereo (WAV, AIFF, FLAC) -->
<PARAMETERS ir="/Users/me/irs/hall.wav"/>
```

With `--list`, `--state` shows what the state set. The Sampler and the reverb print to the terminal if a file in the state can't be loaded.

### Automation

`--automate "Filter Cutoff=0:0.1,8:0.9"` moves a parameter along straight lines between breakpoints: each is a time in seconds and a normalised value (0 to 1, as with `--param`). Before the first breakpoint the parameter holds the first value, and after the last it holds the last. While anything is automated the render runs in blocks of 32 samples (under a millisecond at 48 kHz), and each block starts with the parameters where their lines are, so a sweep or a swell moves smoothly. Give `--automate` once for each parameter that moves; `--list` names them.

### MIDI controllers

Controllers in the `--midi` file reach the plugin with the notes: CC1 (mod wheel), CC7 (volume), CC11 (expression), CC64 (sustain pedal) and the rest. A VST3 has no MIDI controller messages of its own, so JUCE's wrapper presents them as the "MIDI CC" parameters (hidden from `--list`) and the host turns each controller into a change to its parameter at the same point in the block; the wrapper turns that back into a controller message for the plugin. What a plugin does with them is up to the plugin: the Sampler follows CC7, CC11 and CC64, so crescendos and swells can be written as CC11 in the MIDI file.

