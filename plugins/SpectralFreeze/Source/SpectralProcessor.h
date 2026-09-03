#pragma once

#include <juce_dsp/juce_dsp.h>
#include <vector>

/**
 * SpectralProcessor
 *
 * Handles FFT analysis, spectral freezing, and manipulation.
 * Uses overlap-add processing for high-quality spectral effects.
 */
class SpectralProcessor
{
public:
    SpectralProcessor();
    ~SpectralProcessor() = default;

    //==============================================================================
    // Configuration
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void reset();

    //==============================================================================
    // Parameters
    void setFreeze(bool shouldFreeze);
    void setBlurAmount(float amount);      // 0-1: spectral blur/smoothing
    void setLowCutFreq(float freq);        // Low-cut filter frequency
    void setHighCutFreq(float freq);       // High-cut filter frequency
    void setDryWet(float mix);             // 0-1: dry/wet mix

    // Phase 2: Advanced Effects
    void setStretchAmount(float amount);   // 0-1: spectral time-stretch
    void setShiftAmount(float amount);     // -1 to 1: frequency shift (semitones)
    void setMorphAmount(float amount);     // 0-1: morph to frozen spectrum
    void setGateThreshold(float threshold); // 0-1: spectral gate threshold
    void setRandomizeAmount(float amount);  // 0-1: phase randomization

    bool isFrozen() const { return frozen; }
    float getBlurAmount() const { return blurAmount; }
    float getLowCutFreq() const { return lowCutFreq; }
    float getHighCutFreq() const { return highCutFreq; }
    float getDryWet() const { return dryWet; }
    float getStretchAmount() const { return stretchAmount; }
    float getShiftAmount() const { return shiftAmount; }
    float getMorphAmount() const { return morphAmount; }
    float getGateThreshold() const { return gateThreshold; }
    float getRandomizeAmount() const { return randomizeAmount; }

    //==============================================================================
    // Processing
    void processBlock(juce::AudioBuffer<float>& buffer);

    //==============================================================================
    // Visualization Data Access
    const std::vector<float>& getCurrentMagnitudeSpectrum() const { return currentMagnitude; }
    const std::vector<float>& getFrozenMagnitudeSpectrum() const { return frozenMagnitude; }
    int getFFTSize() const { return FFT_SIZE; }
    bool isProcessorPrepared() const { return isPrepared; }

private:
    //==============================================================================
    // FFT Processing
    void processFFT(float* channelData, int numSamples, int channel);
    void applySpectralEffects(std::vector<float>& magnitude, std::vector<float>& phase);
    void freezeSpectrum(const std::vector<float>& magnitude, const std::vector<float>& phase);

    //==============================================================================
    // Spectral Effects
    void applySpectralBlur(std::vector<float>& magnitude);
    void applyFrequencyFilter(std::vector<float>& magnitude);

    // Phase 2: Advanced Effects
    void applySpectralStretch(std::vector<float>& magnitude);
    void applySpectralShift(std::vector<float>& magnitude);
    void applySpectralMorph(std::vector<float>& magnitude, const std::vector<float>& phase);
    void applySpectralGate(std::vector<float>& magnitude);
    void applySpectralRandomize(std::vector<float>& phase);

    //==============================================================================
    // FFT Setup
    static constexpr int FFT_ORDER = 11;           // 2^11 = 2048 samples
    static constexpr int FFT_SIZE = 1 << FFT_ORDER; // 2048
    static constexpr int HOP_SIZE = FFT_SIZE / 4;   // 75% overlap

    juce::dsp::FFT fft { FFT_ORDER };
    std::vector<float> windowTable;

    //==============================================================================
    // Buffers
    std::vector<std::vector<float>> fftBuffer;     // Per-channel FFT buffer
    std::vector<std::vector<float>> inputFifo;     // Input circular buffer
    std::vector<std::vector<float>> outputFifo;    // Output circular buffer
    std::vector<int> inputFifoIndex;               // Current write position
    std::vector<int> outputFifoIndex;              // Current read position
    std::vector<int> fftCounter;                   // Samples until next FFT

    //==============================================================================
    // Frozen Spectrum
    std::vector<float> frozenMagnitude;
    std::vector<float> frozenPhase;
    bool frozen = false;
    bool isPrepared = false;  // Track if prepare() has been called

    //==============================================================================
    // Parameters
    double sampleRate = 44100.0;
    float blurAmount = 0.0f;        // Spectral blur (0-1)
    float lowCutFreq = 20.0f;       // Low-cut frequency (Hz)
    float highCutFreq = 20000.0f;   // High-cut frequency (Hz)
    float dryWet = 1.0f;            // Dry/wet mix (0-1)

    // Phase 2: Advanced Effect Parameters
    float stretchAmount = 0.0f;     // Spectral stretch (0-1)
    float shiftAmount = 0.0f;       // Frequency shift (-1 to 1, in semitones)
    float morphAmount = 0.0f;       // Morph to frozen (0-1)
    float gateThreshold = 0.0f;     // Gate threshold (0-1)
    float randomizeAmount = 0.0f;   // Phase randomization (0-1)

    // Phase 5: Smoothed parameter values (for click-free parameter changes)
    float smoothedBlur = 0.0f;
    float smoothedLowCut = 20.0f;
    float smoothedHighCut = 20000.0f;
    float smoothedDryWet = 1.0f;
    float smoothedStretch = 0.0f;
    float smoothedShift = 0.0f;
    float smoothedMorph = 0.0f;
    float smoothedGate = 0.0f;
    float smoothedRandomize = 0.0f;
    static constexpr float SMOOTHING_COEFF = 0.99f;  // Higher = slower smoothing

    // Morph state
    std::vector<float> currentMagnitude;
    std::vector<float> currentPhase;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralProcessor)
};
