#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay()
{
    setSize(800, 200);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.fillAll(juce::Colour(0xff1a1a1a));

    if (currentSample)
    {
        drawWaveform(g, bounds);
        drawLoopMarkers(g, bounds);
        drawSampleInfo(g, bounds.removeFromTop(20.0f));
    }
    else
    {
        // No sample loaded - show message
        g.setColour(juce::Colours::grey);
        g.setFont(16.0f);
        g.drawText("No sample loaded - drag & drop audio file here",
                  bounds, juce::Justification::centred, true);
    }

    // Border
    g.setColour(juce::Colours::darkgrey);
    g.drawRect(getLocalBounds().toFloat(), 1.0f);
}

void WaveformDisplay::resized()
{
}

void WaveformDisplay::mouseDown(const juce::MouseEvent& e)
{
    if (!currentSample)
        return;

    auto bounds = getLocalBounds().toFloat().reduced(5.0f);

    bool isStart;
    if (isPointInLoopMarker(e.position, bounds, isStart))
    {
        if (isStart)
            isDraggingLoopStart = true;
        else
            isDraggingLoopEnd = true;
    }
}

void WaveformDisplay::mouseDrag(const juce::MouseEvent& e)
{
    if (!currentSample)
        return;

    auto bounds = getLocalBounds().toFloat().reduced(5.0f);
    double newSeconds = xToSeconds(e.position.x, bounds);
    double lengthSeconds = currentSample->getLengthInSamples() / currentSample->getSampleRate();

    newSeconds = juce::jlimit(0.0, lengthSeconds, newSeconds);

    if (isDraggingLoopStart)
    {
        double loopEnd = getLoopEnd();
        newSeconds = juce::jmin(newSeconds, loopEnd - 0.01);
        setLoopStart(newSeconds);

        listeners.call([this, newSeconds](Listener& l)
        {
            l.loopPointsChanged(newSeconds, getLoopEnd());
        });
    }
    else if (isDraggingLoopEnd)
    {
        double loopStart = getLoopStart();
        newSeconds = juce::jmax(newSeconds, loopStart + 0.01);
        setLoopEnd(newSeconds);

        listeners.call([this, newSeconds](Listener& l)
        {
            l.loopPointsChanged(getLoopStart(), newSeconds);
        });
    }

    repaint();
}

void WaveformDisplay::mouseUp(const juce::MouseEvent& e)
{
    isDraggingLoopStart = false;
    isDraggingLoopEnd = false;
}

void WaveformDisplay::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    // Horizontal zoom with wheel
    if (e.mods.isCommandDown())
    {
        float zoomDelta = wheel.deltaY * 0.5f;
        zoomLevel = juce::jlimit(0.1f, 10.0f, zoomLevel + zoomDelta);
        repaint();
    }
    // Vertical zoom with shift+wheel
    else if (e.mods.isShiftDown())
    {
        float zoomDelta = wheel.deltaY * 0.5f;
        verticalZoom = juce::jlimit(0.1f, 10.0f, verticalZoom + zoomDelta);
        repaint();
    }
}

void WaveformDisplay::setSample(std::shared_ptr<Sample> sample)
{
    currentSample = sample;

    if (currentSample)
    {
        // Reset zoom to fit
        zoomLevel = 1.0f;
        verticalZoom = 1.0f;
        scrollPosition = 0.0f;
    }

    repaint();
}

void WaveformDisplay::setLoopStart(double seconds)
{
    if (currentSample)
    {
        double lengthSeconds = currentSample->getLengthInSamples() / currentSample->getSampleRate();
        currentSample->setLoopPoints(seconds, getLoopEnd());
    }
}

void WaveformDisplay::setLoopEnd(double seconds)
{
    if (currentSample)
    {
        currentSample->setLoopPoints(getLoopStart(), seconds);
    }
}

double WaveformDisplay::getLoopStart() const
{
    if (currentSample)
    {
        return currentSample->getLoopStartSamples() / currentSample->getSampleRate();
    }
    return 0.0;
}

double WaveformDisplay::getLoopEnd() const
{
    if (currentSample)
    {
        return currentSample->getLoopEndSamples() / currentSample->getSampleRate();
    }
    return 0.0;
}

void WaveformDisplay::drawWaveform(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    const auto& audioData = currentSample->getAudioData();
    int numSamples = currentSample->getLengthInSamples();

    if (numSamples == 0)
        return;

    bounds = bounds.reduced(5.0f);
    float width = bounds.getWidth();
    float height = bounds.getHeight();
    float centerY = bounds.getCentreY();

    // Calculate samples per pixel
    int samplesPerPixel = juce::jmax(1, static_cast<int>(numSamples / width / zoomLevel));

    // Draw waveform (left channel)
    juce::Path waveformPath;
    bool firstPoint = true;

    for (float x = 0.0f; x < width; x += 1.0f)
    {
        int sampleIndex = static_cast<int>((x / width) * numSamples);

        if (sampleIndex >= numSamples)
            break;

        // Find min/max in this pixel range
        float minVal = 0.0f;
        float maxVal = 0.0f;

        for (int i = 0; i < samplesPerPixel && (sampleIndex + i) < numSamples; ++i)
        {
            float sample = audioData.getSample(0, sampleIndex + i);
            minVal = juce::jmin(minVal, sample);
            maxVal = juce::jmax(maxVal, sample);
        }

        // Scale to display height
        float minY = centerY - (minVal * height * 0.4f * verticalZoom);
        float maxY = centerY - (maxVal * height * 0.4f * verticalZoom);

        // Draw vertical line for this pixel
        if (firstPoint)
        {
            waveformPath.startNewSubPath(bounds.getX() + x, minY);
            firstPoint = false;
        }

        waveformPath.lineTo(bounds.getX() + x, minY);
        waveformPath.lineTo(bounds.getX() + x, maxY);
    }

    g.setColour(juce::Colour(0xff4a9eff));
    g.strokePath(waveformPath, juce::PathStrokeType(1.0f));

    // Draw center line
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    g.drawLine(bounds.getX(), centerY, bounds.getRight(), centerY, 0.5f);
}

void WaveformDisplay::drawLoopMarkers(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    bounds = bounds.reduced(5.0f);

    double loopStart = getLoopStart();
    double loopEnd = getLoopEnd();

    if (loopStart <= 0.0 && loopEnd <= 0.0)
        return;

    // Draw loop region
    float startX = secondsToX(loopStart, bounds);
    float endX = secondsToX(loopEnd, bounds);

    juce::Rectangle<float> loopRegion(startX, bounds.getY(),
                                     endX - startX, bounds.getHeight());

    g.setColour(juce::Colours::yellow.withAlpha(0.1f));
    g.fillRect(loopRegion);

    // Draw loop markers (draggable handles)
    auto startMarker = getLoopMarkerRect(loopStart, bounds, true);
    auto endMarker = getLoopMarkerRect(loopEnd, bounds, false);

    g.setColour(juce::Colours::yellow);
    g.fillRect(startMarker);
    g.fillRect(endMarker);

    // Draw labels
    g.setFont(10.0f);
    g.setColour(juce::Colours::white);
    g.drawText("START", startMarker.expanded(0.0f, -2.0f),
              juce::Justification::centredBottom, false);
    g.drawText("END", endMarker.expanded(0.0f, -2.0f),
              juce::Justification::centredBottom, false);
}

void WaveformDisplay::drawSampleInfo(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    g.setColour(juce::Colours::lightgrey);
    g.setFont(12.0f);

    double lengthSeconds = currentSample->getLengthInSamples() / currentSample->getSampleRate();
    int lengthSamples = currentSample->getLengthInSamples();
    double sampleRate = currentSample->getSampleRate();
    int channels = currentSample->getNumChannels();

    juce::String info = juce::String::formatted("%.2fs | %d samples | %.1f kHz | %d ch",
                                                lengthSeconds, lengthSamples,
                                                sampleRate / 1000.0, channels);

    g.drawText(info, bounds.reduced(5.0f, 0.0f),
              juce::Justification::centredLeft, false);
}

float WaveformDisplay::secondsToX(double seconds, juce::Rectangle<float> bounds) const
{
    if (!currentSample)
        return bounds.getX();

    double lengthSeconds = currentSample->getLengthInSamples() / currentSample->getSampleRate();
    float normalised = static_cast<float>(seconds / lengthSeconds);
    return bounds.getX() + normalised * bounds.getWidth();
}

double WaveformDisplay::xToSeconds(float x, juce::Rectangle<float> bounds) const
{
    if (!currentSample)
        return 0.0;

    double lengthSeconds = currentSample->getLengthInSamples() / currentSample->getSampleRate();
    float normalised = (x - bounds.getX()) / bounds.getWidth();
    return normalised * lengthSeconds;
}

juce::Rectangle<float> WaveformDisplay::getLoopMarkerRect(double seconds,
                                                          juce::Rectangle<float> bounds,
                                                          bool isStart) const
{
    float x = secondsToX(seconds, bounds);
    float markerWidth = 6.0f;
    float markerHeight = 20.0f;

    float markerX = isStart ? x - markerWidth : x;
    float markerY = bounds.getY();

    return juce::Rectangle<float>(markerX, markerY, markerWidth, markerHeight);
}

bool WaveformDisplay::isPointInLoopMarker(juce::Point<float> point,
                                          juce::Rectangle<float> bounds,
                                          bool& isStart) const
{
    if (!currentSample)
        return false;

    double loopStart = getLoopStart();
    double loopEnd = getLoopEnd();

    auto startMarker = getLoopMarkerRect(loopStart, bounds.reduced(5.0f), true);
    auto endMarker = getLoopMarkerRect(loopEnd, bounds.reduced(5.0f), false);

    if (startMarker.contains(point))
    {
        isStart = true;
        return true;
    }

    if (endMarker.contains(point))
    {
        isStart = false;
        return true;
    }

    return false;
}
