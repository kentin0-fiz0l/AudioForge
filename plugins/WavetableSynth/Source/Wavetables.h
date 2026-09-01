#pragma once

#include <array>
#include <cmath>
#include <vector>

namespace Wavetables
{

constexpr int WAVETABLE_SIZE = 2048;
constexpr int NUM_WAVETABLES = 32;

/**
 * Generate basic waveforms and complex wavetables
 */
class WavetableGenerator
{
public:
    using Wavetable = std::array<float, WAVETABLE_SIZE>;

    static std::vector<Wavetable> generateAllWavetables()
    {
        std::vector<Wavetable> tables;
        tables.reserve(NUM_WAVETABLES);

        // Basic waveforms (0-3)
        tables.push_back(generateSine());
        tables.push_back(generateSaw());
        tables.push_back(generateSquare());
        tables.push_back(generateTriangle());

        // Harmonic series (4-11)
        tables.push_back(generateOddHarmonics());
        tables.push_back(generateEvenHarmonics());
        tables.push_back(generateHarmonicSeries(3));
        tables.push_back(generateHarmonicSeries(5));
        tables.push_back(generateHarmonicSeries(7));
        tables.push_back(generatePulse(0.25f));
        tables.push_back(generatePulse(0.1f));
        tables.push_back(generateDoubleSquare());

        // Analog-style (12-19)
        tables.push_back(generateAnalogSaw());
        tables.push_back(generateAnalogSquare());
        tables.push_back(generateSuperSaw());
        tables.push_back(generatePWM());
        tables.push_back(generateRamp());
        tables.push_back(generateTriangleSaw());
        tables.push_back(generateFatSaw());
        tables.push_back(generateDetunedSaw());

        // Complex/Digital (20-27)
        tables.push_back(generateDigital1());
        tables.push_back(generateDigital2());
        tables.push_back(generateDigital3());
        tables.push_back(generateFormant());
        tables.push_back(generateVowel());
        tables.push_back(generateBell());
        tables.push_back(generateGlass());
        tables.push_back(generateOrgan());

        // Special (28-31)
        tables.push_back(generateNoise());
        tables.push_back(generateChip());
        tables.push_back(generateMetallic());
        tables.push_back(generatePad());

        return tables;
    }

private:
    static Wavetable generateSine()
    {
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            float phase = (2.0f * static_cast<float>(M_PI) * i) / WAVETABLE_SIZE;
            table[i] = std::sin(phase);
        }
        return table;
    }

    static Wavetable generateSaw()
    {
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            table[i] = 2.0f * (static_cast<float>(i) / WAVETABLE_SIZE) - 1.0f;
        }
        return table;
    }

    static Wavetable generateSquare()
    {
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            table[i] = (i < WAVETABLE_SIZE / 2) ? 1.0f : -1.0f;
        }
        return table;
    }

    static Wavetable generateTriangle()
    {
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            if (i < WAVETABLE_SIZE / 2)
                table[i] = -1.0f + (4.0f * i) / WAVETABLE_SIZE;
            else
                table[i] = 3.0f - (4.0f * i) / WAVETABLE_SIZE;
        }
        return table;
    }

    static Wavetable generateOddHarmonics()
    {
        Wavetable table;
        table.fill(0.0f);
        for (int harmonic = 1; harmonic <= 9; harmonic += 2)
        {
            float amplitude = 1.0f / harmonic;
            for (int i = 0; i < WAVETABLE_SIZE; ++i)
            {
                float phase = (2.0f * static_cast<float>(M_PI) * i * harmonic) / WAVETABLE_SIZE;
                table[i] += amplitude * std::sin(phase);
            }
        }
        normalize(table);
        return table;
    }

    static Wavetable generateEvenHarmonics()
    {
        Wavetable table;
        table.fill(0.0f);
        for (int harmonic = 2; harmonic <= 10; harmonic += 2)
        {
            float amplitude = 1.0f / harmonic;
            for (int i = 0; i < WAVETABLE_SIZE; ++i)
            {
                float phase = (2.0f * static_cast<float>(M_PI) * i * harmonic) / WAVETABLE_SIZE;
                table[i] += amplitude * std::sin(phase);
            }
        }
        normalize(table);
        return table;
    }

    static Wavetable generateHarmonicSeries(int maxHarmonic)
    {
        Wavetable table;
        table.fill(0.0f);
        for (int harmonic = 1; harmonic <= maxHarmonic; ++harmonic)
        {
            float amplitude = 1.0f / harmonic;
            for (int i = 0; i < WAVETABLE_SIZE; ++i)
            {
                float phase = (2.0f * static_cast<float>(M_PI) * i * harmonic) / WAVETABLE_SIZE;
                table[i] += amplitude * std::sin(phase);
            }
        }
        normalize(table);
        return table;
    }

    static Wavetable generatePulse(float width)
    {
        Wavetable table;
        int threshold = static_cast<int>(WAVETABLE_SIZE * width);
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            table[i] = (i < threshold) ? 1.0f : -1.0f;
        }
        return table;
    }

    static Wavetable generateDoubleSquare()
    {
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            float pos = static_cast<float>(i) / WAVETABLE_SIZE;
            table[i] = (std::fmod(pos * 4.0f, 2.0f) < 1.0f) ? 1.0f : -1.0f;
        }
        return table;
    }

    static Wavetable generateAnalogSaw()
    {
        auto table = generateSaw();
        // Add slight harmonics for analog character
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            float phase = (2.0f * static_cast<float>(M_PI) * i) / WAVETABLE_SIZE;
            table[i] += 0.1f * std::sin(phase * 2.0f);
            table[i] += 0.05f * std::sin(phase * 3.0f);
        }
        normalize(table);
        return table;
    }

    static Wavetable generateAnalogSquare()
    {
        auto table = generateSquare();
        // Smooth transitions for analog character
        int transitionLen = 20;
        for (int i = 0; i < transitionLen; ++i)
        {
            float t = static_cast<float>(i) / transitionLen;
            table[i] = -1.0f + 2.0f * t;
            table[WAVETABLE_SIZE / 2 + i] = 1.0f - 2.0f * t;
        }
        return table;
    }

    static Wavetable generateSuperSaw()
    {
        Wavetable table;
        table.fill(0.0f);
        // Combine 7 detuned saws
        for (int voice = 0; voice < 7; ++voice)
        {
            float detune = (voice - 3) * 0.02f;
            for (int i = 0; i < WAVETABLE_SIZE; ++i)
            {
                float pos = std::fmod(i / static_cast<float>(WAVETABLE_SIZE) + detune, 1.0f);
                table[i] += 2.0f * pos - 1.0f;
            }
        }
        normalize(table);
        return table;
    }

    static Wavetable generatePWM()
    {
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            float width = 0.5f + 0.3f * std::sin((2.0f * static_cast<float>(M_PI) * i) / WAVETABLE_SIZE);
            table[i] = (i < WAVETABLE_SIZE * width) ? 1.0f : -1.0f;
        }
        return table;
    }

    static Wavetable generateRamp()
    {
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            table[i] = 1.0f - 2.0f * (static_cast<float>(i) / WAVETABLE_SIZE);
        }
        return table;
    }

    static Wavetable generateTriangleSaw()
    {
        auto triangle = generateTriangle();
        auto saw = generateSaw();
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            table[i] = 0.6f * triangle[i] + 0.4f * saw[i];
        }
        return table;
    }

    static Wavetable generateFatSaw()
    {
        Wavetable table;
        table.fill(0.0f);
        for (int voice = 0; voice < 3; ++voice)
        {
            float detune = (voice - 1) * 0.01f;
            for (int i = 0; i < WAVETABLE_SIZE; ++i)
            {
                float pos = std::fmod(i / static_cast<float>(WAVETABLE_SIZE) + detune, 1.0f);
                table[i] += 2.0f * pos - 1.0f;
            }
        }
        normalize(table);
        return table;
    }

    static Wavetable generateDetunedSaw()
    {
        Wavetable table;
        table.fill(0.0f);
        for (int voice = 0; voice < 2; ++voice)
        {
            float detune = voice * 0.005f;
            for (int i = 0; i < WAVETABLE_SIZE; ++i)
            {
                float pos = std::fmod(i / static_cast<float>(WAVETABLE_SIZE) + detune, 1.0f);
                table[i] += 2.0f * pos - 1.0f;
            }
        }
        normalize(table);
        return table;
    }

    static Wavetable generateDigital1()
    {
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            float phase = (2.0f * static_cast<float>(M_PI) * i) / WAVETABLE_SIZE;
            table[i] = std::sin(phase) + 0.5f * std::sin(phase * 7.0f) + 0.3f * std::sin(phase * 13.0f);
        }
        normalize(table);
        return table;
    }

    static Wavetable generateDigital2()
    {
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            float phase = (2.0f * static_cast<float>(M_PI) * i) / WAVETABLE_SIZE;
            table[i] = std::sin(phase * 2.0f) * std::sin(phase * 5.0f);
        }
        normalize(table);
        return table;
    }

    static Wavetable generateDigital3()
    {
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            float phase = (2.0f * static_cast<float>(M_PI) * i) / WAVETABLE_SIZE;
            table[i] = std::tanh(3.0f * std::sin(phase) + std::sin(phase * 3.0f));
        }
        normalize(table);
        return table;
    }

    static Wavetable generateFormant()
    {
        Wavetable table;
        table.fill(0.0f);
        int formants[] = {800, 1150, 2900, 3900};
        for (int formant : formants)
        {
            for (int i = 0; i < WAVETABLE_SIZE; ++i)
            {
                float phase = (2.0f * static_cast<float>(M_PI) * i * formant) / (WAVETABLE_SIZE * 440);
                table[i] += std::sin(phase);
            }
        }
        normalize(table);
        return table;
    }

    static Wavetable generateVowel()
    {
        return generateFormant(); // Simplified vowel using formant
    }

    static Wavetable generateBell()
    {
        Wavetable table;
        table.fill(0.0f);
        float ratios[] = {1.0f, 2.76f, 5.4f, 8.93f};
        for (float ratio : ratios)
        {
            for (int i = 0; i < WAVETABLE_SIZE; ++i)
            {
                float phase = (2.0f * static_cast<float>(M_PI) * i * ratio) / WAVETABLE_SIZE;
                table[i] += std::sin(phase) / ratio;
            }
        }
        normalize(table);
        return table;
    }

    static Wavetable generateGlass()
    {
        Wavetable table;
        table.fill(0.0f);
        for (int harmonic = 1; harmonic <= 16; harmonic += 3)
        {
            float amplitude = 1.0f / (harmonic * harmonic);
            for (int i = 0; i < WAVETABLE_SIZE; ++i)
            {
                float phase = (2.0f * static_cast<float>(M_PI) * i * harmonic) / WAVETABLE_SIZE;
                table[i] += amplitude * std::sin(phase);
            }
        }
        normalize(table);
        return table;
    }

    static Wavetable generateOrgan()
    {
        Wavetable table;
        table.fill(0.0f);
        float drawbars[] = {1.0f, 0.8f, 0.6f, 0.4f, 0.3f, 0.2f, 0.15f, 0.1f, 0.08f};
        for (int h = 0; h < 9; ++h)
        {
            for (int i = 0; i < WAVETABLE_SIZE; ++i)
            {
                float phase = (2.0f * static_cast<float>(M_PI) * i * (h + 1)) / WAVETABLE_SIZE;
                table[i] += drawbars[h] * std::sin(phase);
            }
        }
        normalize(table);
        return table;
    }

    static Wavetable generateNoise()
    {
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            table[i] = 2.0f * (std::rand() / static_cast<float>(RAND_MAX)) - 1.0f;
        }
        return table;
    }

    static Wavetable generateChip()
    {
        Wavetable table;
        for (int i = 0; i < WAVETABLE_SIZE; ++i)
        {
            float phase = (2.0f * static_cast<float>(M_PI) * i) / WAVETABLE_SIZE;
            table[i] = std::sin(phase) > 0.0f ? 0.7f : -0.7f;
            if (i % 128 < 64)
                table[i] *= 1.2f;
        }
        normalize(table);
        return table;
    }

    static Wavetable generateMetallic()
    {
        Wavetable table;
        table.fill(0.0f);
        float ratios[] = {1.0f, 1.41f, 1.73f, 2.24f, 2.65f};
        for (float ratio : ratios)
        {
            for (int i = 0; i < WAVETABLE_SIZE; ++i)
            {
                float phase = (2.0f * static_cast<float>(M_PI) * i * ratio) / WAVETABLE_SIZE;
                table[i] += std::sin(phase) / ratio;
            }
        }
        normalize(table);
        return table;
    }

    static Wavetable generatePad()
    {
        Wavetable table;
        table.fill(0.0f);
        for (int harmonic = 1; harmonic <= 8; ++harmonic)
        {
            float amplitude = 1.0f / (harmonic * harmonic);
            for (int i = 0; i < WAVETABLE_SIZE; ++i)
            {
                float phase = (2.0f * static_cast<float>(M_PI) * i * harmonic) / WAVETABLE_SIZE;
                table[i] += amplitude * std::sin(phase);
            }
        }
        normalize(table);
        return table;
    }

    static void normalize(Wavetable& table)
    {
        float maxVal = 0.0f;
        for (float sample : table)
        {
            maxVal = std::max(maxVal, std::abs(sample));
        }
        if (maxVal > 0.0f)
        {
            for (float& sample : table)
            {
                sample /= maxVal;
            }
        }
    }
};

} // namespace Wavetables
