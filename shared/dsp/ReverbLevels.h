#pragma once

namespace AudioForge {
namespace DSP {

/**
 * Wet and dry levels for juce::dsp::Reverb that behave as a mix control.
 *
 * JUCE's reverb multiplies the dry level it is given by 2 and the wet level
 * by 3. Setting them to (1 - mix) and mix, which looks right, doubles the
 * dry signal and makes the reverb three times too loud. These allow for the
 * scaling: a mix of zero is the dry signal untouched, and a mix of one is
 * the reverb alone at its nominal level.
 */
struct ReverbLevels
{
    float wet;
    float dry;
};

inline ReverbLevels reverbLevelsForMix(float mix)
{
    return { mix / 3.0f, (1.0f - mix) / 2.0f };
}

} // namespace DSP
} // namespace AudioForge
