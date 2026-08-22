/**
 * Waveshaping DSP Tests
 *
 * Tests for waveshaping distortion algorithms:
 * - Soft clip saturation curve
 * - Hard clip digital clipping
 * - Tanh saturation
 * - Cubic waveshaping
 * - Asymmetric waveshaping
 * - Output clamping and stability
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include "../../shared/dsp/WaveShaping.h"

class WaveShapingTests : public juce::UnitTest
{
public:
    WaveShapingTests()
        : juce::UnitTest("Waveshaping Algorithms", "DSP")
    {
    }

    void runTest() override
    {
        testSoftClip();
        testHardClip();
        testTanhSaturation();
        testCubic();
        testAsymmetric();
        testOutputClamping();
        testUnityForSmallSignals();
        testShapeInterface();
    }

private:
    void testSoftClip()
    {
        beginTest("Soft clip saturation");

        // Test unity pass for small signals
        expectWithinAbsoluteError(AudioForge::DSP::WaveShaping::softClip(0.5f), 0.5f, 0.01f,
                                "Small signal should pass through mostly unchanged");

        // Test saturation approaching ±1 (soft clip saturates around ±0.75 for input of 3.0)
        float saturatedPos = AudioForge::DSP::WaveShaping::softClip(3.0f);
        expect(saturatedPos > 0.7f && saturatedPos < 1.0f,
             "Large positive signal should saturate (0.7-1.0)");

        float saturatedNeg = AudioForge::DSP::WaveShaping::softClip(-3.0f);
        expect(saturatedNeg < -0.7f && saturatedNeg > -1.0f,
             "Large negative signal should saturate (-1.0 to -0.7)");

        // Test symmetry
        expectWithinAbsoluteError(
            AudioForge::DSP::WaveShaping::softClip(1.5f),
            -AudioForge::DSP::WaveShaping::softClip(-1.5f),
            0.001f,
            "Soft clip should be symmetric");
    }

    void testHardClip()
    {
        beginTest("Hard clip digital clipping");

        // Test unity pass for signals within ±1
        expectEquals(AudioForge::DSP::WaveShaping::hardClip(0.5f), 0.5f,
                    "Signal within range should pass through unchanged");

        // Test hard clipping at ±1
        expectEquals(AudioForge::DSP::WaveShaping::hardClip(2.0f), 1.0f,
                    "Signal above +1 should clip to +1");
        expectEquals(AudioForge::DSP::WaveShaping::hardClip(-2.0f), -1.0f,
                    "Signal below -1 should clip to -1");

        // Test exact threshold
        expectEquals(AudioForge::DSP::WaveShaping::hardClip(1.0f), 1.0f,
                    "Signal at +1 should remain at +1");
        expectEquals(AudioForge::DSP::WaveShaping::hardClip(-1.0f), -1.0f,
                    "Signal at -1 should remain at -1");
    }

    void testTanhSaturation()
    {
        beginTest("Tanh saturation curve");

        // Test zero crossing
        expectWithinAbsoluteError(AudioForge::DSP::WaveShaping::tanhSaturation(0.0f), 0.0f, 0.0001f,
                                "Tanh at zero should be zero");

        // Test small signal linearity
        float input = 0.1f;
        float output = AudioForge::DSP::WaveShaping::tanhSaturation(input);
        expectWithinAbsoluteError(output, input, 0.01f,
                                "Tanh should be approximately linear for small signals");

        // Test saturation
        float saturated = AudioForge::DSP::WaveShaping::tanhSaturation(5.0f);
        expect(saturated > 0.9f && saturated < 1.0f,
             "Large signal should saturate approaching +1");

        // Test symmetry
        expectWithinAbsoluteError(
            AudioForge::DSP::WaveShaping::tanhSaturation(2.0f),
            -AudioForge::DSP::WaveShaping::tanhSaturation(-2.0f),
            0.001f,
            "Tanh should be symmetric");
    }

    void testCubic()
    {
        beginTest("Cubic waveshaping");

        // Test zero crossing
        expectEquals(AudioForge::DSP::WaveShaping::cubic(0.0f), 0.0f,
                    "Cubic at zero should be zero");

        // Test clamping at ±1
        float output = AudioForge::DSP::WaveShaping::cubic(1.5f);
        expect(output >= -1.0f && output <= 1.0f,
             "Cubic output should be clamped to ±1");

        // Test symmetry (cubic is odd function)
        expectWithinAbsoluteError(
            AudioForge::DSP::WaveShaping::cubic(0.8f),
            -AudioForge::DSP::WaveShaping::cubic(-0.8f),
            0.001f,
            "Cubic should be symmetric");

        // Test distortion characteristic
        float small = AudioForge::DSP::WaveShaping::cubic(0.3f);
        expect(small > 0.0f && small < 0.5f,
             "Cubic should produce harmonic distortion");
    }

    void testAsymmetric()
    {
        beginTest("Asymmetric waveshaping");

        // Test zero crossing
        expectWithinAbsoluteError(AudioForge::DSP::WaveShaping::asymmetric(0.0f), 0.0f, 0.0001f,
                                "Asymmetric at zero should be near zero");

        // Test asymmetry (positive and negative should differ)
        float positive = AudioForge::DSP::WaveShaping::asymmetric(1.0f);
        float negative = AudioForge::DSP::WaveShaping::asymmetric(-1.0f);
        expect(std::abs(positive) != std::abs(negative),
             "Asymmetric should produce different curves for +/-");

        // Test positive harder clipping (85% output)
        float posOutput = AudioForge::DSP::WaveShaping::asymmetric(3.0f);
        expectWithinAbsoluteError(posOutput, 0.85f, 0.05f,
                                "Positive saturation should approach 0.85");

        // Test negative softer clipping (95% output)
        float negOutput = AudioForge::DSP::WaveShaping::asymmetric(-3.0f);
        expectWithinAbsoluteError(negOutput, -0.95f, 0.05f,
                                "Negative saturation should approach -0.95");
    }

    void testOutputClamping()
    {
        beginTest("Output clamping and stability");

        // Test all algorithms with extreme inputs
        float extremeInput = 10.0f;

        float softClip = AudioForge::DSP::WaveShaping::softClip(extremeInput);
        expect(std::isfinite(softClip) && std::abs(softClip) <= 1.5f,
             "Soft clip should produce finite output");

        float hardClip = AudioForge::DSP::WaveShaping::hardClip(extremeInput);
        expect(std::abs(hardClip) <= 1.0f,
             "Hard clip should be bounded at ±1");

        float tanh = AudioForge::DSP::WaveShaping::tanhSaturation(extremeInput);
        expect(std::abs(tanh) <= 1.0f,
             "Tanh should be bounded at or below 1.0");

        float cubic = AudioForge::DSP::WaveShaping::cubic(extremeInput);
        expect(std::abs(cubic) <= 1.0f,
             "Cubic should be clamped at ±1");

        float asymmetric = AudioForge::DSP::WaveShaping::asymmetric(extremeInput);
        expect(std::isfinite(asymmetric) && std::abs(asymmetric) <= 1.0f,
             "Asymmetric should produce bounded output");
    }

    void testUnityForSmallSignals()
    {
        beginTest("Unity pass for small signals");

        // All algorithms should pass very small signals with minimal change
        float input = 0.01f;

        float softClip = AudioForge::DSP::WaveShaping::softClip(input);
        expectWithinAbsoluteError(softClip, input, 0.001f,
                                "Soft clip should preserve tiny signals");

        float hardClip = AudioForge::DSP::WaveShaping::hardClip(input);
        expectEquals(hardClip, input,
                    "Hard clip should preserve signals within range");

        float tanh = AudioForge::DSP::WaveShaping::tanhSaturation(input);
        expectWithinAbsoluteError(tanh, input, 0.001f,
                                "Tanh should be linear for tiny signals");

        float cubic = AudioForge::DSP::WaveShaping::cubic(input);
        expectWithinAbsoluteError(cubic, input, 0.001f,
                                "Cubic should preserve tiny signals");
    }

    void testShapeInterface()
    {
        beginTest("Unified shape() interface");

        float input = 1.5f;

        // Test all shape modes
        float softClip = AudioForge::DSP::WaveShaping::shape(input, 0);
        expectWithinAbsoluteError(softClip, AudioForge::DSP::WaveShaping::softClip(input), 0.0001f,
                                "Mode 0 should call soft clip");

        float hardClip = AudioForge::DSP::WaveShaping::shape(input, 1);
        expectEquals(hardClip, AudioForge::DSP::WaveShaping::hardClip(input),
                    "Mode 1 should call hard clip");

        float tanh = AudioForge::DSP::WaveShaping::shape(input, 2);
        expectWithinAbsoluteError(tanh, AudioForge::DSP::WaveShaping::tanhSaturation(input), 0.0001f,
                                "Mode 2 should call tanh");

        float cubic = AudioForge::DSP::WaveShaping::shape(input, 3);
        expectWithinAbsoluteError(cubic, AudioForge::DSP::WaveShaping::cubic(input), 0.0001f,
                                "Mode 3 should call cubic");

        float asymmetric = AudioForge::DSP::WaveShaping::shape(input, 4);
        expectWithinAbsoluteError(asymmetric, AudioForge::DSP::WaveShaping::asymmetric(input), 0.0001f,
                                "Mode 4 should call asymmetric");

        // Test invalid mode (should pass through)
        float passthrough = AudioForge::DSP::WaveShaping::shape(input, 99);
        expectEquals(passthrough, input,
                    "Invalid mode should pass through unchanged");
    }
};

static WaveShapingTests waveShapingTests;
