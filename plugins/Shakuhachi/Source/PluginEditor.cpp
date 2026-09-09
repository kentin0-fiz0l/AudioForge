#include "PluginEditor.h"

ShakuhachiEditor::ShakuhachiEditor(ShakuhachiProcessor& p)
    : AudioProcessorEditor(&p), processor_(p)
{
    setSize(800, 500);

    // Setup controls with bamboo/zen naming
    auto setupControl = [this](juce::Label& label, const char* text, juce::Slider& slider,
                               auto& attachment, const char* paramId) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);

        setupRotarySlider(slider);
        addAndMakeVisible(slider);

        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor_.getValueTreeState(), paramId, slider);
    };

    setupControl(breathPressureLabel_, "Breath", breathPressureSlider_, breathPressureAttachment_, "breathPressure");
    setupControl(embouchureLabel_, "Meri/Kari", embouchureSlider_, embouchureAttachment_, "embouchure");
    setupControl(airNoiseLabel_, "Air", airNoiseSlider_, airNoiseAttachment_, "airNoise");
    setupControl(brightnessLabel_, "Brightness", brightnessSlider_, brightnessAttachment_, "brightness");
    setupControl(attackLabel_, "Attack", attackSlider_, attackAttachment_, "attack");
    setupControl(releaseLabel_, "Release", releaseSlider_, releaseAttachment_, "release");
    setupControl(vibratoRateLabel_, "Vib. Rate", vibratoRateSlider_, vibratoRateAttachment_, "vibratoRate");
    setupControl(vibratoDepthLabel_, "Vib. Depth", vibratoDepthSlider_, vibratoDepthAttachment_, "vibratoDepth");
}

ShakuhachiEditor::~ShakuhachiEditor() {}

void ShakuhachiEditor::paint(juce::Graphics& g)
{
    // Bamboo/zen aesthetic - green-brown gradient
    juce::ColourGradient gradient(
        juce::Colour(0xff1a1f1a), 0, 0,
        juce::Colour(0xff2d3f2d), 0, static_cast<float>(getHeight()),
        false);
    g.setGradientFill(gradient);
    g.fillAll();

    // Title with Japanese characters
    g.setColour(juce::Colour(0xffa8c4a8)); // Bamboo green
    g.setFont(juce::Font(32.0f, juce::Font::bold));
    g.drawText("尺八 Shakuhachi", getLocalBounds().removeFromTop(65), juce::Justification::centred);

    // Section headers
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff8faa8f));
    g.drawText("BREATH", 30, 110, 200, 25, juce::Justification::left);
    g.drawText("TONE", 230, 110, 200, 25, juce::Justification::left);
    g.drawText("ENVELOPE", 430, 110, 200, 25, juce::Justification::left);
    g.drawText("MODULATION", 30, 290, 200, 25, juce::Justification::left);

    // Decorative lines (bamboo segments)
    g.setColour(juce::Colour(0xff5f7a5f));
    g.drawLine(30, 105, getWidth() - 30, 105, 2.0f);
    g.drawLine(30, 285, getWidth() - 30, 285, 2.0f);

    // Zen circle (ensō) in background
    g.setColour(juce::Colour(0xff3f5a3f).withAlpha(0.15f));
    g.drawEllipse(getWidth() / 2 - 150, getHeight() / 2 - 30, 300, 300, 8.0f);
}

void ShakuhachiEditor::resized()
{
    int sliderSize = 80;
    int yPos = 145;

    // Breath controls
    breathPressureLabel_.setBounds(50, yPos, sliderSize, 20);
    breathPressureSlider_.setBounds(50, yPos + 25, sliderSize, sliderSize);

    embouchureLabel_.setBounds(145, yPos, sliderSize, 20);
    embouchureSlider_.setBounds(145, yPos + 25, sliderSize, sliderSize);

    // Tone controls
    airNoiseLabel_.setBounds(250, yPos, sliderSize, 20);
    airNoiseSlider_.setBounds(250, yPos + 25, sliderSize, sliderSize);

    brightnessLabel_.setBounds(345, yPos, sliderSize, 20);
    brightnessSlider_.setBounds(345, yPos + 25, sliderSize, sliderSize);

    // Envelope controls
    attackLabel_.setBounds(450, yPos, sliderSize, 20);
    attackSlider_.setBounds(450, yPos + 25, sliderSize, sliderSize);

    releaseLabel_.setBounds(545, yPos, sliderSize, 20);
    releaseSlider_.setBounds(545, yPos + 25, sliderSize, sliderSize);

    // Modulation controls
    yPos = 325;
    vibratoRateLabel_.setBounds(50, yPos, sliderSize, 20);
    vibratoRateSlider_.setBounds(50, yPos + 25, sliderSize, sliderSize);

    vibratoDepthLabel_.setBounds(145, yPos, sliderSize, 20);
    vibratoDepthSlider_.setBounds(145, yPos + 25, sliderSize, sliderSize);
}

void ShakuhachiEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffa8c4a8)); // Bamboo green
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff8faa8f));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffddeedd));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
