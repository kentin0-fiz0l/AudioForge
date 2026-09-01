#pragma once

#include <juce_core/juce_core.h>
#include <vector>

namespace PadSynthPresets {

/**
 * Factory Presets for PadSynth
 *
 * 20 presets covering lush pad sounds:
 * - Ambient (7)
 * - Strings (6)
 * - Cinematic (7)
 *
 * Parameters:
 * - waveform: 0=Sine, 1=Saw, 2=Square, 3=Triangle
 * - unisonVoices: 3-9
 * - detuneAmount: 0-50 cents
 * - stereoWidth: 0.0-1.0
 * - filterCutoff: 20-20000 Hz
 * - filterRes: 0.5-10.0
 * - filterType: 0=LP, 1=HP, 2=BP, 3=Notch
 * - attack, decay, sustain, release: ADSR envelope
 */

inline std::vector<juce::String> getFactoryPresets()
{
    return {
        // ===== AMBIENT (7) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Warm Cloud" category="Ambient" author="AudioForge">
    <param id="waveform" value="1"/>
    <param id="unisonVoices" value="7"/>
    <param id="detuneAmount" value="12.0"/>
    <param id="stereoWidth" value="0.9"/>
    <param id="filterCutoff" value="1500.0"/>
    <param id="filterRes" value="0.7"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="1.2"/>
    <param id="decay" value="0.8"/>
    <param id="sustain" value="0.95"/>
    <param id="release" value="2.0"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Deep Space" category="Ambient" author="AudioForge">
    <param id="waveform" value="0"/>
    <param id="unisonVoices" value="9"/>
    <param id="detuneAmount" value="8.0"/>
    <param id="stereoWidth" value="1.0"/>
    <param id="filterCutoff" value="800.0"/>
    <param id="filterRes" value="0.6"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="2.0"/>
    <param id="decay" value="1.0"/>
    <param id="sustain" value="0.9"/>
    <param id="release" value="3.0"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Shimmer Wash" category="Ambient" author="AudioForge">
    <param id="waveform" value="3"/>
    <param id="unisonVoices" value="9"/>
    <param id="detuneAmount" value="15.0"/>
    <param id="stereoWidth" value="0.95"/>
    <param id="filterCutoff" value="3500.0"/>
    <param id="filterRes" value="0.9"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="1.5"/>
    <param id="decay" value="0.6"/>
    <param id="sustain" value="0.92"/>
    <param id="release" value="2.5"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Frozen Tundra" category="Ambient" author="AudioForge">
    <param id="waveform" value="2"/>
    <param id="unisonVoices" value="7"/>
    <param id="detuneAmount" value="20.0"/>
    <param id="stereoWidth" value="0.85"/>
    <param id="filterCutoff" value="1200.0"/>
    <param id="filterRes" value="1.2"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="1.8"/>
    <param id="decay" value="1.2"/>
    <param id="sustain" value="0.88"/>
    <param id="release" value="2.8"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Glass Texture" category="Ambient" author="AudioForge">
    <param id="waveform" value="3"/>
    <param id="unisonVoices" value="9"/>
    <param id="detuneAmount" value="10.0"/>
    <param id="stereoWidth" value="0.9"/>
    <param id="filterCutoff" value="5000.0"/>
    <param id="filterRes" value="0.8"/>
    <param id="filterType" value="1"/>
    <param id="attack" value="1.0"/>
    <param id="decay" value="0.7"/>
    <param id="sustain" value="0.9"/>
    <param id="release" value="2.2"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Evolving Drone" category="Ambient" author="AudioForge">
    <param id="waveform" value="1"/>
    <param id="unisonVoices" value="7"/>
    <param id="detuneAmount" value="18.0"/>
    <param id="stereoWidth" value="0.8"/>
    <param id="filterCutoff" value="2000.0"/>
    <param id="filterRes" value="1.5"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="2.5"/>
    <param id="decay" value="1.5"/>
    <param id="sustain" value="0.95"/>
    <param id="release" value="3.5"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Ethereal Mist" category="Ambient" author="AudioForge">
    <param id="waveform" value="0"/>
    <param id="unisonVoices" value="9"/>
    <param id="detuneAmount" value="6.0"/>
    <param id="stereoWidth" value="1.0"/>
    <param id="filterCutoff" value="2500.0"/>
    <param id="filterRes" value="0.7"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="1.5"/>
    <param id="decay" value="0.9"/>
    <param id="sustain" value="0.93"/>
    <param id="release" value="2.5"/>
</preset>)",

        // ===== STRINGS (6) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Lush Strings" category="Strings" author="AudioForge">
    <param id="waveform" value="1"/>
    <param id="unisonVoices" value="7"/>
    <param id="detuneAmount" value="10.0"/>
    <param id="stereoWidth" value="0.85"/>
    <param id="filterCutoff" value="3000.0"/>
    <param id="filterRes" value="0.8"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="0.8"/>
    <param id="decay" value="0.5"/>
    <param id="sustain" value="0.9"/>
    <param id="release" value="1.5"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Bright Ensemble" category="Strings" author="AudioForge">
    <param id="waveform" value="1"/>
    <param id="unisonVoices" value="9"/>
    <param id="detuneAmount" value="12.0"/>
    <param id="stereoWidth" value="0.9"/>
    <param id="filterCutoff" value="4500.0"/>
    <param id="filterRes" value="0.9"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="0.6"/>
    <param id="decay" value="0.4"/>
    <param id="sustain" value="0.88"/>
    <param id="release" value="1.2"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Chamber Section" category="Strings" author="AudioForge">
    <param id="waveform" value="1"/>
    <param id="unisonVoices" value="7"/>
    <param id="detuneAmount" value="8.0"/>
    <param id="stereoWidth" value="0.7"/>
    <param id="filterCutoff" value="2500.0"/>
    <param id="filterRes" value="0.7"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="0.7"/>
    <param id="decay" value="0.6"/>
    <param id="sustain" value="0.85"/>
    <param id="release" value="1.0"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Warm Violins" category="Strings" author="AudioForge">
    <param id="waveform" value="1"/>
    <param id="unisonVoices" value="7"/>
    <param id="detuneAmount" value="9.0"/>
    <param id="stereoWidth" value="0.75"/>
    <param id="filterCutoff" value="3500.0"/>
    <param id="filterRes" value="1.0"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="0.5"/>
    <param id="decay" value="0.5"/>
    <param id="sustain" value="0.87"/>
    <param id="release" value="1.3"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Sweeping Cello" category="Strings" author="AudioForge">
    <param id="waveform" value="1"/>
    <param id="unisonVoices" value="5"/>
    <param id="detuneAmount" value="7.0"/>
    <param id="stereoWidth" value="0.6"/>
    <param id="filterCutoff" value="1800.0"/>
    <param id="filterRes" value="0.9"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="0.9"/>
    <param id="decay" value="0.7"/>
    <param id="sustain" value="0.9"/>
    <param id="release" value="1.6"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Synthetic Orchestra" category="Strings" author="AudioForge">
    <param id="waveform" value="1"/>
    <param id="unisonVoices" value="9"/>
    <param id="detuneAmount" value="14.0"/>
    <param id="stereoWidth" value="0.95"/>
    <param id="filterCutoff" value="4000.0"/>
    <param id="filterRes" value="0.8"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="0.7"/>
    <param id="decay" value="0.6"/>
    <param id="sustain" value="0.92"/>
    <param id="release" value="1.8"/>
</preset>)",

        // ===== CINEMATIC (7) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Epic Rise" category="Cinematic" author="AudioForge">
    <param id="waveform" value="1"/>
    <param id="unisonVoices" value="9"/>
    <param id="detuneAmount" value="15.0"/>
    <param id="stereoWidth" value="1.0"/>
    <param id="filterCutoff" value="3500.0"/>
    <param id="filterRes" value="1.2"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="2.0"/>
    <param id="decay" value="1.0"/>
    <param id="sustain" value="0.95"/>
    <param id="release" value="2.5"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Dark Tension" category="Cinematic" author="AudioForge">
    <param id="waveform" value="2"/>
    <param id="unisonVoices" value="7"/>
    <param id="detuneAmount" value="20.0"/>
    <param id="stereoWidth" value="0.85"/>
    <param id="filterCutoff" value="600.0"/>
    <param id="filterRes" value="1.5"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="1.5"/>
    <param id="decay" value="1.2"/>
    <param id="sustain" value="0.88"/>
    <param id="release" value="2.0"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Heroic Theme" category="Cinematic" author="AudioForge">
    <param id="waveform" value="1"/>
    <param id="unisonVoices" value="9"/>
    <param id="detuneAmount" value="11.0"/>
    <param id="stereoWidth" value="0.9"/>
    <param id="filterCutoff" value="5000.0"/>
    <param id="filterRes" value="0.9"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="0.8"/>
    <param id="decay" value="0.6"/>
    <param id="sustain" value="0.92"/>
    <param id="release" value="1.8"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Suspense Bed" category="Cinematic" author="AudioForge">
    <param id="waveform" value="3"/>
    <param id="unisonVoices" value="7"/>
    <param id="detuneAmount" value="18.0"/>
    <param id="stereoWidth" value="0.95"/>
    <param id="filterCutoff" value="1200.0"/>
    <param id="filterRes" value="2.0"/>
    <param id="filterType" value="2"/>
    <param id="attack" value="2.5"/>
    <param id="decay" value="1.5"/>
    <param id="sustain" value="0.85"/>
    <param id="release" value="3.0"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Hope & Wonder" category="Cinematic" author="AudioForge">
    <param id="waveform" value="3"/>
    <param id="unisonVoices" value="9"/>
    <param id="detuneAmount" value="10.0"/>
    <param id="stereoWidth" value="1.0"/>
    <param id="filterCutoff" value="4000.0"/>
    <param id="filterRes" value="0.8"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="1.2"/>
    <param id="decay" value="0.8"/>
    <param id="sustain" value="0.93"/>
    <param id="release" value="2.2"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Emotional Swell" category="Cinematic" author="AudioForge">
    <param id="waveform" value="1"/>
    <param id="unisonVoices" value="9"/>
    <param id="detuneAmount" value="13.0"/>
    <param id="stereoWidth" value="0.9"/>
    <param id="filterCutoff" value="3000.0"/>
    <param id="filterRes" value="1.0"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="1.8"/>
    <param id="decay" value="1.0"/>
    <param id="sustain" value="0.9"/>
    <param id="release" value="2.8"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Vast Landscape" category="Cinematic" author="AudioForge">
    <param id="waveform" value="0"/>
    <param id="unisonVoices" value="9"/>
    <param id="detuneAmount" value="8.0"/>
    <param id="stereoWidth" value="1.0"/>
    <param id="filterCutoff" value="2200.0"/>
    <param id="filterRes" value="0.7"/>
    <param id="filterType" value="0"/>
    <param id="attack" value="2.2"/>
    <param id="decay" value="1.2"/>
    <param id="sustain" value="0.94"/>
    <param id="release" value="3.2"/>
</preset>)"
    };
}

} // namespace PadSynthPresets
