#pragma once

/**
 * Multi-Mode State-Variable Filter
 *
 * Provides 4 filter types from a single topology:
 * - Low-pass (LP)
 * - High-pass (HP)
 * - Band-pass (BP)
 * - Notch (LP + HP)
 *
 * State-variable filters allow smooth transitions between filter types
 * and provide all outputs simultaneously.
 */
class MultiModeFilter
{
public:
    enum class FilterType
    {
        LowPass = 0,
        HighPass = 1,
        BandPass = 2,
        Notch = 3
    };

    MultiModeFilter();

    /**
     * Set filter parameters.
     *
     * @param cutoffHz Cutoff frequency in Hz (20-20000)
     * @param resonance Resonance/Q factor (0.5-20.0)
     * @param sampleRate Current sample rate
     */
    void setParameters(float cutoffHz, float resonance, double sampleRate);

    /**
     * Set filter type.
     *
     * @param type Filter type (LP, HP, BP, or Notch)
     */
    void setFilterType(FilterType type);

    /**
     * Process one sample through the filter.
     *
     * @param input Input sample
     * @return Filtered output sample
     */
    float processSample(float input);

    /**
     * Reset filter state (clear internal buffers).
     * Call this when starting a new note or stopping playback.
     */
    void reset();

private:
    // Filter type
    FilterType filterType = FilterType::LowPass;

    // State variables (integrators)
    float lp = 0.0f;  // Low-pass output
    float bp = 0.0f;  // Band-pass output
    float hp = 0.0f;  // High-pass output

    // Filter coefficients
    float cutoffCoeff = 0.0f;  // Cutoff coefficient (2 * sin(π * f / fs))
    float resonanceCoeff = 0.0f;  // Resonance coefficient (1 / Q)

    // Last parameters (for change detection)
    float lastCutoff = -1.0f;
    float lastResonance = -1.0f;
    double lastSampleRate = 0.0;

    /**
     * Update filter coefficients when parameters change.
     */
    void updateCoefficients(float cutoffHz, float resonance, double sampleRate);
};
