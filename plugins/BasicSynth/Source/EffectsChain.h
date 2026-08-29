#pragma once

#include "Chorus.h"
#include "Reverb.h"
#include "Saturation.h"

/**
 * Effects Processing Chain
 *
 * Manages the complete effects chain for BasicSynth:
 * Input → Saturation → Chorus → Reverb → Output
 *
 * Provides stereo processing with individual effect bypass and mix controls.
 */
class EffectsChain
{
public:
    EffectsChain();

    /**
     * Prepare effects chain for playback.
     *
     * @param sampleRate Sample rate in Hz
     * @param maxBlockSize Maximum expected block size
     */
    void prepare(double sampleRate, int maxBlockSize);

    /**
     * Process one mono sample through the effects chain, output stereo.
     *
     * @param input Mono input sample
     * @param leftOut Left channel output
     * @param rightOut Right channel output
     */
    void processSample(float input, float& leftOut, float& rightOut);

    /**
     * Set saturation parameters.
     *
     * @param drive Drive amount (0.0-1.0)
     * @param mix Dry/wet mix (0.0-1.0)
     * @param type Saturation type (0=soft, 1=hard, 2=tube)
     */
    void setSaturation(float drive, float mix, int type);

    /**
     * Set chorus parameters.
     *
     * @param rate LFO rate in Hz (0.1-10.0)
     * @param depth Modulation depth (0.0-1.0)
     * @param mix Dry/wet mix (0.0-1.0)
     */
    void setChorus(float rate, float depth, float mix);

    /**
     * Set reverb parameters.
     *
     * @param roomSize Room size (0.0-1.0)
     * @param damping High frequency damping (0.0-1.0)
     * @param mix Dry/wet mix (0.0-1.0)
     */
    void setReverb(float roomSize, float damping, float mix);

    /**
     * Reset all effects (clear delay buffers, etc).
     */
    void reset();

private:
    // Individual effects (signal flow order)
    Saturation saturation;
    Chorus chorus;
    Reverb reverb;
};
