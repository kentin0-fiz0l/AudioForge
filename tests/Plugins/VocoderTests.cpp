/**
 * Vocoder Tests
 *
 * The vocoder filters the input (the modulator) into bands, follows each
 * band's level, and applies those levels to the same bands of a sawtooth
 * carrier played from MIDI. These tests drive the processor through its
 * public API and check the sound that comes out:
 * - Changing Bands, before or after prepareToPlay, sets up every band's filter
 * - Both input channels feed the modulator, not only the left one
 * - Formant Shift moves the carrier bands relative to the modulator bands
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>

#include "../../plugins/Vocoder/Source/PluginProcessor.h"

class VocoderTests : public juce::UnitTest
{
public:
    VocoderTests()
        : juce::UnitTest("Vocoder", "Plugins")
    {
    }

    void runTest() override
    {
        testBandsChangedWhileRunningFilterTheCarrier();
        testBandsRestoredBeforePrepareFilterTheCarrier();
        testRightOnlyInputStillVocodes();
        testFormantShiftMovesTheCarrierBands();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;

    enum class Feed { both, leftOnly, rightOnly };

    static void setParam(VocoderProcessor& processor, const juce::String& id, float value)
    {
        auto* param = processor.getValueTreeState().getParameter(id);
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }

    static void prepare(VocoderProcessor& processor)
    {
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);
    }

    // One second of a sine modulator at modFreq, with MIDI note `note` held
    // as the carrier. Returns the left output channel, skipping the first
    // quarter second while the band envelopes settle.
    static juce::AudioBuffer<float> run(VocoderProcessor& processor, float modFreq,
                                        int note, Feed feed = Feed::both)
    {
        const int numBlocks = static_cast<int>(sampleRate / blockSize);
        const int skipBlocks = numBlocks / 4;
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::AudioBuffer<float> output(1, (numBlocks - skipBlocks) * blockSize);
        int sampleIndex = 0;

        for (int block = 0; block < numBlocks; ++block)
        {
            juce::MidiBuffer midi;
            if (block == 0)
                midi.addEvent(juce::MidiMessage::noteOn(1, note, 1.0f), 0);

            for (int i = 0; i < blockSize; ++i, ++sampleIndex)
            {
                const float s = 0.5f * std::sin(juce::MathConstants<float>::twoPi * modFreq
                                                * static_cast<float>(sampleIndex) / static_cast<float>(sampleRate));
                buffer.setSample(0, i, feed == Feed::rightOnly ? 0.0f : s);
                buffer.setSample(1, i, feed == Feed::leftOnly ? 0.0f : s);
            }

            processor.processBlock(buffer, midi);

            if (block >= skipBlocks)
                output.copyFrom(0, (block - skipBlocks) * blockSize, buffer, 0, 0, blockSize);
        }

        // Release the note so a later run on the same processor starts with one voice
        juce::MidiBuffer noteOff;
        noteOff.addEvent(juce::MidiMessage::noteOff(1, note), 0);
        buffer.clear();
        processor.processBlock(buffer, noteOff);

        return output;
    }

    // Level of one frequency in the signal (Goertzel), as an RMS amplitude
    static float levelAt(const juce::AudioBuffer<float>& signal, float freq)
    {
        const int n = signal.getNumSamples();
        const double w = juce::MathConstants<double>::twoPi * freq / sampleRate;
        const double coeff = 2.0 * std::cos(w);
        double s1 = 0.0, s2 = 0.0;
        for (int i = 0; i < n; ++i)
        {
            const double s0 = signal.getSample(0, i) + coeff * s1 - s2;
            s2 = s1;
            s1 = s0;
        }
        const double power = s1 * s1 + s2 * s2 - coeff * s1 * s2;
        return static_cast<float>(std::sqrt(2.0 * power) / n);
    }

    // Fraction of the output's RMS that sits above 3 kHz
    static float highFrequencyShare(const juce::AudioBuffer<float>& signal)
    {
        juce::dsp::IIR::Filter<float> highPass(
            juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 3000.0f));
        double total = 0.0, high = 0.0;
        for (int i = 0; i < signal.getNumSamples(); ++i)
        {
            const float s = signal.getSample(0, i);
            const float h = highPass.processSample(s);
            total += s * s;
            high += h * h;
        }
        return total > 0.0 ? static_cast<float>(std::sqrt(high / total)) : 0.0f;
    }

    // A 200 Hz modulator should only let the low harmonics of a 110 Hz saw
    // through. A band without a band-pass filter passes the whole saw.
    void expectCarrierIsFiltered(VocoderProcessor& processor, const juce::String& what)
    {
        const auto output = run(processor, 200.0f, 45);
        const float rms = output.getRMSLevel(0, 0, output.getNumSamples());
        const float share = highFrequencyShare(output);
        expect(rms > 1.0e-4f, what + ": output should not be silent");
        expect(share < 0.05f, what + ": share of output above 3 kHz should be small, got "
                                  + juce::String(share, 3));
    }

    void testBandsChangedWhileRunningFilterTheCarrier()
    {
        beginTest("Changing Bands while running sets up the new bands' filters");

        VocoderProcessor processor;
        setParam(processor, "numBands", 8.0f);
        prepare(processor);

        setParam(processor, "numBands", 32.0f);
        expectCarrierIsFiltered(processor, "Bands 8 -> 32 after prepare");
    }

    void testBandsRestoredBeforePrepareFilterTheCarrier()
    {
        beginTest("Bands set before prepareToPlay (a restored session) is used at prepare");

        VocoderProcessor processor;
        setParam(processor, "numBands", 32.0f);
        prepare(processor);

        expectCarrierIsFiltered(processor, "Bands 32 before prepare");
    }

    void testRightOnlyInputStillVocodes()
    {
        beginTest("Input on the right channel only still drives the vocoder");

        VocoderProcessor left, right;
        prepare(left);
        prepare(right);

        const auto leftOut = run(left, 300.0f, 45, Feed::leftOnly);
        const auto rightOut = run(right, 300.0f, 45, Feed::rightOnly);
        const float leftRms = leftOut.getRMSLevel(0, 0, leftOut.getNumSamples());
        const float rightRms = rightOut.getRMSLevel(0, 0, rightOut.getNumSamples());

        expect(leftRms > 1.0e-4f, "Left-only input should produce output");
        expect(rightRms > 1.0e-4f, "Right-only input should produce output, got "
                                       + juce::String(rightRms, 6));
        expectWithinAbsoluteError(rightRms, leftRms, leftRms * 0.01f,
                                  "Left-only and right-only input should vocode the same");
    }

    void testFormantShiftMovesTheCarrierBands()
    {
        beginTest("Formant Shift moves the carrier bands relative to the modulator");

        // 220 Hz saw (A3) has harmonics at 220, 440, 660 Hz. The modulator
        // sits at 220 Hz, so unshifted the 220 Hz harmonic dominates; with
        // the carrier bands doubled, the 440 Hz harmonic should.
        VocoderProcessor unshifted, shifted;
        setParam(shifted, "formantShift", 2.0f);
        prepare(unshifted);
        prepare(shifted);

        const auto plain = run(unshifted, 220.0f, 57);
        const auto up = run(shifted, 220.0f, 57);

        expect(levelAt(plain, 220.0f) > levelAt(plain, 440.0f),
               "At Formant Shift 1.0 the 220 Hz harmonic should be loudest");
        expect(levelAt(up, 440.0f) > levelAt(up, 220.0f),
               "At Formant Shift 2.0 the 440 Hz harmonic should be loudest, got 220: "
                   + juce::String(levelAt(up, 220.0f), 5) + " 440: "
                   + juce::String(levelAt(up, 440.0f), 5));

        // Changing it while running takes effect too
        setParam(unshifted, "formantShift", 2.0f);
        const auto changed = run(unshifted, 220.0f, 57);
        expect(levelAt(changed, 440.0f) > levelAt(changed, 220.0f),
               "Changing Formant Shift while running should move the carrier bands");
    }
};

static VocoderTests vocoderTests;
