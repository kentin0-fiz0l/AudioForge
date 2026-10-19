#include "PluginEditor.h"
ConvolutionReverbEditor::ConvolutionReverbEditor(ConvolutionReverbProcessor& p) : AudioProcessorEditor(&p), processor_(p) {
    setSize(700, 350);

    auto setup = [this](juce::Label& l, const char* t, juce::Slider& s, auto& a, const char* id) {
        l.setText(t, juce::dontSendNotification); l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd)); addAndMakeVisible(l);
        setupRotarySlider(s); addAndMakeVisible(s);
        a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor_.getValueTreeState(), id, s);
    };
    setup(mixLabel_, "Mix", mixSlider_, mixAttachment_, "mix");
    setup(preDelayLabel_, "Pre-Delay", preDelaySlider_, preDelayAttachment_, "predelay");
    setup(lowCutLabel_, "Low Cut", lowCutSlider_, lowCutAttachment_, "lowcut");
    setup(highCutLabel_, "High Cut", highCutSlider_, highCutAttachment_, "highcut");
    setup(outputLabel_, "Output", outputSlider_, outputAttachment_, "output");

    bypassButton_.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffdddddd));
    addAndMakeVisible(bypassButton_);
    bypassAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor_.getValueTreeState(), "bypass", bypassButton_);

    irLabel_.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd));
    addAndMakeVisible(irLabel_);
    showImpulseResponse();

    loadButton_.onClick = [this] {
        chooser_ = std::make_unique<juce::FileChooser>("Impulse response", juce::File(), "*.wav;*.aif;*.aiff;*.flac");
        chooser_->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [this](const juce::FileChooser& fc) {
                                  if (fc.getResult() == juce::File())
                                      return;
                                  juce::String error;
                                  if (! processor_.loadImpulseResponse(fc.getResult(), error))
                                      irLabel_.setText(error, juce::dontSendNotification);
                                  else
                                      showImpulseResponse();
                              });
    };
    addAndMakeVisible(loadButton_);
}
ConvolutionReverbEditor::~ConvolutionReverbEditor() {}
void ConvolutionReverbEditor::showImpulseResponse() {
    const auto& file = processor_.getImpulseResponseFile();
    irLabel_.setText(file == juce::File() ? juce::String("No impulse response loaded")
                                          : file.getFileName() + "  (" + juce::String(processor_.getImpulseResponseSeconds(), 1) + " s)",
                     juce::dontSendNotification);
}
void ConvolutionReverbEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
    g.setColour(juce::Colour(0xff66ccaa));
    g.setFont(juce::Font(juce::FontOptions(38.0f, juce::Font::bold)));
    g.drawText("CONVOLUTION REVERB", getLocalBounds().removeFromTop(65), juce::Justification::centred);
    g.setFont(juce::Font(juce::FontOptions(15.0f, juce::Font::bold)));
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 660, 230);
    g.setColour(juce::Colour(0xff66ccaa));
    g.drawText("IMPULSE RESPONSE", 30, 95, 630, 22, juce::Justification::left);
}
void ConvolutionReverbEditor::resized() {
    int ks = 80;
    irLabel_.setBounds(30, 118, 480, 20); loadButton_.setBounds(520, 116, 90, 24); bypassButton_.setBounds(615, 116, 60, 24);
    mixLabel_.setBounds(40, 150, ks, 18); mixSlider_.setBounds(40, 170, ks, ks);
    preDelayLabel_.setBounds(160, 150, ks, 18); preDelaySlider_.setBounds(160, 170, ks, ks);
    lowCutLabel_.setBounds(280, 150, ks, 18); lowCutSlider_.setBounds(280, 170, ks, ks);
    highCutLabel_.setBounds(400, 150, ks, 18); highCutSlider_.setBounds(400, 170, ks, ks);
    outputLabel_.setBounds(520, 150, ks, 18); outputSlider_.setBounds(520, 170, ks, ks);
}
void ConvolutionReverbEditor::setupRotarySlider(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff66ccaa));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xff44aa88));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
