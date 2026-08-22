/**
 * Saturation Algorithms DSP Tests
 *
 * Tests for saturation/distortion algorithms:
 * - Tape saturation (soft, even harmonics)
 * - Tube saturation (asymmetric, warm)
 * - Transistor saturation (hard clipping, odd harmonics)
 * - Transformer saturation (gentle, H2 dominant)
 * - Diode saturation (asymmetric hard clipping)
 * - Output stability and harmonic characteristics
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include "../../shared/dsp/SaturationAlgorithms.h"

class SaturationAlgorithmsTests : public juce::UnitTest
{
public:
    SaturationAlgorithmsTests()
        : juce::UnitTest("Saturation Algorithms", "DSP")
    {
    }

    void runTest() override
    {
        testTapeSaturation();
        testTubeSaturation();
        testTransistorSaturation();
        testTransformerSaturation();
        testDiodeSaturation();
        testOutputStability();
        testUnifiedInterface();
    }

private:
    void testTapeSaturation()
    {
        beginTest("Tape saturation characteristics");

        // Test unity pass for small signals
        float small = AudioForge::DSP::SaturationAlgorithms::tape(0.05f);
        expectWithinAbsoluteError(small, 0.05f, 0.01f,
                                "Small signal should pass through mostly unchanged");

        // Test soft compression
        float medium = AudioForge::DSP::SaturationAlgorithms::tape(1.0f);
        expect(medium > 0.5f && medium < 1.0f,
             "Medium signal should compress smoothly");

        // Test saturation
        float large = AudioForge::DSP::SaturationAlgorithms::tape(5.0f);
        expect(std::abs(large) < 2.0f,
             "Large signal should saturate");

        // Test no NaN or inf
        expect(std::isfinite(large),
             "Output should be finite");
    }

    void testTubeSaturation()
    {
        beginTest("Tube saturation asymmetry");

        // Test asymmetric behavior (positive != negative)
        float positive = AudioForge::DSP::SaturationAlgorithms::tube(2.0f);
        float negative = AudioForge::DSP::SaturationAlgorithms::tube(-2.0f);

        expect(std::abs(positive) != std::abs(negative),
             "Tube should be asymmetric (different +/- curves)");

        // Test positive harder than negative
        expect(std::abs(positive) < std::abs(negative),
             "Positive should clip harder than negative");

        // Test saturation bounds
        float extreme = AudioForge::DSP::SaturationAlgorithms::tube(10.0f);
        expect(std::abs(extreme) < 2.0f,
             "Extreme input should saturate");
    }

    void testTransistorSaturation()
    {
        beginTest("Transistor saturation");

        // Test linear region
        float small = AudioForge::DSP::SaturationAlgorithms::transistor(0.3f);
        expectWithinAbsoluteError(small, 0.3f, 0.05f,
                                "Small signal should be in linear region");

        // Test hard clipping threshold
        float medium = AudioForge::DSP::SaturationAlgorithms::transistor(1.5f);
        expect(medium > 0.7f && medium < 1.2f,
             "Medium signal should clip near threshold");

        // Test saturation
        float large = AudioForge::DSP::SaturationAlgorithms::transistor(5.0f);
        expect(std::abs(large) < 1.5f,
             "Large signal should saturate hard");

        // Test symmetry (transistor should be fairly symmetric)
        float pos = AudioForge::DSP::SaturationAlgorithms::transistor(2.0f);
        float neg = AudioForge::DSP::SaturationAlgorithms::transistor(-2.0f);
        expectWithinAbsoluteError(std::abs(pos), std::abs(neg), 0.1f,
                                "Transistor should be roughly symmetric");
    }

    void testTransformerSaturation()
    {
        beginTest("Transformer saturation gentleness");

        // Test very gentle compression
        float medium = AudioForge::DSP::SaturationAlgorithms::transformer(1.0f);
        expect(medium > 0.7f && medium < 1.0f,
             "Medium signal should compress gently");

        // Test subtle asymmetry
        float pos = AudioForge::DSP::SaturationAlgorithms::transformer(1.0f);
        float neg = AudioForge::DSP::SaturationAlgorithms::transformer(-1.0f);
        expect(std::abs(pos) != std::abs(neg),
             "Transformer should have subtle asymmetry");

        // Test smooth saturation
        float large = AudioForge::DSP::SaturationAlgorithms::transformer(5.0f);
        expect(std::abs(large) < 1.5f,
             "Large signal should saturate smoothly");
    }

    void testDiodeSaturation()
    {
        beginTest("Diode saturation asymmetry");

        // Test very asymmetric behavior
        float pos = AudioForge::DSP::SaturationAlgorithms::diode(2.0f);
        float neg = AudioForge::DSP::SaturationAlgorithms::diode(-2.0f);

        // Positive should clip much harder than negative
        expect(std::abs(pos) < 1.0f,
             "Positive should clip hard (low threshold)");
        expect(std::abs(neg) > 1.5f,
             "Negative should remain more linear (high threshold)");

        // Test extreme asymmetry
        float ratio = std::abs(neg) / std::abs(pos);
        expect(ratio > 1.5f,
             "Diode should have strong asymmetry");
    }

    void testOutputStability()
    {
        beginTest("Output stability for extreme inputs");

        // Test all algorithms with extreme inputs
        float extremeInput = 100.0f;

        float tape = AudioForge::DSP::SaturationAlgorithms::tape(extremeInput);
        expect(std::isfinite(tape) && std::abs(tape) < 10.0f,
             "Tape should produce finite, bounded output");

        float tube = AudioForge::DSP::SaturationAlgorithms::tube(extremeInput);
        expect(std::isfinite(tube) && std::abs(tube) < 10.0f,
             "Tube should produce finite, bounded output");

        float transistor = AudioForge::DSP::SaturationAlgorithms::transistor(extremeInput);
        expect(std::isfinite(transistor) && std::abs(transistor) < 10.0f,
             "Transistor should produce finite, bounded output");

        float transformer = AudioForge::DSP::SaturationAlgorithms::transformer(extremeInput);
        expect(std::isfinite(transformer) && std::abs(transformer) < 10.0f,
             "Transformer should produce finite, bounded output");

        float diode = AudioForge::DSP::SaturationAlgorithms::diode(extremeInput);
        expect(std::isfinite(diode) && std::abs(diode) < 10.0f,
             "Diode should produce finite, bounded output");
    }

    void testUnifiedInterface()
    {
        beginTest("Unified saturate() interface");

        float input = 1.5f;

        // Test all modes
        float tape = AudioForge::DSP::SaturationAlgorithms::saturate(input, 0);
        expectWithinAbsoluteError(tape,
                                AudioForge::DSP::SaturationAlgorithms::tape(input),
                                0.0001f,
                                "Mode 0 should call tape()");

        float tube = AudioForge::DSP::SaturationAlgorithms::saturate(input, 1);
        expectWithinAbsoluteError(tube,
                                AudioForge::DSP::SaturationAlgorithms::tube(input),
                                0.0001f,
                                "Mode 1 should call tube()");

        float transistor = AudioForge::DSP::SaturationAlgorithms::saturate(input, 2);
        expectWithinAbsoluteError(transistor,
                                AudioForge::DSP::SaturationAlgorithms::transistor(input),
                                0.0001f,
                                "Mode 2 should call transistor()");

        float transformer = AudioForge::DSP::SaturationAlgorithms::saturate(input, 3);
        expectWithinAbsoluteError(transformer,
                                AudioForge::DSP::SaturationAlgorithms::transformer(input),
                                0.0001f,
                                "Mode 3 should call transformer()");

        float diode = AudioForge::DSP::SaturationAlgorithms::saturate(input, 4);
        expectWithinAbsoluteError(diode,
                                AudioForge::DSP::SaturationAlgorithms::diode(input),
                                0.0001f,
                                "Mode 4 should call diode()");

        // Test invalid mode
        float passthrough = AudioForge::DSP::SaturationAlgorithms::saturate(input, 99);
        expectEquals(passthrough, input,
                    "Invalid mode should pass through unchanged");
    }
};

static SaturationAlgorithmsTests saturationAlgorithmsTests;
