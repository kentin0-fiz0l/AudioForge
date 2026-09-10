#include "XYPadComponent.h"

XYPadComponent::XYPadComponent(juce::AudioProcessorValueTreeState& apvts)
    : apvts_(apvts) {
    // Initialize from saved parameters
    if (auto* xParam = apvts_.getRawParameterValue("xPosition"))
        xPosition_ = xParam->load();
    if (auto* yParam = apvts_.getRawParameterValue("yPosition"))
        yPosition_ = yParam->load();
}

XYPadComponent::~XYPadComponent() {}

void XYPadComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRoundedRectangle(bounds, 8.0f);

    // Grid lines
    g.setColour(juce::Colour(0xff404040));
    for (int i = 1; i < 4; ++i) {
        float x = bounds.getWidth() * i / 4.0f;
        g.drawLine(x, 0, x, bounds.getHeight(), 1.0f);

        float y = bounds.getHeight() * i / 4.0f;
        g.drawLine(0, y, bounds.getWidth(), y, 1.0f);
    }

    // Center crosshair
    g.setColour(juce::Colour(0xff505050));
    float centerX = bounds.getWidth() / 2.0f;
    float centerY = bounds.getHeight() / 2.0f;
    g.drawLine(centerX, 0, centerX, bounds.getHeight(), 1.5f);
    g.drawLine(0, centerY, bounds.getWidth(), centerY, 1.5f);

    // Position indicator (ball)
    float ballX = xPosition_ * bounds.getWidth();
    float ballY = (1.0f - yPosition_) * bounds.getHeight();  // Flip Y (screen coords)

    float ballRadius = 15.0f;

    // Glow effect
    juce::ColourGradient gradient(
        juce::Colour(0x8000d4ff), ballX, ballY,
        juce::Colour(0x0000d4ff), ballX, ballY,
        true);
    gradient.addColour(0.0, juce::Colour(0xff00d4ff));
    gradient.addColour(1.0, juce::Colour(0x0000d4ff));

    g.setGradientFill(gradient);
    g.fillEllipse(ballX - ballRadius * 1.5f, ballY - ballRadius * 1.5f,
                  ballRadius * 3.0f, ballRadius * 3.0f);

    // Ball
    g.setColour(isDragging_ ? juce::Colour(0xff00ffff) : juce::Colour(0xff00d4ff));
    g.fillEllipse(ballX - ballRadius, ballY - ballRadius, ballRadius * 2.0f, ballRadius * 2.0f);

    // Ball outline
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.drawEllipse(ballX - ballRadius, ballY - ballRadius, ballRadius * 2.0f, ballRadius * 2.0f, 2.0f);

    // Border
    g.setColour(juce::Colour(0xff606060));
    g.drawRoundedRectangle(bounds.reduced(1), 8.0f, 2.0f);

    // Display coordinates
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.setFont(12.0f);
    juce::String coords = juce::String::formatted("X: %.2f  Y: %.2f", xPosition_, yPosition_);
    g.drawText(coords, bounds.reduced(10), juce::Justification::topLeft);
}

void XYPadComponent::resized() {}

void XYPadComponent::mouseDown(const juce::MouseEvent& e) {
    isDragging_ = true;
    updatePosition(e);
    repaint();
}

void XYPadComponent::mouseDrag(const juce::MouseEvent& e) {
    updatePosition(e);
    repaint();
}

void XYPadComponent::mouseUp(const juce::MouseEvent& e) {
    isDragging_ = false;
    repaint();
}

void XYPadComponent::updatePosition(const juce::MouseEvent& e) {
    auto pos = getNormalizedPosition(e);

    xPosition_ = juce::jlimit(0.0f, 1.0f, pos.x);
    yPosition_ = juce::jlimit(0.0f, 1.0f, pos.y);

    // Update parameter tree
    if (auto* xParam = apvts_.getParameter("xPosition"))
        xParam->setValueNotifyingHost(xPosition_);

    if (auto* yParam = apvts_.getParameter("yPosition"))
        yParam->setValueNotifyingHost(yPosition_);

    sendCCMessages();
}

void XYPadComponent::sendCCMessages() {
    if (onCCChange) {
        // Get CC numbers from parameter tree
        int xCC = (int)apvts_.getRawParameterValue("xCC")->load();
        int yCC = (int)apvts_.getRawParameterValue("yCC")->load();

        onCCChange(xCC, xPosition_);
        onCCChange(yCC, yPosition_);
    }
}

juce::Point<float> XYPadComponent::getNormalizedPosition(const juce::MouseEvent& e) const {
    auto bounds = getLocalBounds().toFloat();

    float x = e.position.x / bounds.getWidth();
    float y = 1.0f - (e.position.y / bounds.getHeight());  // Flip Y coordinate

    return {x, y};
}
