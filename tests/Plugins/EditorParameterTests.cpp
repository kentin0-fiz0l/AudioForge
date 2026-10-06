/**
 * Editor parameter tests
 *
 * An editor's controls should show the plugin's parameters, not fixed
 * starting values: when the window opens on a saved session, when the host
 * changes a parameter while it is open, and when a control is moved, the
 * control and its parameter should agree. Several editors only ever wrote
 * to their parameters, and two of them (SpectralFreeze's Low Cut and High
 * Cut) wrote the wrong values, mapping a skewed range as if it were linear.
 *
 * Each control carries its parameter's ID as its component ID.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>

#include "../../plugins/CleanDelay/Source/PluginProcessor.h"
#include "../../plugins/CleanDelay/Source/PluginEditor.h"
#include "../../plugins/SimpleGain/Source/PluginProcessor.h"
#include "../../plugins/SimpleGain/Source/PluginEditor.h"
#include "../../plugins/SpectralFreeze/Source/PluginProcessor.h"
#include "../../plugins/SpectralFreeze/Source/PluginEditor.h"

class EditorParameterTests : public juce::UnitTest
{
public:
    EditorParameterTests()
        : juce::UnitTest("Editor Parameters", "Plugins")
    {
    }

    void runTest() override
    {
        check<CleanDelayProcessor>("CleanDelay");
        check<SimpleGainProcessor>("SimpleGain");
        check<SpectralFreezeProcessor>("SpectralFreeze");
    }

private:
    static juce::Array<juce::RangedAudioParameter*> parametersOf(juce::AudioProcessor& processor)
    {
        juce::Array<juce::RangedAudioParameter*> result;
        for (auto* param : processor.getParameters())
            if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param))
                result.add(ranged);
        return result;
    }

    // What a control shows, in the parameter's own units
    static double shown(juce::Component& control)
    {
        if (auto* slider = dynamic_cast<juce::Slider*>(&control))
            return slider->getValue();
        if (auto* button = dynamic_cast<juce::Button*>(&control))
            return button->getToggleState() ? 1.0 : 0.0;
        return std::nan("");
    }

    // Moves a control as a user would
    static void move(juce::Component& control, double value)
    {
        if (auto* slider = dynamic_cast<juce::Slider*>(&control))
            slider->setValue(value, juce::sendNotificationSync);
        else if (auto* button = dynamic_cast<juce::Button*>(&control))
            button->setToggleState(value > 0.5, juce::sendNotificationSync);
    }

    // Close enough: within half a step of the parameter's range, so on the
    // same snapped value (for an on/off parameter, on the same side)
    void expectShows(double actual, juce::RangedAudioParameter& param, double expected, const juce::String& when)
    {
        const double halfStep = juce::jmax(1.0e-4, 0.5 * param.getNormalisableRange().interval);
        expect(std::abs(actual - expected) <= halfStep,
               param.getName(64) + " " + when + ": expected " + juce::String(expected, 3) + ", got " + juce::String(actual, 3));
    }

    template <typename Processor>
    void check(const juce::String& plugin)
    {
        beginTest(plugin + ": the editor opens showing the plugin's settings");
        {
            // As after a saved session is loaded
            Processor processor;
            for (auto* param : parametersOf(processor))
                param->setValueNotifyingHost(0.7f);

            std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());

            for (auto* param : parametersOf(processor))
            {
                auto* control = editor->findChildWithID(param->paramID);
                expect(control != nullptr, "No control for " + param->paramID);
                if (control != nullptr)
                    expectShows(shown(*control), *param, param->convertFrom0to1(param->getValue()), "on opening");
            }
        }

        beginTest(plugin + ": the editor follows changes made by the host");
        {
            Processor processor;
            std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());

            for (auto* param : parametersOf(processor))
            {
                param->setValueNotifyingHost(0.8f);
                if (auto* control = editor->findChildWithID(param->paramID))
                    expectShows(shown(*control), *param, param->convertFrom0to1(param->getValue()), "after the host set it");
            }
        }

        beginTest(plugin + ": moving a control sets the value it shows");
        {
            Processor processor;
            std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());

            for (auto* param : parametersOf(processor))
            {
                // A third of the way along the control, where a skewed range
                // and a straight line disagree the most
                const double target = param->convertFrom0to1(0.33f);
                if (auto* control = editor->findChildWithID(param->paramID))
                {
                    move(*control, target);
                    expectShows(param->convertFrom0to1(param->getValue()), *param, shown(*control), "after its control was moved");
                }
            }
        }
    }
};

static EditorParameterTests editorParameterTests;
