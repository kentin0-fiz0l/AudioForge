#pragma once

#include "Wavetables.h"
#include <vector>

/**
 * Wavetable Oscillator
 *
 * Oscillator that scans through multiple wavetables with linear interpolation.
 * Supports wavetable position morphing for evolving timbres.
 */
class WavetableOscillator
{
public:
    WavetableOscillator();

    /**
     * Set the base frequency for oscillation.
     */
    void setFrequency(float frequencyHz);

    /**
     * Set wavetable position (0.0 to 1.0).
     * 0.0 = first wavetable, 1.0 = last wavetable
     * Values in between interpolate between adjacent wavetables.
     */
    void setPosition(float position);

    /**
     * Reset oscillator state.
     */
    void reset();

    /**
     * Process one sample.
     *
     * @param sampleRate Current sample rate
     * @return Output sample value
     */
    float processSample(double sampleRate);

    /**
     * Check if oscillator is active.
     */
    bool isActive() const { return active; }

    /**
     * Set active state.
     */
    void setActive(bool shouldBeActive) { active = shouldBeActive; }

private:
    std::vector<Wavetables::WavetableGenerator::Wavetable> wavetables;
    float frequency = 440.0f;
    float position = 0.0f;  // Wavetable position (0.0-1.0)
    float phase = 0.0f;     // Oscillator phase (0.0-1.0)
    bool active = false;

    /**
     * Get interpolated sample from wavetables at current position and phase.
     */
    float getInterpolatedSample();

    /**
     * Linear interpolation between two values.
     */
    inline float lerp(float a, float b, float t) const
    {
        return a + t * (b - a);
    }
};
