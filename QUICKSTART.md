# AudioForge Quick Start

Get your first plugin running in under 5 minutes.

## Prerequisites

### macOS
- Xcode (from App Store) or Xcode Command Line Tools
- CMake: `brew install cmake`

### Windows
- Visual Studio 2019+ with C++ desktop development
- CMake: Download from cmake.org

### Linux
- GCC 7+ or Clang 6+
- Required packages:
  ```bash
  sudo apt install cmake build-essential libasound2-dev \
                   libfreetype6-dev libx11-dev libxrandr-dev \
                   libxinerama-dev libxcursor-dev
  ```

## Build Your First Plugin

```bash
# Clone the repository
git clone --recursive https://github.com/yourusername/AudioForge.git
cd AudioForge

# Build SimpleGain
cd plugins/SimpleGain
cmake -B build
cmake --build build --config Release

# The plugin is automatically installed!
```

## Where Are My Plugins?

### VST3 (auto-installed during build)
- **macOS**: `~/Library/Audio/Plug-Ins/VST3/SimpleGain.vst3`
- **Windows**: `C:\Program Files\Common Files\VST3\SimpleGain.vst3`
- **Linux**: `~/.vst3/SimpleGain.vst3`

### Standalone App
- **macOS**: `build/SimpleGain_artefacts/Standalone/SimpleGain.app`
- **Windows**: `build\SimpleGain_artefacts\Standalone\SimpleGain.exe`
- **Linux**: `build/SimpleGain_artefacts/Standalone/SimpleGain`

## Test the Plugin

### Option 1: Standalone App
Just double-click the `.app` (macOS) or `.exe` (Windows) to run it standalone.

### Option 2: In Your DAW
1. Open your DAW (Ableton, Logic, FL Studio, Reaper, etc.)
2. Rescan plugins if needed
3. Look for "AudioForge - SimpleGain" in your effects list
4. Load it on an audio track
5. Turn the knob and watch the meter!

### Option 3: Plugin Host (for testing)
Download a free plugin host:
- [REAPER](https://www.reaper.fm/download.php) (60-day trial)
- [Kushview Element](https://kushview.net/element/) (free)

## Making Changes

1. Edit source files in `plugins/SimpleGain/Source/`
2. Rebuild: `cmake --build build --config Release`
3. Reload in your DAW (or restart standalone)

## Development Workflow

```bash
# Rebuild after code changes
cmake --build build

# Clean build from scratch
rm -rf build
cmake -B build && cmake --build build

# Build in Debug mode (for debugging)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## IDE Setup

### Xcode (macOS)
```bash
cmake -B build -G Xcode
open build/SimpleGain.xcodeproj
```

### Visual Studio (Windows)
```bash
cmake -B build -G "Visual Studio 17 2022"
# Open build\SimpleGain.sln
```

### VS Code (All platforms)
Install CMake Tools extension, then:
1. Open the `AudioForge` folder
2. Select a kit (compiler)
3. Press F7 to build

## Next Steps

- Read `docs/dsp-basics.md` to understand the DSP concepts
- Read `docs/juce-guide.md` for JUCE patterns
- Check out `plugins/SimpleGain/README.md` for code walkthrough
- Look at `ROADMAP.md` to see what's next

## Troubleshooting

### Build fails with "JUCE not found"
Make sure you cloned with `--recursive` flag. If not:
```bash
git submodule update --init --recursive
```

### Plugin doesn't appear in DAW
- Rescan plugins in your DAW settings
- Check the VST3 folder is in your DAW's plugin search paths
- Try the standalone version first to verify the build works

### Code signing errors (macOS)
Expected for development. For distribution:
```bash
codesign --force --deep --sign - path/to/plugin.vst3
```

### CMake version too old
Update CMake:
```bash
# macOS
brew upgrade cmake

# Linux
pip install cmake --upgrade
```

## Getting Help

- Check existing issues on GitHub
- Read the documentation in `docs/`
- Look at JUCE forum: https://forum.juce.com/
- Ask in JUCE Discord

## Contributing

Want to help? Check `CONTRIBUTING.md` (coming soon) or open an issue with your ideas.

Enjoy making plugins! 🎚️
