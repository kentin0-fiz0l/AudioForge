import type { Metadata } from 'next';

export const metadata: Metadata = {
  title: 'Documentation | AudioForge',
  description: 'Learn how to build audio plugins with AudioForge. Complete guides on DSP basics, JUCE framework, and plugin development.',
  openGraph: {
    title: 'Documentation | AudioForge',
    description: 'Learn how to build audio plugins with AudioForge. Complete guides on DSP basics, JUCE framework, and plugin development.',
  },
};

export default function DocsPage() {
  return (
    <div className="min-h-screen bg-zinc-950 text-white">
      {/* Hero Section */}
      <section className="border-b border-zinc-800 bg-gradient-to-b from-zinc-900 to-zinc-950">
        <div className="max-w-7xl mx-auto px-6 py-16 lg:py-24">
          <h1 className="text-4xl lg:text-5xl font-bold mb-4">Documentation</h1>
          <p className="text-xl text-zinc-400 max-w-3xl">
            Everything you need to build professional audio plugins with JUCE and C++
          </p>
        </div>
      </section>

      {/* Table of Contents */}
      <section className="border-b border-zinc-800">
        <div className="max-w-7xl mx-auto px-6 py-12">
          <div className="grid md:grid-cols-3 gap-6">
            <a href="#getting-started" className="p-6 border border-zinc-800 rounded-lg hover:border-orange-500/50 transition-colors">
              <h3 className="text-xl font-semibold mb-2 text-orange-500">Getting Started</h3>
              <p className="text-zinc-400">Build your first plugin in under 5 minutes</p>
            </a>
            <a href="#dsp-basics" className="p-6 border border-zinc-800 rounded-lg hover:border-orange-500/50 transition-colors">
              <h3 className="text-xl font-semibold mb-2 text-orange-500">DSP Basics</h3>
              <p className="text-zinc-400">Fundamental audio processing concepts</p>
            </a>
            <a href="#juce-guide" className="p-6 border border-zinc-800 rounded-lg hover:border-orange-500/50 transition-colors">
              <h3 className="text-xl font-semibold mb-2 text-orange-500">JUCE Guide</h3>
              <p className="text-zinc-400">Framework patterns and best practices</p>
            </a>
          </div>
        </div>
      </section>

      {/* Getting Started Section */}
      <section id="getting-started" className="border-b border-zinc-800">
        <div className="max-w-4xl mx-auto px-6 py-16">
          <h2 className="text-3xl font-bold mb-8">Getting Started</h2>

          <div className="space-y-8">
            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Prerequisites</h3>

              <div className="space-y-6">
                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">macOS</h4>
                  <ul className="list-disc list-inside space-y-2 text-zinc-400 ml-4">
                    <li>Xcode (from App Store) or Xcode Command Line Tools</li>
                    <li>CMake: <code className="text-sm bg-zinc-900 px-2 py-1 rounded text-orange-500">brew install cmake</code></li>
                  </ul>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Windows</h4>
                  <ul className="list-disc list-inside space-y-2 text-zinc-400 ml-4">
                    <li>Visual Studio 2019+ with C++ desktop development</li>
                    <li>CMake: Download from cmake.org</li>
                  </ul>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Linux</h4>
                  <ul className="list-disc list-inside space-y-2 text-zinc-400 ml-4">
                    <li>GCC 7+ or Clang 6+</li>
                    <li>Required packages:</li>
                  </ul>
                  <pre className="mt-2 bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                    <code className="text-orange-400">{`sudo apt install cmake build-essential libasound2-dev \\
                 libfreetype6-dev libx11-dev libxrandr-dev \\
                 libxinerama-dev libxcursor-dev`}</code>
                  </pre>
                </div>
              </div>
            </div>

            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Build Your First Plugin</h3>
              <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                <code className="text-orange-400">{`# Clone the repository
git clone --recursive https://github.com/yourusername/AudioForge.git
cd AudioForge

# Build SimpleGain
cd plugins/SimpleGain
cmake -B build
cmake --build build --config Release

# The plugin is automatically installed!`}</code>
              </pre>
            </div>

            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Where Are My Plugins?</h3>

              <div className="space-y-4">
                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">VST3 (auto-installed during build)</h4>
                  <ul className="list-disc list-inside space-y-2 text-zinc-400 ml-4">
                    <li><strong>macOS:</strong> <code className="text-sm bg-zinc-900 px-2 py-1 rounded">~/Library/Audio/Plug-Ins/VST3/SimpleGain.vst3</code></li>
                    <li><strong>Windows:</strong> <code className="text-sm bg-zinc-900 px-2 py-1 rounded">C:\Program Files\Common Files\VST3\SimpleGain.vst3</code></li>
                    <li><strong>Linux:</strong> <code className="text-sm bg-zinc-900 px-2 py-1 rounded">~/.vst3/SimpleGain.vst3</code></li>
                  </ul>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Standalone App</h4>
                  <ul className="list-disc list-inside space-y-2 text-zinc-400 ml-4">
                    <li><strong>macOS:</strong> <code className="text-sm bg-zinc-900 px-2 py-1 rounded">build/SimpleGain_artefacts/Standalone/SimpleGain.app</code></li>
                    <li><strong>Windows:</strong> <code className="text-sm bg-zinc-900 px-2 py-1 rounded">build\SimpleGain_artefacts\Standalone\SimpleGain.exe</code></li>
                    <li><strong>Linux:</strong> <code className="text-sm bg-zinc-900 px-2 py-1 rounded">build/SimpleGain_artefacts/Standalone/SimpleGain</code></li>
                  </ul>
                </div>
              </div>
            </div>

            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Test the Plugin</h3>

              <div className="space-y-4">
                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Option 1: Standalone App</h4>
                  <p className="text-zinc-400">Just double-click the .app (macOS) or .exe (Windows) to run it standalone.</p>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Option 2: In Your DAW</h4>
                  <ol className="list-decimal list-inside space-y-2 text-zinc-400 ml-4">
                    <li>Open your DAW (Ableton, Logic, FL Studio, Reaper, etc.)</li>
                    <li>Rescan plugins if needed</li>
                    <li>Look for &quot;AudioForge - SimpleGain&quot; in your effects list</li>
                    <li>Load it on an audio track</li>
                    <li>Turn the knob and watch the meter!</li>
                  </ol>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Option 3: Plugin Host (for testing)</h4>
                  <p className="text-zinc-400 mb-2">Download a free plugin host:</p>
                  <ul className="list-disc list-inside space-y-2 text-zinc-400 ml-4">
                    <li><a href="https://www.reaper.fm/download.php" className="text-orange-500 hover:underline">REAPER</a> (60-day trial)</li>
                    <li><a href="https://kushview.net/element/" className="text-orange-500 hover:underline">Kushview Element</a> (free)</li>
                  </ul>
                </div>
              </div>
            </div>

            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Development Workflow</h3>
              <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                <code className="text-orange-400">{`# Rebuild after code changes
cmake --build build

# Clean build from scratch
rm -rf build
cmake -B build && cmake --build build

# Build in Debug mode (for debugging)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build`}</code>
              </pre>
            </div>
          </div>
        </div>
      </section>

      {/* DSP Basics Section */}
      <section id="dsp-basics" className="border-b border-zinc-800">
        <div className="max-w-4xl mx-auto px-6 py-16">
          <h2 className="text-3xl font-bold mb-8">DSP Basics</h2>

          <div className="space-y-8">
            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Audio Fundamentals</h3>

              <div className="space-y-6">
                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Sample Rate</h4>
                  <ul className="list-disc list-inside space-y-2 text-zinc-400 ml-4">
                    <li>Common rates: 44.1kHz (CD), 48kHz (video), 96kHz (hi-res)</li>
                    <li>Sample rate determines maximum frequency (Nyquist: sample_rate / 2)</li>
                    <li>All our plugins should handle any sample rate dynamically</li>
                  </ul>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Bit Depth</h4>
                  <ul className="list-disc list-inside space-y-2 text-zinc-400 ml-4">
                    <li>16-bit (CD quality), 24-bit (professional), 32-bit float (DAW internal)</li>
                    <li>JUCE handles this for us, we work with floats</li>
                  </ul>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Buffers</h4>
                  <ul className="list-disc list-inside space-y-2 text-zinc-400 ml-4">
                    <li>Audio processed in blocks (typically 64-512 samples)</li>
                    <li>JUCE gives us AudioBuffer&lt;float&gt; with channels</li>
                    <li>We process samples in a loop</li>
                  </ul>
                </div>
              </div>
            </div>

            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Basic DSP Operations</h3>

              <div className="space-y-6">
                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Gain (Volume Control)</h4>
                  <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                    <code className="text-orange-400">{`// Simplest effect: multiply each sample
for (int sample = 0; sample < numSamples; ++sample) {
    buffer[sample] = buffer[sample] * gainLinear;
}`}</code>
                  </pre>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Decibels to Linear</h4>
                  <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                    <code className="text-orange-400">{`// Convert dB to linear gain
float linearGain = std::pow(10.0f, gainDb / 20.0f);

// Convert linear to dB
float gainDb = 20.0f * std::log10(linearGain);`}</code>
                  </pre>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Panning (Stereo)</h4>
                  <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                    <code className="text-orange-400">{`// Constant power pan (-45dB to +45dB)
float leftGain = std::cos(panAngle);
float rightGain = std::sin(panAngle);`}</code>
                  </pre>
                </div>
              </div>
            </div>

            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Common Effects</h3>

              <div className="space-y-6">
                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Delay</h4>
                  <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                    <code className="text-orange-400">{`// Circular buffer
delayBuffer[(writePos++) % bufferSize] = input;
output = delayBuffer[(writePos - delaySamples) % bufferSize];`}</code>
                  </pre>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Simple Filter (One-pole lowpass)</h4>
                  <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                    <code className="text-orange-400">{`// Exponential moving average
output = output + cutoff * (input - output);`}</code>
                  </pre>
                </div>
              </div>
            </div>

            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Common Pitfalls</h3>
              <ol className="list-decimal list-inside space-y-2 text-zinc-400 ml-4">
                <li><strong className="text-zinc-300">Not initializing variables</strong> - Causes pops and noise</li>
                <li><strong className="text-zinc-300">Ignoring sample rate</strong> - Delay times, cutoffs must scale</li>
                <li><strong className="text-zinc-300">Forgetting to smooth parameters</strong> - Causes clicks</li>
                <li><strong className="text-zinc-300">Buffer overruns</strong> - Check array bounds</li>
                <li><strong className="text-zinc-300">Denormal numbers</strong> - Can kill CPU, flush to zero</li>
              </ol>
            </div>
          </div>
        </div>
      </section>

      {/* JUCE Guide Section */}
      <section id="juce-guide" className="border-b border-zinc-800">
        <div className="max-w-4xl mx-auto px-6 py-16">
          <h2 className="text-3xl font-bold mb-8">JUCE Framework Guide</h2>

          <div className="space-y-8">
            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Project Structure</h3>
              <p className="text-zinc-400 mb-4">Every JUCE plugin has:</p>
              <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                <code className="text-orange-400">{`PluginName/
├── CMakeLists.txt          # Build configuration
├── Source/
│   ├── PluginProcessor.h   # Audio processing logic
│   ├── PluginProcessor.cpp
│   ├── PluginEditor.h      # GUI
│   └── PluginEditor.cpp`}</code>
              </pre>
            </div>

            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Audio Processor Pattern</h3>
              <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                <code className="text-orange-400">{`class MyProcessor : public AudioProcessor {
public:
    MyProcessor() {
        // Initialize parameters
        addParameter(gain = new AudioParameterFloat(
            "gain", "Gain", 0.0f, 1.0f, 0.5f));
    }

    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi) override {
        // Get parameter value
        float gainValue = gain->get();

        // Process each channel
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            float* channelData = buffer.getWritePointer(channel);

            // Process each sample
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
                channelData[sample] *= gainValue;
            }
        }
    }

private:
    AudioParameterFloat* gain;
};`}</code>
              </pre>
            </div>

            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Parameters</h3>

              <div className="space-y-6">
                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">AudioParameterFloat</h4>
                  <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                    <code className="text-orange-400">{`addParameter(new AudioParameterFloat(
    "paramID",              // Unique ID
    "Parameter Name",       // Display name
    0.0f,                   // Min
    1.0f,                   // Max
    0.5f));                 // Default`}</code>
                  </pre>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">AudioParameterChoice (dropdown)</h4>
                  <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                    <code className="text-orange-400">{`addParameter(new AudioParameterChoice(
    "mode",
    "Mode",
    StringArray{"Option 1", "Option 2", "Option 3"},
    0));  // Default index`}</code>
                  </pre>
                </div>
              </div>
            </div>

            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Common JUCE Utilities</h3>

              <div className="space-y-6">
                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">dB Conversion</h4>
                  <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                    <code className="text-orange-400">{`#include <juce_audio_basics/juce_audio_basics.h>

float linear = Decibels::decibelsToGain(dbValue);
float db = Decibels::gainToDecibels(linearValue);`}</code>
                  </pre>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Smoothing</h4>
                  <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                    <code className="text-orange-400">{`#include <juce_dsp/juce_dsp.h>

SmoothedValue<float> smoothedGain;
smoothedGain.reset(sampleRate, 0.05); // 50ms ramp
smoothedGain.setTargetValue(newGain);

// In processBlock:
float currentGain = smoothedGain.getNextValue();`}</code>
                  </pre>
                </div>

                <div>
                  <h4 className="text-lg font-medium mb-2 text-zinc-300">Thread Safety</h4>
                  <pre className="bg-zinc-900 p-4 rounded-lg overflow-x-auto text-sm">
                    <code className="text-orange-400">{`// Parameters are thread-safe
// But for custom variables:
CriticalSection lock;

{
    const ScopedLock sl(lock);
    // Access shared data
}`}</code>
                  </pre>
                </div>
              </div>
            </div>

            <div>
              <h3 className="text-2xl font-semibold mb-4 text-orange-500">Debugging Tips</h3>
              <ol className="list-decimal list-inside space-y-2 text-zinc-400 ml-4">
                <li>Use <code className="text-sm bg-zinc-900 px-2 py-1 rounded text-orange-500">DBG(&quot;message&quot;)</code> for console output</li>
                <li>Enable assertions in Debug builds</li>
                <li>Use Xcode/Visual Studio debugger</li>
                <li>Test in actual DAW, not just standalone</li>
                <li>Check CPU usage in DAW&apos;s performance meter</li>
              </ol>
            </div>
          </div>
        </div>
      </section>

      {/* Resources Section */}
      <section className="border-b border-zinc-800">
        <div className="max-w-4xl mx-auto px-6 py-16">
          <h2 className="text-3xl font-bold mb-8">Additional Resources</h2>

          <div className="grid md:grid-cols-2 gap-6">
            <a
              href="https://juce.com/learn/tutorials"
              target="_blank"
              rel="noopener noreferrer"
              className="p-6 border border-zinc-800 rounded-lg hover:border-orange-500/50 transition-colors"
            >
              <h3 className="text-xl font-semibold mb-2 text-orange-500">JUCE Tutorials</h3>
              <p className="text-zinc-400">Official JUCE framework tutorials and documentation</p>
            </a>

            <a
              href="https://www.youtube.com/c/TheAudioProgrammer"
              target="_blank"
              rel="noopener noreferrer"
              className="p-6 border border-zinc-800 rounded-lg hover:border-orange-500/50 transition-colors"
            >
              <h3 className="text-xl font-semibold mb-2 text-orange-500">The Audio Programmer</h3>
              <p className="text-zinc-400">YouTube channel with audio plugin development tutorials</p>
            </a>

            <a
              href="https://www.musicdsp.org/"
              target="_blank"
              rel="noopener noreferrer"
              className="p-6 border border-zinc-800 rounded-lg hover:border-orange-500/50 transition-colors"
            >
              <h3 className="text-xl font-semibold mb-2 text-orange-500">musicdsp.org</h3>
              <p className="text-zinc-400">Collection of DSP algorithms and audio processing code</p>
            </a>

            <a
              href="https://forum.juce.com/"
              target="_blank"
              rel="noopener noreferrer"
              className="p-6 border border-zinc-800 rounded-lg hover:border-orange-500/50 transition-colors"
            >
              <h3 className="text-xl font-semibold mb-2 text-orange-500">JUCE Forum</h3>
              <p className="text-zinc-400">Community forum for JUCE developers</p>
            </a>
          </div>

          <div className="mt-8 p-6 border border-zinc-800 rounded-lg bg-zinc-900/50">
            <h3 className="text-xl font-semibold mb-2 text-orange-500">Recommended Book</h3>
            <p className="text-zinc-400">
              <strong className="text-white">&quot;Designing Audio Effect Plugins in C++&quot;</strong> by Will Pirkle -
              Comprehensive guide to audio plugin development with practical DSP implementations
            </p>
          </div>
        </div>
      </section>
    </div>
  );
}
