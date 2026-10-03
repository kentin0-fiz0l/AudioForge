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
 *
 * Implemented as a zero-delay-feedback (trapezoidal) filter, which is stable
 * at every cutoff up to Nyquist.
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
    float ic1eq = 0.0f;  // First integrator state
    float ic2eq = 0.0f;  // Second integrator state

    // Filter coefficients
    float cutoffCoeff = 0.0f;  // Cutoff coefficient (tan(π * f / fs))
    float resonanceCoeff = 0.0f;  // Resonance coefficient (1 / Q)
    float a1 = 1.0f;  // Terms of the solved feedback loop,
    float a2 = 0.0f;  // derived from the two coefficients above
    float a3 = 0.0f;

    // Last parameters (for change detection)
    float lastCutoff = -1.0f;
    float lastResonance = -1.0f;
    double lastSampleRate = 0.0;

    /**
     * Update filter coefficients when parameters change.
     */
    void updateCoefficients(float cutoffHz, float resonance, double sampleRate);
};
