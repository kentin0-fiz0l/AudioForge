/**
 * State restore tests
 *
 * A host can hand a plugin a state that is not its own: a set saved by a
 * different version, a corrupt file, or nothing at all. The plugin has to
 * leave its settings alone rather than read through a state that never
 * parsed.
 */

#include <juce_audio_processors/juce_audio_processors.h>

#include "../../plugins/ElectricPiano/Source/PluginProcessor.h"
#include "../../plugins/Polysynth/Source/PluginProcessor.h"
#include "../../plugins/PlateReverb/Source/PluginProcessor.h"

class StateRestoreTests : public juce::UnitTest
{
public:
    StateRestoreTests()
        : juce::UnitTest("State Restore", "Plugins")
    {
    }

    void runTest() override
    {
        testSurvivesBadState<ElectricPianoProcessor>("ElectricPiano");
        testSurvivesBadState<PolysynthProcessor>("Polysynth");
        testSurvivesBadState<ReverbProcessor>("PlateReverb");
        testKeepsItsOwnState<ElectricPianoProcessor>("ElectricPiano");
        testKeepsItsOwnState<PolysynthProcessor>("Polysynth");
        testKeepsItsOwnState<ReverbProcessor>("PlateReverb");
    }

private:
    static std::vector<float> values(juce::AudioProcessor& processor)
    {
        std::vector<float> result;
        for (auto* param : processor.getParameters())
            result.push_back(param->getValue());
        return result;
    }

    template <typename Processor>
    void testSurvivesBadState(const juce::String& name)
    {
        beginTest(name + " ignores a state it cannot read");

        Processor processor;
        const auto before = values(processor);

        // Not a saved state at all
        const char junk[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
        processor.setStateInformation(junk, sizeof(junk));
        processor.setStateInformation(nullptr, 0);

        // A well-formed state that belongs to something else
        juce::XmlElement other("SomeOtherPlugin");
        other.createNewChildElement("MIDILearnMappings");
        juce::MemoryBlock otherState;
        juce::AudioProcessor::copyXmlToBinary(other, otherState);
        processor.setStateInformation(otherState.getData(), static_cast<int>(otherState.getSize()));

        expect(values(processor) == before, "The settings should be as they were");
    }

    template <typename Processor>
    void testKeepsItsOwnState(const juce::String& name)
    {
        beginTest(name + " restores a state it saved");

        Processor original;
        for (auto* param : original.getParameters())
            param->setValueNotifyingHost(param->getDefaultValue() < 0.5f ? 0.75f : 0.25f);

        juce::MemoryBlock state;
        original.getStateInformation(state);

        Processor restored;
        restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));

        const auto expected = values(original);
        const auto actual = values(restored);

        for (size_t i = 0; i < expected.size(); ++i)
            expectWithinAbsoluteError(actual[i], expected[i], 0.001f,
                                      original.getParameters()[static_cast<int>(i)]->getName(64) + " should be restored");
    }
};

static StateRestoreTests stateRestoreTests;
