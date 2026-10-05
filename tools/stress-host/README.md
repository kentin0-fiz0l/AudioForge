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

# Also fail any plugin whose loudest peak is above 12
build/stress-host/StressHost_artefacts/Release/StressHost --max-peak 12 path/to/Plugin.vst3

# One line per run instead of one per plugin
STRESS_VERBOSE=1 build/stress-host/StressHost_artefacts/Release/StressHost path/to/Plugin.vst3
```

Run each plugin in its own process if you are testing many, so that a crash in one does not stop the rest:

```bash
for p in ~/Library/Audio/Plug-Ins/VST3/*.vst3; do
    build/stress-host/StressHost_artefacts/Release/StressHost "$p"
done
```

The exit code is 0 when every plugin loaded and passed. Each plugin's line carries its verdict:

- `ok`
- `NON-FINITE`: some output sample was `inf` or `NaN`
- `TOO-LOUD`: the loudest peak was above the ceiling given with `--max-peak`
- `LOAD-FAILED`: the bundle held no plugin, or it could not be instantiated

## In CI

The `Build All Plugins` workflow builds this host in every shard and runs it on each plugin straight after that plugin builds, on Linux. A plugin that fails turns its shard red, and the verdict and loudest peak for every plugin are in the run summary. CI runs with `--max-peak 12`; the value is `STRESS_MAX_PEAK` at the top of `.github/workflows/build-all-plugins.yml`. With every plugin healthy the loudest is PadSynth at 9.3; the faults this host was written to catch gave 13.2 (BasicSynth), 14.4 (Reverb), 190 (Koto) and 1e38 (Gate). The ceiling is a tripwire for runaway output, not a statement that 12 is an acceptable level. A plugin that is legitimately louder needs the ceiling raised, since there are no per-plugin exceptions.

The host's settings are the same on every run, but a few plugins generate their own unseeded noise, so their peaks move a little from run to run (Shakuhachi has given 6.9 to 7.9). Leave room for that when moving the ceiling.

## Default levels

Every AudioForge instrument is calibrated to the same output level, so that swapping one for another does not change the mix by 20 dB:

> The loudest single full-velocity note, at the default settings, peaks at about -9 dBFS. Anything from -12 to -6 is accepted.

`--levels` measures it. For each instrument it plays seven notes one at a time and reports the loudest, then a four-note chord and three drum notes together. Each single note is played five times and the median taken, because instruments excited by random noise (Shakuhachi, Sitar, Koto) differ by several dB from one note to the next:

```bash
build/stress-host/StressHost_artefacts/Release/StressHost --levels path/to/Plugin.vst3

# Also fail any instrument outside the range
build/stress-host/StressHost_artefacts/Release/StressHost --levels --level-range -12 -6 path/to/Plugin.vst3
```

A line reads `Name  LEVEL  note 36  -9.1 dBFS  together  -4.2 dBFS`, or `LEVEL-OUT-OF-RANGE` in place of `LEVEL`. Effects report `EFFECT`. An instrument that makes no sound reports `SILENT` and is not failed, because the pattern generators wait for the host's transport and Sampler has nothing loaded.

An instrument is brought into range with the `outputTrim` constant at the end of its `processBlock`. To recalibrate one, measure it, then scale that constant by the difference from -9 dB.

-9 leaves room for chords: four notes together typically come out 7 to 8 dB above one. CI checks the range for every instrument, with the limits in `LEVEL_LOW` and `LEVEL_HIGH` at the top of `.github/workflows/build-all-plugins.yml`.

## Reading the results

A peak well above 1.0 is not automatically a bug. Compressors with maximum makeup gain, saturators at full drive, and filters at high resonance are legitimately loud. Look for peaks that are orders of magnitude out, output from an effect that exceeds what its controls could explain, and anything non-finite.

Runs on macOS and, without a display, on Linux. Not tried on Windows.
