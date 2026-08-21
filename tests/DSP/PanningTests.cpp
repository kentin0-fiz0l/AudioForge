/**
 * Panning DSP Tests
 *
 * Tests for stereo panning algorithms:
 * - Constant-power panning law
 * - Pan positions (left, center, right)
 * - Balance mode
 * - Stereo width control (M/S processing)
 * - Energy conservation
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class PanningTests : public juce::UnitTest
{
public:
    PanningTests()
        : juce::UnitTest("Panning", "DSP")
    {
    }

    void runTest() override
    {
        testConstantPowerPanning();
        testPanPositions();
        testBalanceMode();
        testStereoWidth();
        testEnergyConservation();
    }

private:
    void testConstantPowerPanning()
    {
        beginTest("Constant-power panning algorithm");

        // Test center position (pan = 0.0)
        float pan = 0.0f;
        float angle = (pan + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
        float leftGain = std::cos(angle);
        float rightGain = std::sin(angle);

        // At center, both channels should be at -3 dB (~0.707)
        expectWithinAbsoluteError(leftGain, 0.707f, 0.01f, "Center left gain incorrect");
        expectWithinAbsoluteError(rightGain, 0.707f, 0.01f, "Center right gain incorrect");

        // Test hard left (pan = -1.0)
        pan = -1.0f;
        angle = (pan + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
        leftGain = std::cos(angle);
        rightGain = std::sin(angle);

        expectWithinAbsoluteError(leftGain, 1.0f, 0.01f, "Hard left should be full left");
        expectWithinAbsoluteError(rightGain, 0.0f, 0.01f, "Hard left should be silent right");

        // Test hard right (pan = +1.0)
        pan = 1.0f;
        angle = (pan + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
        leftGain = std::cos(angle);
        rightGain = std::sin(angle);

        expectWithinAbsoluteError(leftGain, 0.0f, 0.01f, "Hard right should be silent left");
        expectWithinAbsoluteError(rightGain, 1.0f, 0.01f, "Hard right should be full right");
    }

    void testPanPositions()
    {
        beginTest("Pan position processing");

        // Create stereo buffer with mono content
        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();

        // Fill with mono signal (same in both channels)
        for (int i = 0; i < 512; ++i)
        {
            float value = std::sin(2.0f * juce::MathConstants<float>::pi * i / 512.0f);
            buffer.setSample(0, i, value); // Left
            buffer.setSample(1, i, value); // Right
        }

        // Test panning hard left
        float pan = -1.0f;
        float angle = (pan + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
        float leftGain = std::cos(angle);
        float rightGain = std::sin(angle);

        juce::AudioBuffer<float> pannedBuffer(2, 512);
        for (int i = 0; i < 512; ++i)
        {
            pannedBuffer.setSample(0, i, buffer.getSample(0, i) * leftGain);
            pannedBuffer.setSample(1, i, buffer.getSample(1, i) * rightGain);
        }

        // Right channel should be much quieter than left
        float leftPeak = pannedBuffer.getMagnitude(0, 0, 512);
        float rightPeak = pannedBuffer.getMagnitude(1, 0, 512);

        expect(leftPeak > 0.9f, "Left channel should be loud when panned left");
        expect(rightPeak < 0.1f, "Right channel should be quiet when panned left");
    }

    void testBalanceMode()
    {
        beginTest("Balance mode");

        // Create stereo signal with different content
        juce::AudioBuffer<float> buffer(2, 512);

        for (int i = 0; i < 512; ++i)
        {
            buffer.setSample(0, i, 0.8f); // Left constant
            buffer.setSample(1, i, 0.6f); // Right constant
        }

        // Test balance left (pan = -0.5)
        float pan = -0.5f;

        juce::AudioBuffer<float> balanced(2, 512);
        for (int i = 0; i < 512; ++i)
        {
            float L = buffer.getSample(0, i);
            float R = buffer.getSample(1, i);

            // Balance left = reduce right
            balanced.setSample(0, i, L);
            balanced.setSample(1, i, R * (1.0f + pan));
        }

        // Left should be unchanged, right should be attenuated
        expectEquals(balanced.getSample(0, 0), 0.8f, "Left unchanged in balance mode");
        expectWithinAbsoluteError(balanced.getSample(1, 0), 0.3f, 0.001f,
                                "Right attenuated in balance mode");

        // Test balance right (pan = +0.5)
        pan = 0.5f;
        for (int i = 0; i < 512; ++i)
        {
            float L = buffer.getSample(0, i);
            float R = buffer.getSample(1, i);

            // Balance right = reduce left
            balanced.setSample(0, i, L * (1.0f - pan));
            balanced.setSample(1, i, R);
        }

        expectWithinAbsoluteError(balanced.getSample(0, 0), 0.4f, 0.001f,
                                "Left attenuated in balance mode");
        expectEquals(balanced.getSample(1, 0), 0.6f, "Right unchanged in balance mode");
    }

    void testStereoWidth()
    {
        beginTest("Stereo width control (M/S processing)");

        // Create stereo signal
        juce::AudioBuffer<float> buffer(2, 512);

        for (int i = 0; i < 512; ++i)
        {
            buffer.setSample(0, i, 1.0f);  // Left
            buffer.setSample(1, i, -1.0f); // Right (opposite phase = max stereo)
        }

        // Test width = 0.0 (mono)
        float width = 0.0f;

        juce::AudioBuffer<float> processed(2, 512);
        for (int i = 0; i < 512; ++i)
        {
            float L = buffer.getSample(0, i);
            float R = buffer.getSample(1, i);

            // M/S processing
            float mid = (L + R) * 0.5f;
            float side = (L - R) * 0.5f;

            side *= width; // Scale side by width

            // Back to L/R
            processed.setSample(0, i, mid + side);
            processed.setSample(1, i, mid - side);
        }

        // With width = 0, both channels should be identical (mono)
        expectEquals(processed.getSample(0, 0), processed.getSample(1, 0),
                   "Width 0 should produce mono");

        // Test width = 2.0 (wide)
        width = 2.0f;
        for (int i = 0; i < 512; ++i)
        {
            float L = buffer.getSample(0, i);
            float R = buffer.getSample(1, i);

            float mid = (L + R) * 0.5f;
            float side = (L - R) * 0.5f;
            side *= width;

            processed.setSample(0, i, mid + side);
            processed.setSample(1, i, mid - side);
        }

        // With width = 2.0, stereo should be enhanced
        float diff = std::abs(processed.getSample(0, 0) - processed.getSample(1, 0));
        expect(diff > 1.5f, "Width 2.0 should enhance stereo separation");
    }

    void testEnergyConservation()
    {
        beginTest("Energy conservation in panning");

        // Create mono signal
        juce::AudioBuffer<float> monoBuffer(1, 1024);
        auto* monoData = monoBuffer.getWritePointer(0);

        for (int i = 0; i < 1024; ++i)
            monoData[i] = std::sin(2.0f * juce::MathConstants<float>::pi * i / 1024.0f);

        float monoRMS = monoBuffer.getRMSLevel(0, 0, 1024);

        // Pan the signal to various positions
        std::vector<float> panPositions = {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f};

        for (float pan : panPositions)
        {
            // Apply constant-power panning
            float angle = (pan + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
            float leftGain = std::cos(angle);
            float rightGain = std::sin(angle);

            juce::AudioBuffer<float> stereoBuffer(2, 1024);

            for (int i = 0; i < 1024; ++i)
            {
                stereoBuffer.setSample(0, i, monoData[i] * leftGain);
                stereoBuffer.setSample(1, i, monoData[i] * rightGain);
            }

            // Calculate combined energy (RMS of both channels)
            float leftRMS = stereoBuffer.getRMSLevel(0, 0, 1024);
            float rightRMS = stereoBuffer.getRMSLevel(1, 0, 1024);
            float combinedEnergy = std::sqrt(leftRMS * leftRMS + rightRMS * rightRMS);

            // Energy should be approximately conserved
            expectWithinAbsoluteError(combinedEnergy, monoRMS, 0.05f,
                                    "Energy not conserved at pan = " + juce::String(pan));
        }
    }
};

static PanningTests panningTests;
