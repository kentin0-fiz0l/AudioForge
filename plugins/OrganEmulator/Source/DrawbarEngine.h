#pragma once

#include <cmath>

/**
 * DrawbarEngine - Hammond-style additive synthesis
 *
 * Generates 9 harmonic sine waves based on drawbar positions:
 * 16'  - Sub-fundamental (0.5x)
 * 5⅓' - Sub-third (1.5x)
 * 8'   - Fundamental (1.0x)
 * 4'   - 1st harmonic (2.0x)
 * 2⅔' - 3rd harmonic (3.0x)
 * 2'   - 2nd harmonic (4.0x)
 * 1⅗' - 5th harmonic (5.0x)
 * 1⅓' - 6th harmonic (6.0x)
 * 1'   - 8th harmonic (8.0x)
 */
class DrawbarEngine
{
public:
    DrawbarEngine();

    void setFrequency(float frequency);
    void setDrawbars(const float drawbars[9]); // 0.0-1.0 for each drawbar

    float processSample(double sampleRate);
    void reset();

private:
    // 9 phase accumulators for each harmonic
    float phases[9];

    // Current frequency and drawbar levels
    float baseFrequency;
    float drawbarLevels[9];

    // Hammond organ harmonic ratios
    static constexpr float HARMONIC_RATIOS[9] = {
        0.5f,   // 16' - sub-fundamental
        1.5f,   // 5⅓' - sub-third
        1.0f,   // 8'  - fundamental
        2.0f,   // 4'  - 1 octave
        3.0f,   // 2⅔' - 1 octave + fifth
        4.0f,   // 2'  - 2 octaves
        5.0f,   // 1⅗' - 2 octaves + third
        6.0f,   // 1⅓' - 2 octaves + fifth
        8.0f    // 1'  - 3 octaves
    };
};
