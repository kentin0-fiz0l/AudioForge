#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>
#include <vector>

namespace AudioForge
{

/**
 * Ties an editor's controls to its plugin's parameters, both ways. A control
 * takes its parameter's range, skew and current value; follows the host's
 * changes, a loaded session's and automation's; and sets the parameter when
 * moved. Each control is given its parameter's ID as its component ID.
 *
 * Declare it after the controls it attaches, so it is destroyed first.
 */
class ParameterAttachments
{
public:
    explicit ParameterAttachments(juce::AudioProcessor& processorToUse)
        : processor(processorToUse)
    {
    }

    void attach(juce::Slider& slider, const juce::String& id)
    {
        if (auto* param = find(slider, id))
            sliders.push_back(std::make_unique<juce::SliderParameterAttachment>(*param, slider));
    }

    void attach(juce::Button& button, const juce::String& id)
    {
        if (auto* param = find(button, id))
            buttons.push_back(std::make_unique<juce::ButtonParameterAttachment>(*param, button));
    }

    void attach(juce::ComboBox& comboBox, const juce::String& id)
    {
        if (auto* param = find(comboBox, id))
            comboBoxes.push_back(std::make_unique<juce::ComboBoxParameterAttachment>(*param, comboBox));
    }

private:
    juce::RangedAudioParameter* find(juce::Component& control, const juce::String& id)
    {
        control.setComponentID(id);

        for (auto* param : processor.getParameters())
            if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param); ranged != nullptr && ranged->paramID == id)
                return ranged;

        jassertfalse;  // No parameter has this ID
        return nullptr;
    }

    juce::AudioProcessor& processor;
    std::vector<std::unique_ptr<juce::SliderParameterAttachment>> sliders;
    std::vector<std::unique_ptr<juce::ButtonParameterAttachment>> buttons;
    std::vector<std::unique_ptr<juce::ComboBoxParameterAttachment>> comboBoxes;

    JUCE_DECLARE_NON_COPYABLE(ParameterAttachments)
};

} // namespace AudioForge
