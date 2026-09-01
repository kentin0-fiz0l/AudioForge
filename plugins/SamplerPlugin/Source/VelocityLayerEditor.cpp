#include "VelocityLayerEditor.h"

VelocityLayerEditor::VelocityLayerEditor()
{
    setSize(200, 300);
}

VelocityLayerEditor::~VelocityLayerEditor()
{
}

void VelocityLayerEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.fillAll(juce::Colour(0xff2a2a2a));

    if (currentZone)
    {
        drawVelocityScale(g, bounds);
        drawLayers(g, bounds);
    }
    else
    {
        // No zone selected
        g.setColour(juce::Colours::grey);
        g.setFont(14.0f);
        g.drawText("Select a zone to edit velocity layers",
                  bounds, juce::Justification::centred, true);
    }

    // Border
    g.setColour(juce::Colours::darkgrey);
    g.drawRect(bounds, 1.0f);
}

void VelocityLayerEditor::resized()
{
}

void VelocityLayerEditor::mouseDown(const juce::MouseEvent& e)
{
    if (!currentZone)
        return;

    // Check if clicking near a boundary
    int boundaryIndex;
    if (isNearBoundary(e.position.y, boundaryIndex))
    {
        isDraggingBoundary = true;
        draggedBoundaryIndex = boundaryIndex;
        return;
    }

    // Select layer
    int layerIndex = findLayerAtY(e.position.y);
    if (layerIndex >= 0)
    {
        selectedLayerIndex = layerIndex;
        repaint();
    }
}

void VelocityLayerEditor::mouseDrag(const juce::MouseEvent& e)
{
    if (!currentZone || !isDraggingBoundary)
        return;

    float newVelocity = yToVelocity(e.position.y);
    newVelocity = juce::jlimit(0.0f, 1.0f, newVelocity);

    // Update boundary
    if (draggedBoundaryIndex >= 0 && draggedBoundaryIndex < currentZone->velocityLayers.size())
    {
        auto& layer = currentZone->velocityLayers[draggedBoundaryIndex];

        // Update velocityMax of current layer and velocityMin of next layer
        layer.velocityMax = newVelocity;

        if (draggedBoundaryIndex + 1 < currentZone->velocityLayers.size())
        {
            currentZone->velocityLayers[draggedBoundaryIndex + 1].velocityMin = newVelocity;
        }

        listeners.call([this](Listener& l)
        {
            l.velocityLayerChanged(draggedBoundaryIndex);
        });

        repaint();
    }
}

void VelocityLayerEditor::mouseUp(const juce::MouseEvent& e)
{
    isDraggingBoundary = false;
    draggedBoundaryIndex = -1;
}

void VelocityLayerEditor::setZone(KeyZone* zone, SampleLibrary* library)
{
    currentZone = zone;
    sampleLibrary = library;
    selectedLayerIndex = -1;
    repaint();
}

void VelocityLayerEditor::clearZone()
{
    currentZone = nullptr;
    sampleLibrary = nullptr;
    selectedLayerIndex = -1;
    repaint();
}

void VelocityLayerEditor::addLayer(int sampleIndex)
{
    if (!currentZone)
        return;

    // Split the last layer in half
    if (currentZone->velocityLayers.empty())
    {
        // First layer: full range
        VelocityLayer layer(sampleIndex, 0.0f, 1.0f);
        currentZone->velocityLayers.push_back(layer);
    }
    else
    {
        // Split last layer
        auto& lastLayer = currentZone->velocityLayers.back();
        float midVel = (lastLayer.velocityMin + lastLayer.velocityMax) * 0.5f;

        lastLayer.velocityMax = midVel;

        VelocityLayer newLayer(sampleIndex, midVel, 1.0f);
        currentZone->velocityLayers.push_back(newLayer);
    }

    repaint();
}

void VelocityLayerEditor::removeLayer(int layerIndex)
{
    if (!currentZone || layerIndex < 0 || layerIndex >= currentZone->velocityLayers.size())
        return;

    currentZone->velocityLayers.erase(currentZone->velocityLayers.begin() + layerIndex);

    // Adjust ranges of remaining layers
    if (!currentZone->velocityLayers.empty())
    {
        currentZone->velocityLayers.front().velocityMin = 0.0f;
        currentZone->velocityLayers.back().velocityMax = 1.0f;
    }

    selectedLayerIndex = -1;
    repaint();
}

float VelocityLayerEditor::velocityToY(float velocity) const
{
    float height = static_cast<float>(getHeight());
    return height * (1.0f - velocity);  // Invert: 1.0 at top, 0.0 at bottom
}

float VelocityLayerEditor::yToVelocity(float y) const
{
    float height = static_cast<float>(getHeight());
    return 1.0f - (y / height);  // Invert: top = 1.0, bottom = 0.0
}

juce::Rectangle<float> VelocityLayerEditor::getLayerRect(int layerIndex) const
{
    if (!currentZone || layerIndex < 0 || layerIndex >= currentZone->velocityLayers.size())
        return {};

    const auto& layer = currentZone->velocityLayers[layerIndex];

    float y1 = velocityToY(layer.velocityMax);
    float y2 = velocityToY(layer.velocityMin);
    float width = static_cast<float>(getWidth());

    return juce::Rectangle<float>(0.0f, y1, width, y2 - y1);
}

int VelocityLayerEditor::findLayerAtY(float y) const
{
    if (!currentZone)
        return -1;

    for (int i = 0; i < currentZone->velocityLayers.size(); ++i)
    {
        if (getLayerRect(i).contains(0.0f, y))
            return i;
    }

    return -1;
}

bool VelocityLayerEditor::isNearBoundary(float y, int& boundaryIndex) const
{
    if (!currentZone)
        return false;

    const float threshold = 5.0f;

    for (int i = 0; i < currentZone->velocityLayers.size() - 1; ++i)
    {
        const auto& layer = currentZone->velocityLayers[i];
        float boundaryY = velocityToY(layer.velocityMin);

        if (std::abs(y - boundaryY) < threshold)
        {
            boundaryIndex = i;
            return true;
        }
    }

    return false;
}

void VelocityLayerEditor::drawVelocityScale(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    g.setColour(juce::Colours::grey);
    g.setFont(10.0f);

    // Draw velocity markers (0, 64, 127)
    for (int vel = 0; vel <= 127; vel += 64)
    {
        float velocity = vel / 127.0f;
        float y = velocityToY(velocity);

        g.drawText(juce::String(vel), 5.0f, y - 6.0f, 30.0f, 12.0f,
                  juce::Justification::centredLeft, false);

        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawLine(40.0f, y, bounds.getWidth(), y, 0.5f);
        g.setColour(juce::Colours::grey);
    }
}

void VelocityLayerEditor::drawLayers(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    if (!currentZone)
        return;

    // Draw layers
    for (int i = 0; i < currentZone->velocityLayers.size(); ++i)
    {
        const auto& layer = currentZone->velocityLayers[i];
        auto layerRect = getLayerRect(i);

        // Layer background
        juce::Colour layerColor = juce::Colour(0xff4a9eff).withRotatedHue(i * 0.2f);
        if (i == selectedLayerIndex)
            layerColor = layerColor.brighter(0.3f);

        g.setColour(layerColor.withAlpha(0.5f));
        g.fillRect(layerRect.reduced(2.0f));

        // Layer border
        g.setColour(layerColor.brighter(0.5f));
        g.drawRect(layerRect.reduced(2.0f), 2.0f);

        // Sample name
        if (sampleLibrary && layer.sampleIndex >= 0 && layer.sampleIndex < sampleLibrary->getNumSamples())
        {
            g.setColour(juce::Colours::white);
            g.setFont(12.0f);
            juce::String text = juce::String::formatted("Sample %d", layer.sampleIndex);
            g.drawText(text, layerRect.reduced(5.0f),
                      juce::Justification::centred, true);
        }

        // Velocity range
        g.setFont(10.0f);
        g.setColour(juce::Colours::lightgrey);
        juce::String rangeText = juce::String::formatted("%d-%d",
            static_cast<int>(layer.velocityMin * 127),
            static_cast<int>(layer.velocityMax * 127));
        g.drawText(rangeText, layerRect.reduced(5.0f).removeFromBottom(15.0f),
                  juce::Justification::centredRight, false);
    }
}
