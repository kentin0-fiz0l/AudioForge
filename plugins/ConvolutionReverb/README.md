# ConvolutionReverb

A reverb made from a recording of a real space: the input is convolved with an impulse response (a WAV, AIFF or FLAC file, mono or stereo, up to 20 seconds), using JUCE's `dsp::Convolution`. The wet signal goes through a pre-delay and a low and a high cut before it's mixed back with the dry. The impulse response is scaled to unit energy as it loads, so that fully wet, broadband material comes out at about the level it went in.

## Parameters

| Parameter | Range | Default |
|---|---|---|
| Mix | 0 to 100 % (dry to wet) | 30 % |
| Pre-Delay | 0 to 200 ms | 0 ms |
| Low Cut | 20 to 1000 Hz, on the wet signal | 20 Hz |
| High Cut | 1 to 20 kHz, on the wet signal | 20 kHz |
| Output | -24 to +6 dB | 0 dB |
| Bypass | on or off | off |

With no impulse response loaded there's no wet signal, only the dry at (100 - Mix) %.

## Loading an impulse response

In a DAW, use the Load IR button. The saved state keeps the file's path as an `ir` attribute on its root element, so a state with only that loads one:

```xml
<PARAMETERS ir="/Users/me/irs/hall.wav"/>
```

That's what the [render host](../../tools/render-host/README.md) takes with `--state`:

```bash
RenderHost ConvolutionReverb.vst3 --state hall.xml --param "Mix=0.35" --in dry.wav --tail 4 --out wet.wav
```

A relative path is taken from the working directory. An impulse response loaded before the plugin starts playing is in place from the first sample; one loaded while it plays crossfades in.

## Building

```bash
cmake -S plugins/ConvolutionReverb -B build/ConvolutionReverb -DCMAKE_BUILD_TYPE=Release
cmake --build build/ConvolutionReverb --parallel 8
```

Tests are in `tests/Plugins/ConvolutionReverbTests.cpp`.
