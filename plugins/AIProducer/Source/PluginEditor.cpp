#include "PluginEditor.h"

namespace {

const char* const kChoiceIds[] = {"genre", "key", "scale", "length", "part"};
const char* const kChoiceNames[] = {"Genre", "Key", "Scale", "Length", "Part"};
const char* const kPartIds[] = {"drums", "bass", "chords", "lead"};
const char* const kPartNames[] = {"Drums (ch 10)", "Bass (ch 1)", "Chords (ch 2)", "Lead (ch 3)"};

} // namespace

AIProducerEditor::AIProducerEditor(AIProducerProcessor& p)
    : AudioProcessorEditor(&p), processor_(p) {
    auto& apvts = processor_.getValueTreeState();

    for (size_t i = 0; i < choices_.size(); ++i) {
        auto& choice = choices_[i];
        choice.label.setText(kChoiceNames[i], juce::dontSendNotification);
        choice.label.setJustificationType(juce::Justification::centredLeft);
        if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(kChoiceIds[i])))
            choice.box.addItemList(param->choices, 1);
        choice.attachment = std::make_unique<ComboAttachment>(apvts, kChoiceIds[i], choice.box);
        addAndMakeVisible(choice.label);
        addAndMakeVisible(choice.box);
    }

    for (size_t i = 0; i < parts_.size(); ++i) {
        auto& part = parts_[i];
        part.button.setButtonText(kPartNames[i]);
        part.attachment = std::make_unique<ButtonAttachment>(apvts, kPartIds[i], part.button);
        addAndMakeVisible(part.button);
    }

    variationButton_.setButtonText("New Variation");
    variationButton_.onClick = [this] { processor_.nextVariation(); };
    addAndMakeVisible(variationButton_);
    addAndMakeVisible(variationLabel_);

    descriptionLabel_.setJustificationType(juce::Justification::centred);
    descriptionLabel_.setFont(juce::Font(15.0f, juce::Font::bold));
    addAndMakeVisible(descriptionLabel_);

    routingLabel_.setText("Plays along the song from bar 1 at the tempo shown. Route each MIDI channel to its own "
                          "instrument, or (in Live) use one copy per Part with the same settings.",
                          juce::dontSendNotification);
    routingLabel_.setJustificationType(juce::Justification::centred);
    routingLabel_.setFont(juce::Font(12.0f));
    addAndMakeVisible(routingLabel_);

    timerCallback();
    startTimerHz(5);
    setSize(600, 420);
}

AIProducerEditor::~AIProducerEditor() {
    stopTimer();
}

void AIProducerEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a2e));

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("Track Sketch", getLocalBounds().removeFromTop(60), juce::Justification::centred);

    g.setColour(juce::Colour(0xff16213e));
    g.drawRect(getLocalBounds(), 2);
}

void AIProducerEditor::resized() {
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Title

    // Drop-downs, two to a row
    for (size_t first = 0; first < choices_.size(); first += 2) {
        auto line = bounds.removeFromTop(32);
        const int cellWidth = line.getWidth() / 2;
        for (size_t i = first; i < std::min(first + 2, choices_.size()); ++i) {
            auto& choice = choices_[i];
            auto cell = line.removeFromLeft(cellWidth).reduced(6, 2);
            choice.label.setBounds(cell.removeFromLeft(60));
            choice.box.setBounds(cell);
        }
        bounds.removeFromTop(8);
    }

    bounds.removeFromTop(6);
    auto partRow = bounds.removeFromTop(30);
    const int partWidth = partRow.getWidth() / static_cast<int>(parts_.size());
    for (auto& part : parts_)
        part.button.setBounds(partRow.removeFromLeft(partWidth).reduced(4, 0));

    bounds.removeFromTop(16);
    auto variationRow = bounds.removeFromTop(36).withSizeKeepingCentre(320, 36);
    variationButton_.setBounds(variationRow.removeFromLeft(180));
    variationLabel_.setBounds(variationRow.reduced(10, 0));

    bounds.removeFromTop(20);
    descriptionLabel_.setBounds(bounds.removeFromTop(28));
    routingLabel_.setBounds(bounds.removeFromTop(40));
}

void AIProducerEditor::timerCallback() {
    const auto description = processor_.getDescription();
    if (descriptionLabel_.getText() != description)
        descriptionLabel_.setText(description, juce::dontSendNotification);

    if (auto* variation = processor_.getValueTreeState().getParameter("variation")) {
        const auto text = "Variation " + variation->getCurrentValueAsText();
        if (variationLabel_.getText() != text)
            variationLabel_.setText(text, juce::dontSendNotification);
    }
}
