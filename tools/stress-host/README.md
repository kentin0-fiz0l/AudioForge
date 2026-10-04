# Stress host

A small command-line plugin host that looks for runaway or invalid output.

For each VST3 it is given, it runs the plugin with its parameters at their defaults, all at minimum, all at maximum, and 24 random settings biased towards the extremes. Each setting is run at 44.1, 48 and 96 kHz while it plays a full-velocity three-note chord and feeds noise into any audio inputs. It reports:

- `NON-FINITE` if any output sample is `inf` or `NaN`, with the settings that caused it
- the loudest peak seen, and the settings behind it when that peak is above 8.0

It exists because pluginval at strictness 10 passed several plugins that this caught: a filter that overflowed, a gate that amplified its input, a window function that produced `NaN`, and a string model whose loop gain exceeded 1.

## Build

```bash
cmake -S tools/stress-host -B build/stress-host -DCMAKE_BUILD_TYPE=Release
cmake --build build/stress-host --parallel 8
```

## Run

```bash
build/stress-host/StressHost_artefacts/Release/StressHost path/to/Plugin.vst3

# One line per run instead of one per plugin
STRESS_VERBOSE=1 build/stress-host/StressHost_artefacts/Release/StressHost path/to/Plugin.vst3
```

Run each plugin in its own process if you are testing many, so that a crash in one does not stop the rest:

```bash
for p in ~/Library/Audio/Plug-Ins/VST3/*.vst3; do
    build/stress-host/StressHost_artefacts/Release/StressHost "$p"
done
```

The exit code is 0 when every plugin loaded and produced only finite output.

## Reading the results

A peak well above 1.0 is not automatically a bug. Compressors with maximum makeup gain, saturators at full drive, and filters at high resonance are legitimately loud. Look for peaks that are orders of magnitude out, output from an effect that exceeds what its controls could explain, and anything non-finite.

Only tested on macOS so far.
