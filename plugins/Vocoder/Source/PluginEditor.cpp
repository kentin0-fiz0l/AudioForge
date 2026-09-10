#include "PluginEditor.h"

VocoderEditor::VocoderEditor(VocoderProcessor& p) : AudioProcessorEditor(&p), processor_(p) {
    setSize(850, 450);

    auto setup = [this](juce::Label& l, const char* t, juce::Slider& s, auto& a, const char* id) {
        l.setText(t, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffdddddd));
        addAndMakeVisible(l);
        setupRotarySlider(s);
        addAndMakeVisible(s);
        a = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor_.getValueTreeState(), id, s);
    };

    setup(numBandsLabel_, "Bands", numBandsSlider_, numBandsAttachment_, "numBands");
    numBandsSlider_.setTextValueSuffix("");
    
    setup(formantLabel_, "Formant", formantSlider_, formantAttachment_, "formantShift");
    setup(carrierLabel_, "Carrier", carrierSlider_, carrierAttachment_, "carrierLevel");
    setup(modulatorLabel_, "Modulator", modulatorSlider_, modulatorAttachment_, "modulatorLevel");
    setup(attackLabel_, "Attack", attackSlider_, attackAttachment_, "attack");
    setup(releaseLabel_, "Release", releaseSlider_, releaseAttachment_, "release");
    setup(mixLabel_, "Mix", mixSlider_, mixAttachment_, "mix");

    startTimerHz(30); // 30fps for visualizer
}

VocoderEditor::~VocoderEditor() {
    stopTimer();
}

void VocoderEditor::timerCallback() {
    repaint(20, 90, 810, 150); // Repaint visualizer area only
}

void VocoderEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
    
    // Title
    g.setColour(juce::Colour(0xff44ddaa));
    g.setFont(juce::Font(42.0f, juce::Font::bold));
    g.drawText("VOCODER", getLocalBounds().removeFromTop(70), juce::Justification::centred);
    
    // Main panel
    g.setColour(juce::Colour(0xff222222));
    g.fillRect(20, 90, 810, 330);
    
    // Band visualizer
    drawBandVisualizer(g, juce::Rectangle<int>(30, 100, 790, 130));
    
    // Section labels
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff44ddaa));
    g.drawText("SPECTRUM ANALYZER", 30, 95, 200, 20, juce::Justification::left);
    g.drawText("CONTROLS", 30, 245, 200, 20, juce::Justification::left);
    
    // Help text
    g.setFont(juce::Font(11.0f));
    g.setColour(juce::Colour(0xff888888));
    g.drawText("Play MIDI notes to generate carrier • Audio input becomes modulator", 
               30, 420, 790, 20, juce::Justification::centred);
}

void VocoderEditor::drawBandVisualizer(juce::Graphics& g, juce::Rectangle<int> bounds) {
    int numBands = static_cast<int>(numBandsSlider_.getValue());
    float bandWidth = bounds.getWidth() / static_cast<float>(numBands);
    
    for (int i = 0; i < numBands; ++i) {
        float level = processor_.getEngine().getBandLevel(i);
        float height = level * bounds.getHeight();
        
        int x = bounds.getX() + static_cast<int>(i * bandWidth);
        int y = bounds.getBottom() - static_cast<int>(height);
        int w = static_cast<int>(bandWidth) - 2;
        int h = static_cast<int>(height);
        
        // Gradient color based on level
        juce::Colour barColor = juce::Colour(0xff44ddaa).withBrightness(0.3f + level * 0.7f);
        g.setColour(barColor);
        g.fillRect(x, y, w, h);
        
        // Bar outline
        g.setColour(juce::Colour(0xff333333));
        g.drawRect(x, bounds.getY(), w, bounds.getHeight(), 1);
    }
    
    // Visualizer border
    g.setColour(juce::Colour(0xff444444));
    g.drawRect(bounds, 2);
}

void VocoderEditor::resized() {
    int ks = 70;
    int y = 270;
    
    numBandsLabel_.setBounds(60, y, ks, 18);
    numBandsSlider_.setBounds(60, y + 20, ks, ks);
    
    formantLabel_.setBounds(160, y, ks, 18);
    formantSlider_.setBounds(160, y + 20, ks, ks);
    
    carrierLabel_.setBounds(260, y, ks, 18);
    carrierSlider_.setBounds(260, y + 20, ks, ks);
    
    modulatorLabel_.setBounds(360, y, ks, 18);
    modulatorSlider_.setBounds(360, y + 20, ks, ks);
    
    attackLabel_.setBounds(460, y, ks, 18);
    attackSlider_.setBounds(460, y + 20, ks, ks);
    
    releaseLabel_.setBounds(560, y, ks, 18);
    releaseSlider_.setBounds(560, y + 20, ks, ks);
    
    mixLabel_.setBounds(660, y, ks, 18);
    mixSlider_.setBounds(660, y + 20, ks, ks);
}

void VocoderEditor::setupRotarySlider(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff44ddaa));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xff22bb88));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffdddddd));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
