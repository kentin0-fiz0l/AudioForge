#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Setup all drawbar faders (vertical)
    setupDrawbar(drawbar16Slider, drawbar16Label, "16'", drawbar16Attachment, "drawbar16");
    setupDrawbar(drawbar5_3Slider, drawbar5_3Label, "5⅓'", drawbar5_3Attachment, "drawbar5_3");
    setupDrawbar(drawbar8Slider, drawbar8Label, "8'", drawbar8Attachment, "drawbar8");
    setupDrawbar(drawbar4Slider, drawbar4Label, "4'", drawbar4Attachment, "drawbar4");
    setupDrawbar(drawbar2_3Slider, drawbar2_3Label, "2⅔'", drawbar2_3Attachment, "drawbar2_3");
    setupDrawbar(drawbar2Slider, drawbar2Label, "2'", drawbar2Attachment, "drawbar2");
    setupDrawbar(drawbar1_3_5Slider, drawbar1_3_5Label, "1⅗'", drawbar1_3_5Attachment, "drawbar1_3_5");
    setupDrawbar(drawbar1_1_3Slider, drawbar1_1_3Label, "1⅓'", drawbar1_1_3Attachment, "drawbar1_1_3");
    setupDrawbar(drawbar1Slider, drawbar1Label, "1'", drawbar1Attachment, "drawbar1");

    // Setup Leslie controls (rotary knobs)
    setupKnob(leslieSpeedSlider, leslieSpeedLabel, "Speed", leslieSpeedAttachment, "leslieSpeed");
    setupKnob(leslieDepthSlider, leslieDepthLabel, "Depth", leslieDepthAttachment, "leslieDepth");
    setupKnob(leslieMixSlider, leslieMixLabel, "Mix", leslieMixAttachment, "leslieMix");

    // Setup percussion controls (rotary knobs)
    setupKnob(percussionClickSlider, percussionClickLabel, "Click", percussionClickAttachment, "percussionClick");
    setupKnob(percussionDecaySlider, percussionDecayLabel, "Decay", percussionDecayAttachment, "percussionDecay");

    setSize(950, 500);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::setupDrawbar(juce::Slider& slider, juce::Label& label,
                                 const juce::String& labelText,
                                 std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                                 const juce::String& parameterID)
{
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), parameterID, slider);
}

void PluginEditor::setupKnob(juce::Slider& slider, juce::Label& label,
                             const juce::String& labelText,
                             std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                             const juce::String& parameterID)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), parameterID, slider);
}

void PluginEditor::paint(juce::Graphics& g)
{
    // Dark background
    g.fillAll(juce::Colour(0xff1a1a1a));

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(28.0f);
    g.drawFittedText("Organ Emulator", 10, 10, getWidth() - 20, 40, juce::Justification::centred, 1);

    // Section backgrounds
    auto drawbarsArea = juce::Rectangle<int>(30, 70, 600, 400);
    auto leslieArea = juce::Rectangle<int>(660, 70, 260, 180);
    auto percussionArea = juce::Rectangle<int>(660, 270, 260, 180);

    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRoundedRectangle(drawbarsArea.toFloat(), 8.0f);
    g.fillRoundedRectangle(leslieArea.toFloat(), 8.0f);
    g.fillRoundedRectangle(percussionArea.toFloat(), 8.0f);

    // Section labels
    g.setColour(juce::Colours::lightblue);
    g.setFont(18.0f);
    g.drawText("Drawbars", drawbarsArea.getX(), drawbarsArea.getY() + 5, drawbarsArea.getWidth(), 25, juce::Justification::centred);
    g.drawText("Leslie", leslieArea.getX(), leslieArea.getY() + 5, leslieArea.getWidth(), 25, juce::Justification::centred);
    g.drawText("Percussion", percussionArea.getX(), percussionArea.getY() + 5, percussionArea.getWidth(), 25, juce::Justification::centred);
}

void PluginEditor::resized()
{
    // Drawbars section (9 vertical faders)
    int drawbarX = 50;
    int drawbarY = 120;
    int drawbarWidth = 50;
    int drawbarHeight = 320;
    int drawbarSpacing = 65;

    drawbar16Slider.setBounds(drawbarX, drawbarY, drawbarWidth, drawbarHeight);
    drawbar5_3Slider.setBounds(drawbarX + drawbarSpacing, drawbarY, drawbarWidth, drawbarHeight);
    drawbar8Slider.setBounds(drawbarX + drawbarSpacing * 2, drawbarY, drawbarWidth, drawbarHeight);
    drawbar4Slider.setBounds(drawbarX + drawbarSpacing * 3, drawbarY, drawbarWidth, drawbarHeight);
    drawbar2_3Slider.setBounds(drawbarX + drawbarSpacing * 4, drawbarY, drawbarWidth, drawbarHeight);
    drawbar2Slider.setBounds(drawbarX + drawbarSpacing * 5, drawbarY, drawbarWidth, drawbarHeight);
    drawbar1_3_5Slider.setBounds(drawbarX + drawbarSpacing * 6, drawbarY, drawbarWidth, drawbarHeight);
    drawbar1_1_3Slider.setBounds(drawbarX + drawbarSpacing * 7, drawbarY, drawbarWidth, drawbarHeight);
    drawbar1Slider.setBounds(drawbarX + drawbarSpacing * 8, drawbarY, drawbarWidth, drawbarHeight);

    // Leslie section (3 rotary knobs)
    int leslieX = 680;
    int leslieY = 120;

    leslieSpeedSlider.setBounds(leslieX, leslieY, 80, 100);
    leslieDepthSlider.setBounds(leslieX + 90, leslieY, 80, 100);
    leslieMixSlider.setBounds(leslieX + 180, leslieY - 80, 80, 100);

    // Percussion section (2 rotary knobs)
    int percX = 700;
    int percY = 320;

    percussionClickSlider.setBounds(percX, percY, 90, 110);
    percussionDecaySlider.setBounds(percX + 110, percY, 90, 110);
}
