# Contributing to AudioForge

Thank you for your interest in contributing to AudioForge! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
- [Development Setup](#development-setup)
- [Building Plugins](#building-plugins)
- [Running Tests](#running-tests)
- [Code Style Guidelines](#code-style-guidelines)
- [Pull Request Process](#pull-request-process)
- [Reporting Bugs](#reporting-bugs)
- [Suggesting Features](#suggesting-features)

---

## Code of Conduct

This project adheres to a Code of Conduct that all contributors are expected to follow. Please read [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) before contributing.

## How Can I Contribute?

There are many ways to contribute to AudioForge:

### 🐛 Report Bugs
Found a bug? [Open an issue](https://github.com/username/AudioForge/issues/new?template=bug_report.md) with:
- Clear description of the problem
- Steps to reproduce
- Expected vs actual behavior
- Your system info (OS, DAW, plugin version)

### 💡 Suggest Features
Have an idea? [Start a discussion](https://github.com/username/AudioForge/discussions/new?category=ideas) or [open a feature request](https://github.com/username/AudioForge/issues/new?template=feature_request.md) with:
- Clear description of the feature
- Use cases and benefits
- Example workflows

### 📝 Improve Documentation
- Fix typos or clarify existing docs
- Add examples or tutorials
- Translate documentation
- Write user guides

### 🔧 Submit Code
- Fix bugs
- Implement new features
- Optimize performance
- Add tests

### 🎨 Design Contributions
- Create plugin UI mockups
- Design icons or logos
- Improve user experience

---

## Development Setup

### Prerequisites

**All Platforms**:
- C++17 compatible compiler
- CMake 3.15 or higher
- Git

**macOS**:
- Xcode 14.0 or higher
- Xcode Command Line Tools

**Windows**:
- Visual Studio 2022 (Community Edition or higher)
- "Desktop development with C++" workload

**Linux**:
- GCC 9+ or Clang 10+
- Build essentials: `sudo apt install build-essential cmake git`
- JUCE dependencies: `sudo apt install libasound2-dev libfreetype6-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev`

### Clone the Repository

```bash
git clone https://github.com/username/AudioForge.git
cd AudioForge

# Initialize JUCE submodule
git submodule update --init --recursive
```

### Install Testing Dependencies (Optional)

```bash
# macOS/Linux
pip3 install pytest pytest-cov

# Windows
pip install pytest pytest-cov
```

---

## Building Plugins

### Quick Build (All Plugins)

**macOS/Linux**:
```bash
./build-all.sh
```

**Windows (PowerShell)**:
```powershell
.\build-all.ps1
```

### Build Single Plugin

```bash
cd plugins/SimpleGain
cmake -B build
cmake --build build --config Release

# Install to system plugin folder
cmake --install build
```

### Build Configurations

**Debug build** (for development):
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

**Release build** (optimized):
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Build Output Locations

**macOS**:
- VST3: `~/Library/Audio/Plug-Ins/VST3/`
- AU: `~/Library/Audio/Plug-Ins/Components/`

**Windows**:
- VST3: `C:\Program Files\Common Files\VST3\`

**Linux**:
- VST3: `/usr/lib/vst3/` or `~/.vst3/`

---

## Running Tests

AudioForge uses Google Test (via JUCE) for C++ tests and pytest for integration tests.

### Run All Tests

```bash
# C++ unit tests
cd tests
cmake -B build
cmake --build build
ctest --test-dir build --output-on-failure

# Python integration tests
pytest tests/ -v
```

### Run Specific Test Suite

```bash
# Run only DSP tests
ctest --test-dir build -R DSP --output-on-failure

# Run only SimpleGain tests
ctest --test-dir build -R SimpleGain --output-on-failure
```

### Code Coverage

```bash
# Generate coverage report
cd tests
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build
ctest --test-dir build
./calculate_coverage.sh
```

Coverage reports are generated in `tests/coverage/`.

---

## Code Style Guidelines

### C++ Style

We follow the **JUCE coding conventions** with some modifications:

**Naming Conventions**:
```cpp
// Classes: PascalCase
class AudioProcessor;

// Functions/methods: camelCase
void processBlock();

// Variables: camelCase
float gainValue;

// Constants: UPPER_SNAKE_CASE
const float MAX_GAIN_DB = 12.0f;

// Member variables: camelCase with m_ prefix (optional)
float m_currentGain;
```

**Formatting**:
- Indentation: 4 spaces (no tabs)
- Braces: K&R style (opening brace on same line)
- Line length: 120 characters max
- File encoding: UTF-8

**Example**:
```cpp
void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                    juce::MidiBuffer& midiMessages)
{
    const int numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] *= gainValue;
        }
    }
}
```

### Header Guards

Use `#pragma once` instead of traditional header guards:

```cpp
#pragma once

#include <JuceHeader.h>

class MyPlugin : public juce::AudioProcessor
{
    // ...
};
```

### Include Order

```cpp
// 1. Corresponding header (for .cpp files)
#include "PluginProcessor.h"

// 2. JUCE headers
#include <JuceHeader.h>

// 3. Standard library headers
#include <vector>
#include <memory>

// 4. Third-party library headers
#include "thirdparty/library.h"

// 5. Project headers
#include "shared/DSPUtils.h"
```

### Comments

**Use comments to explain WHY, not WHAT**:

```cpp
// Good: Explains rationale
// Smooth parameter changes over 50ms to avoid clicks
parameterSmoother.reset(sampleRate, 0.05);

// Bad: Restates code
// Set the ramp length to 0.05
parameterSmoother.reset(sampleRate, 0.05);
```

**Use Doxygen-style comments for public APIs**:

```cpp
/**
 * Process an audio block with gain adjustment
 *
 * @param buffer Audio buffer to process (modified in-place)
 * @param gainDB Gain adjustment in decibels (-60 to +12)
 */
void applyGain(juce::AudioBuffer<float>& buffer, float gainDB);
```

---

## Pull Request Process

### 1. Create a Feature Branch

```bash
git checkout -b feature/my-new-feature
# or
git checkout -b fix/issue-123
```

### 2. Make Your Changes

- Write clear, focused commits
- Follow code style guidelines
- Add tests for new functionality
- Update documentation as needed

### 3. Test Your Changes

```bash
# Run all tests
./run-tests.sh

# Build all plugins
./build-all.sh

# Test in a DAW (Reaper, Ableton, etc.)
```

### 4. Commit Your Changes

Write clear, descriptive commit messages:

```bash
git commit -m "Add resonance control to SimpleEQ high-pass filter

- Add resonance parameter (0.1 to 10.0)
- Update UI with new knob
- Add unit tests for filter response
- Update README with new parameter

Fixes #42"
```

**Commit message format**:
- First line: Short summary (50 chars or less)
- Blank line
- Detailed explanation (wrap at 72 chars)
- Reference issues/PRs if applicable

### 5. Push to Your Fork

```bash
git push origin feature/my-new-feature
```

### 6. Open a Pull Request

1. Go to https://github.com/username/AudioForge
2. Click "New Pull Request"
3. Select your branch
4. Fill out the PR template:
   - **Description**: What does this PR do?
   - **Motivation**: Why is this change needed?
   - **Testing**: How was this tested?
   - **Screenshots**: Add if UI changes
   - **Checklist**: Complete all items

### 7. Code Review

- Address reviewer feedback promptly
- Push additional commits to your branch
- Be open to suggestions and discussion

### 8. Merge

Once approved, a maintainer will merge your PR. Thank you for contributing!

---

## Reporting Bugs

### Before Submitting

1. **Check existing issues**: Search [GitHub Issues](https://github.com/username/AudioForge/issues) to see if it's already reported
2. **Try latest version**: Verify the bug exists in the latest release
3. **Test in multiple DAWs**: Confirm it's not DAW-specific

### Bug Report Template

Use the [bug report template](https://github.com/username/AudioForge/issues/new?template=bug_report.md) and include:

**Required**:
- Clear title (e.g., "SimpleGain crashes when loading in Ableton Live 11")
- Steps to reproduce (numbered list)
- Expected behavior
- Actual behavior
- System information:
  - OS version (e.g., Windows 11 22H2, macOS Ventura 13.5)
  - DAW and version (e.g., Ableton Live 11.3.4)
  - Plugin version (e.g., AudioForge v1.1.0)

**Optional but helpful**:
- Screenshots or screen recordings
- Console/error logs
- Sample project that reproduces the issue

---

## Suggesting Features

### Feature Request Process

1. **Check discussions**: See if someone already suggested it in [GitHub Discussions](https://github.com/username/AudioForge/discussions)
2. **Open a discussion** in the "Ideas" category
3. If there's community support, it may become an issue/PR

### Feature Request Template

Use the [feature request template](https://github.com/username/AudioForge/issues/new?template=feature_request.md) and include:

- **Problem statement**: What problem does this solve?
- **Proposed solution**: How should it work?
- **Alternatives considered**: Other ways to solve this?
- **Use cases**: Who benefits from this feature?

---

## Development Resources

### JUCE Documentation
- [JUCE Tutorials](https://docs.juce.com/master/tutorial_new_projucer_project.html)
- [JUCE API Reference](https://docs.juce.com/master/index.html)
- [JUCE Forum](https://forum.juce.com/)

### DSP Resources
- [AudioForge DSP Guide](docs/dsp-basics.md) - Our internal DSP documentation
- [The Scientist and Engineer's Guide to DSP](http://www.dspguide.com/)
- [JUCE DSP Module](https://docs.juce.com/master/group__juce__dsp.html)

### Plugin Development
- [VST3 SDK Documentation](https://steinbergmedia.github.io/vst3_doc/)
- [Audio Developer Conference (ADC) Videos](https://www.youtube.com/c/JUCElibrary)

---

## License

By contributing to AudioForge, you agree that your contributions will be licensed under the [GNU General Public License v3.0](LICENSE).

---

## Questions?

- **General questions**: [GitHub Discussions](https://github.com/username/AudioForge/discussions)
- **Security issues**: See [SECURITY.md](SECURITY.md)
- **Other inquiries**: contact@audioforge.art

---

Thank you for contributing to AudioForge! 🎵
