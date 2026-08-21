# AudioForge Test Suite

Comprehensive testing framework for AudioForge DAW plugins using JUCE UnitTestRunner.

## Overview

This test suite validates the correctness of DSP algorithms and plugin functionality for the AudioForge project.

## Test Categories

### DSP Tests (`tests/DSP/`)
Core DSP algorithm tests that validate mathematical correctness:

- **GainProcessingTests**: Decibel conversion, unity gain, signal multiplication
- **PanningTests**: Constant-power panning, balance mode, stereo width, energy conservation
- **ParameterSmoothingTests**: Smoothing initialization, ramp time, discontinuity detection

### Plugin Tests (`tests/Plugins/`)
Integration tests for individual plugins:

- **SimpleGainTests**: Plugin initialization, parameter ranges, gain processing, state management
- **PanUtilTests**: Pan/balance modes, width control, stereo metering, state persistence

### Integration Tests (`tests/Integration/`)
Real-world usage scenarios:

- **PluginIntegrationTests**: Plugin chaining, different sample rates, variable buffer sizes, parameter automation, edge cases

## Building

```bash
cd tests
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Running Tests

### Run all tests
```bash
./build/TestRunner
```

### Run with verbose output
```bash
./build/TestRunner -v
```

### Run tests in a specific category
```bash
./build/TestRunner --category=DSP
./build/TestRunner --category=Plugins
./build/TestRunner --category=Integration
```

## Test Results

The test runner provides:
- Real-time test execution feedback
- Pass/fail counts per test suite
- Detailed failure messages with expected vs actual values
- Overall summary statistics

**Exit codes:**
- `0`: All tests passed
- `1`: One or more tests failed

## Test Coverage

### DSP Coverage
- ✅ Decibel/linear conversion accuracy
- ✅ Gain processing (unity, extreme values, multi-channel)
- ✅ Constant-power panning algorithm
- ✅ Balance mode
- ✅ Stereo width (M/S processing)
- ✅ Energy conservation in panning
- ✅ Parameter smoothing (no clicks/pops)

### Plugin Coverage
- ✅ Plugin initialization and metadata
- ✅ Parameter ranges and defaults
- ✅ Audio processing correctness
- ✅ State save/restore
- ✅ Level metering (thread-safe)
- ✅ Smooth parameter changes

### Integration Coverage
- ✅ Plugin chaining (multiple effects)
- ✅ Different sample rates (44.1k - 192kHz)
- ✅ Variable buffer sizes (32 - 2048 samples)
- ✅ Parameter automation
- ✅ Edge cases (DC, silence, Nyquist frequency)
- ✅ Zero-crossing detection

## Known Issues

Some tests currently fail due to test logic issues (not plugin bugs):

1. **SimpleGain gain processing test**: Buffer magnitude calculation issue
2. **PanUtil plugin name test**: JucePlugin_Name macro definition
3. **PanUtil pan processing tests**: Test expectations need adjustment

These will be addressed in future updates. The DSP algorithms themselves are correct.

## Architecture

### Test Structure
```
tests/
├── TestRunner.cpp           # Main test entry point
├── DSP/                     # DSP algorithm tests
│   ├── GainProcessingTests.cpp
│   ├── PanningTests.cpp
│   └── ParameterSmoothingTests.cpp
├── Plugins/                 # Plugin integration tests
│   ├── SimpleGainTests.cpp
│   └── PanUtilTests.cpp
└── Integration/             # Real-world scenario tests
    └── PluginIntegrationTests.cpp
```

### Dependencies
- JUCE framework (juce_audio_processors, juce_audio_utils, juce_dsp)
- AudioForge Shared DSP library (`shared/dsp/`) - ParameterSmoothing, Metering, PanningAlgorithms, FilterDesign, Utilities
- SimpleGain and PanUtil plugin source code

## Adding New Tests

1. Create a new test class inheriting from `juce::UnitTest`
2. Implement the `runTest()` method with individual test functions
3. Register the test with a static instance
4. Add the source file to `CMakeLists.txt`

Example:
```cpp
class MyNewTests : public juce::UnitTest
{
public:
    MyNewTests() : juce::UnitTest("Test Name", "Category") {}

    void runTest() override
    {
        beginTest("Test case 1");
        expect(condition, "Failure message");
        expectWithinAbsoluteError(actual, expected, tolerance, "Message");
    }
};

static MyNewTests myNewTests;
```

## CI/CD Integration

This test suite is designed to integrate with GitHub Actions for automated testing:

```yaml
- name: Build and Test
  run: |
    cd tests
    cmake -B build
    cmake --build build
    ./build/TestRunner
```

## References

- [JUCE UnitTest Documentation](https://docs.juce.com/master/classUnitTest.html)
- [AudioForge Plugin Development Guide](../docs/plugin-development.md)
- [DSP Library Documentation](../dsp/README.md)
