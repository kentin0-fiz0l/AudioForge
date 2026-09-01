#pragma once

#include <juce_core/juce_core.h>
#include <vector>

namespace DrumSynthPresets
{

/**
 * Factory Presets for DrumSynth
 *
 * 30 presets covering classic and modern drum sounds:
 * - 808 Kit (7 presets)
 * - 909 Kit (7 presets)
 * - Acoustic (8 presets)
 * - Electronic (8 presets)
 *
 * Parameters per drum module:
 * Kick: pitch (30-100 Hz), decay (0.05-2.0s), click (0-1), tone (0-1), drive (0-1)
 * Snare: tune (0-1), snap (0-1), tone (0-1), decay (0.05-1.0s), mix (0-1)
 * Hi-Hat: tune (0-1), decay (0.01-0.5s), tone (0-1), click (0-1)
 */

inline std::vector<juce::String> getFactoryPresets()
{
    return {
        // ===== 808 KIT (7) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Init" category="808 Kit" author="AudioForge">
    <param id="kickPitch" value="50.0"/>
    <param id="kickDecay" value="0.4"/>
    <param id="kickClick" value="0.3"/>
    <param id="kickTone" value="0.8"/>
    <param id="kickDrive" value="0.2"/>
    <param id="snareTune" value="0.5"/>
    <param id="snareSnap" value="0.4"/>
    <param id="snareTone" value="0.6"/>
    <param id="snareDecay" value="0.2"/>
    <param id="snareMix" value="0.7"/>
    <param id="hihatTune" value="0.5"/>
    <param id="hihatDecay" value="0.1"/>
    <param id="hihatTone" value="0.7"/>
    <param id="hihatClick" value="0.5"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="808 Deep Kick" category="808 Kit" author="AudioForge">
    <param id="kickPitch" value="35.0"/>
    <param id="kickDecay" value="0.8"/>
    <param id="kickClick" value="0.2"/>
    <param id="kickTone" value="0.9"/>
    <param id="kickDrive" value="0.15"/>
    <param id="snareTune" value="0.45"/>
    <param id="snareSnap" value="0.5"/>
    <param id="snareTone" value="0.5"/>
    <param id="snareDecay" value="0.25"/>
    <param id="snareMix" value="0.75"/>
    <param id="hihatTune" value="0.5"/>
    <param id="hihatDecay" value="0.08"/>
    <param id="hihatTone" value="0.6"/>
    <param id="hihatClick" value="0.4"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="808 Punchy" category="808 Kit" author="AudioForge">
    <param id="kickPitch" value="55.0"/>
    <param id="kickDecay" value="0.25"/>
    <param id="kickClick" value="0.6"/>
    <param id="kickTone" value="0.7"/>
    <param id="kickDrive" value="0.35"/>
    <param id="snareTune" value="0.55"/>
    <param id="snareSnap" value="0.7"/>
    <param id="snareTone" value="0.65"/>
    <param id="snareDecay" value="0.15"/>
    <param id="snareMix" value="0.65"/>
    <param id="hihatTune" value="0.6"/>
    <param id="hihatDecay" value="0.06"/>
    <param id="hihatTone" value="0.75"/>
    <param id="hihatClick" value="0.65"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="808 Tight" category="808 Kit" author="AudioForge">
    <param id="kickPitch" value="60.0"/>
    <param id="kickDecay" value="0.2"/>
    <param id="kickClick" value="0.4"/>
    <param id="kickTone" value="0.6"/>
    <param id="kickDrive" value="0.25"/>
    <param id="snareTune" value="0.6"/>
    <param id="snareSnap" value="0.6"/>
    <param id="snareTone" value="0.7"/>
    <param id="snareDecay" value="0.12"/>
    <param id="snareMix" value="0.6"/>
    <param id="hihatTune" value="0.55"/>
    <param id="hihatDecay" value="0.05"/>
    <param id="hihatTone" value="0.8"/>
    <param id="hihatClick" value="0.55"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="808 Boom" category="808 Kit" author="AudioForge">
    <param id="kickPitch" value="40.0"/>
    <param id="kickDecay" value="1.2"/>
    <param id="kickClick" value="0.25"/>
    <param id="kickTone" value="0.85"/>
    <param id="kickDrive" value="0.1"/>
    <param id="snareTune" value="0.4"/>
    <param id="snareSnap" value="0.35"/>
    <param id="snareTone" value="0.45"/>
    <param id="snareDecay" value="0.3"/>
    <param id="snareMix" value="0.8"/>
    <param id="hihatTune" value="0.45"/>
    <param id="hihatDecay" value="0.12"/>
    <param id="hihatTone" value="0.65"/>
    <param id="hihatClick" value="0.45"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="808 Lo-Fi" category="808 Kit" author="AudioForge">
    <param id="kickPitch" value="52.0"/>
    <param id="kickDecay" value="0.35"/>
    <param id="kickClick" value="0.5"/>
    <param id="kickTone" value="0.5"/>
    <param id="kickDrive" value="0.65"/>
    <param id="snareTune" value="0.5"/>
    <param id="snareSnap" value="0.5"/>
    <param id="snareTone" value="0.4"/>
    <param id="snareDecay" value="0.18"/>
    <param id="snareMix" value="0.7"/>
    <param id="hihatTune" value="0.5"/>
    <param id="hihatDecay" value="0.09"/>
    <param id="hihatTone" value="0.5"/>
    <param id="hihatClick" value="0.6"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="808 Crispy" category="808 Kit" author="AudioForge">
    <param id="kickPitch" value="48.0"/>
    <param id="kickDecay" value="0.45"/>
    <param id="kickClick" value="0.35"/>
    <param id="kickTone" value="0.75"/>
    <param id="kickDrive" value="0.2"/>
    <param id="snareTune" value="0.65"/>
    <param id="snareSnap" value="0.65"/>
    <param id="snareTone" value="0.75"/>
    <param id="snareDecay" value="0.22"/>
    <param id="snareMix" value="0.65"/>
    <param id="hihatTune" value="0.7"/>
    <param id="hihatDecay" value="0.08"/>
    <param id="hihatTone" value="0.9"/>
    <param id="hihatClick" value="0.7"/>
</preset>)",

        // ===== 909 KIT (7) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="909 Classic" category="909 Kit" author="AudioForge">
    <param id="kickPitch" value="65.0"/>
    <param id="kickDecay" value="0.3"/>
    <param id="kickClick" value="0.5"/>
    <param id="kickTone" value="0.6"/>
    <param id="kickDrive" value="0.3"/>
    <param id="snareTune" value="0.6"/>
    <param id="snareSnap" value="0.6"/>
    <param id="snareTone" value="0.7"/>
    <param id="snareDecay" value="0.18"/>
    <param id="snareMix" value="0.6"/>
    <param id="hihatTune" value="0.65"/>
    <param id="hihatDecay" value="0.06"/>
    <param id="hihatTone" value="0.8"/>
    <param id="hihatClick" value="0.6"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="909 Hard" category="909 Kit" author="AudioForge">
    <param id="kickPitch" value="70.0"/>
    <param id="kickDecay" value="0.25"/>
    <param id="kickClick" value="0.7"/>
    <param id="kickTone" value="0.5"/>
    <param id="kickDrive" value="0.5"/>
    <param id="snareTune" value="0.7"/>
    <param id="snareSnap" value="0.8"/>
    <param id="snareTone" value="0.75"/>
    <param id="snareDecay" value="0.14"/>
    <param id="snareMix" value="0.55"/>
    <param id="hihatTune" value="0.7"/>
    <param id="hihatDecay" value="0.05"/>
    <param id="hihatTone" value="0.85"/>
    <param id="hihatClick" value="0.75"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="909 Soft" category="909 Kit" author="AudioForge">
    <param id="kickPitch" value="58.0"/>
    <param id="kickDecay" value="0.35"/>
    <param id="kickClick" value="0.3"/>
    <param id="kickTone" value="0.7"/>
    <param id="kickDrive" value="0.15"/>
    <param id="snareTune" value="0.5"/>
    <param id="snareSnap" value="0.35"/>
    <param id="snareTone" value="0.6"/>
    <param id="snareDecay" value="0.22"/>
    <param id="snareMix" value="0.7"/>
    <param id="hihatTune" value="0.55"/>
    <param id="hihatDecay" value="0.08"/>
    <param id="hihatTone" value="0.7"/>
    <param id="hihatClick" value="0.45"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="909 Techno" category="909 Kit" author="AudioForge">
    <param id="kickPitch" value="68.0"/>
    <param id="kickDecay" value="0.28"/>
    <param id="kickClick" value="0.6"/>
    <param id="kickTone" value="0.55"/>
    <param id="kickDrive" value="0.4"/>
    <param id="snareTune" value="0.65"/>
    <param id="snareSnap" value="0.7"/>
    <param id="snareTone" value="0.8"/>
    <param id="snareDecay" value="0.16"/>
    <param id="snareMix" value="0.5"/>
    <param id="hihatTune" value="0.7"/>
    <param id="hihatDecay" value="0.05"/>
    <param id="hihatTone" value="0.9"/>
    <param id="hihatClick" value="0.7"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="909 House" category="909 Kit" author="AudioForge">
    <param id="kickPitch" value="62.0"/>
    <param id="kickDecay" value="0.32"/>
    <param id="kickClick" value="0.45"/>
    <param id="kickTone" value="0.65"/>
    <param id="kickDrive" value="0.25"/>
    <param id="snareTune" value="0.58"/>
    <param id="snareSnap" value="0.55"/>
    <param id="snareTone" value="0.68"/>
    <param id="snareDecay" value="0.19"/>
    <param id="snareMix" value="0.65"/>
    <param id="hihatTune" value="0.6"/>
    <param id="hihatDecay" value="0.07"/>
    <param id="hihatTone" value="0.75"/>
    <param id="hihatClick" value="0.55"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="909 Tight" category="909 Kit" author="AudioForge">
    <param id="kickPitch" value="72.0"/>
    <param id="kickDecay" value="0.18"/>
    <param id="kickClick" value="0.55"/>
    <param id="kickTone" value="0.5"/>
    <param id="kickDrive" value="0.35"/>
    <param id="snareTune" value="0.7"/>
    <param id="snareSnap" value="0.7"/>
    <param id="snareTone" value="0.75"/>
    <param id="snareDecay" value="0.12"/>
    <param id="snareMix" value="0.55"/>
    <param id="hihatTune" value="0.65"/>
    <param id="hihatDecay" value="0.04"/>
    <param id="hihatTone" value="0.85"/>
    <param id="hihatClick" value="0.65"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="909 Vintage" category="909 Kit" author="AudioForge">
    <param id="kickPitch" value="60.0"/>
    <param id="kickDecay" value="0.4"/>
    <param id="kickClick" value="0.4"/>
    <param id="kickTone" value="0.75"/>
    <param id="kickDrive" value="0.2"/>
    <param id="snareTune" value="0.55"/>
    <param id="snareSnap" value="0.5"/>
    <param id="snareTone" value="0.6"/>
    <param id="snareDecay" value="0.2"/>
    <param id="snareMix" value="0.7"/>
    <param id="hihatTune" value="0.5"/>
    <param id="hihatDecay" value="0.09"/>
    <param id="hihatTone" value="0.65"/>
    <param id="hihatClick" value="0.5"/>
</preset>)",

        // ===== ACOUSTIC (8) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Acoustic Rock" category="Acoustic" author="AudioForge">
    <param id="kickPitch" value="45.0"/>
    <param id="kickDecay" value="0.5"/>
    <param id="kickClick" value="0.5"/>
    <param id="kickTone" value="0.6"/>
    <param id="kickDrive" value="0.3"/>
    <param id="snareTune" value="0.45"/>
    <param id="snareSnap" value="0.55"/>
    <param id="snareTone" value="0.5"/>
    <param id="snareDecay" value="0.25"/>
    <param id="snareMix" value="0.75"/>
    <param id="hihatTune" value="0.45"/>
    <param id="hihatDecay" value="0.12"/>
    <param id="hihatTone" value="0.6"/>
    <param id="hihatClick" value="0.5"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Acoustic Jazz" category="Acoustic" author="AudioForge">
    <param id="kickPitch" value="42.0"/>
    <param id="kickDecay" value="0.6"/>
    <param id="kickClick" value="0.25"/>
    <param id="kickTone" value="0.7"/>
    <param id="kickDrive" value="0.1"/>
    <param id="snareTune" value="0.55"/>
    <param id="snareSnap" value="0.3"/>
    <param id="snareTone" value="0.55"/>
    <param id="snareDecay" value="0.3"/>
    <param id="snareMix" value="0.8"/>
    <param id="hihatTune" value="0.5"/>
    <param id="hihatDecay" value="0.15"/>
    <param id="hihatTone" value="0.65"/>
    <param id="hihatClick" value="0.35"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Acoustic Studio" category="Acoustic" author="AudioForge">
    <param id="kickPitch" value="48.0"/>
    <param id="kickDecay" value="0.45"/>
    <param id="kickClick" value="0.4"/>
    <param id="kickTone" value="0.65"/>
    <param id="kickDrive" value="0.2"/>
    <param id="snareTune" value="0.5"/>
    <param id="snareSnap" value="0.45"/>
    <param id="snareTone" value="0.6"/>
    <param id="snareDecay" value="0.22"/>
    <param id="snareMix" value="0.75"/>
    <param id="hihatTune" value="0.5"/>
    <param id="hihatDecay" value="0.1"/>
    <param id="hihatTone" value="0.7"/>
    <param id="hihatClick" value="0.5"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Acoustic Live" category="Acoustic" author="AudioForge">
    <param id="kickPitch" value="50.0"/>
    <param id="kickDecay" value="0.35"/>
    <param id="kickClick" value="0.6"/>
    <param id="kickTone" value="0.55"/>
    <param id="kickDrive" value="0.35"/>
    <param id="snareTune" value="0.55"/>
    <param id="snareSnap" value="0.65"/>
    <param id="snareTone" value="0.65"/>
    <param id="snareDecay" value="0.18"/>
    <param id="snareMix" value="0.7"/>
    <param id="hihatTune" value="0.55"/>
    <param id="hihatDecay" value="0.08"/>
    <param id="hihatTone" value="0.75"/>
    <param id="hihatClick" value="0.6"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Acoustic Tight" category="Acoustic" author="AudioForge">
    <param id="kickPitch" value="55.0"/>
    <param id="kickDecay" value="0.28"/>
    <param id="kickClick" value="0.5"/>
    <param id="kickTone" value="0.6"/>
    <param id="kickDrive" value="0.25"/>
    <param id="snareTune" value="0.6"/>
    <param id="snareSnap" value="0.6"/>
    <param id="snareTone" value="0.7"/>
    <param id="snareDecay" value="0.15"/>
    <param id="snareMix" value="0.65"/>
    <param id="hihatTune" value="0.6"/>
    <param id="hihatDecay" value="0.06"/>
    <param id="hihatTone" value="0.8"/>
    <param id="hihatClick" value="0.55"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Acoustic Deep" category="Acoustic" author="AudioForge">
    <param id="kickPitch" value="38.0"/>
    <param id="kickDecay" value="0.7"/>
    <param id="kickClick" value="0.3"/>
    <param id="kickTone" value="0.8"/>
    <param id="kickDrive" value="0.15"/>
    <param id="snareTune" value="0.35"/>
    <param id="snareSnap" value="0.4"/>
    <param id="snareTone" value="0.45"/>
    <param id="snareDecay" value="0.28"/>
    <param id="snareMix" value="0.8"/>
    <param id="hihatTune" value="0.4"/>
    <param id="hihatDecay" value="0.14"/>
    <param id="hihatTone" value="0.55"/>
    <param id="hihatClick" value="0.4"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Acoustic Bright" category="Acoustic" author="AudioForge">
    <param id="kickPitch" value="58.0"/>
    <param id="kickDecay" value="0.35"/>
    <param id="kickClick" value="0.55"/>
    <param id="kickTone" value="0.5"/>
    <param id="kickDrive" value="0.3"/>
    <param id="snareTune" value="0.65"/>
    <param id="snareSnap" value="0.65"/>
    <param id="snareTone" value="0.75"/>
    <param id="snareDecay" value="0.2"/>
    <param id="snareMix" value="0.7"/>
    <param id="hihatTune" value="0.7"/>
    <param id="hihatDecay" value="0.08"/>
    <param id="hihatTone" value="0.85"/>
    <param id="hihatClick" value="0.65"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Acoustic Ambient" category="Acoustic" author="AudioForge">
    <param id="kickPitch" value="44.0"/>
    <param id="kickDecay" value="0.9"/>
    <param id="kickClick" value="0.2"/>
    <param id="kickTone" value="0.75"/>
    <param id="kickDrive" value="0.1"/>
    <param id="snareTune" value="0.4"/>
    <param id="snareSnap" value="0.25"/>
    <param id="snareTone" value="0.5"/>
    <param id="snareDecay" value="0.4"/>
    <param id="snareMix" value="0.85"/>
    <param id="hihatTune" value="0.45"/>
    <param id="hihatDecay" value="0.2"/>
    <param id="hihatTone" value="0.6"/>
    <param id="hihatClick" value="0.3"/>
</preset>)",

        // ===== ELECTRONIC (8) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Electronic Club" category="Electronic" author="AudioForge">
    <param id="kickPitch" value="65.0"/>
    <param id="kickDecay" value="0.35"/>
    <param id="kickClick" value="0.5"/>
    <param id="kickTone" value="0.5"/>
    <param id="kickDrive" value="0.45"/>
    <param id="snareTune" value="0.65"/>
    <param id="snareSnap" value="0.7"/>
    <param id="snareTone" value="0.8"/>
    <param id="snareDecay" value="0.15"/>
    <param id="snareMix" value="0.5"/>
    <param id="hihatTune" value="0.7"/>
    <param id="hihatDecay" value="0.05"/>
    <param id="hihatTone" value="0.9"/>
    <param id="hihatClick" value="0.7"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Electronic Bass" category="Electronic" author="AudioForge">
    <param id="kickPitch" value="32.0"/>
    <param id="kickDecay" value="1.0"/>
    <param id="kickClick" value="0.3"/>
    <param id="kickTone" value="0.9"/>
    <param id="kickDrive" value="0.5"/>
    <param id="snareTune" value="0.4"/>
    <param id="snareSnap" value="0.45"/>
    <param id="snareTone" value="0.4"/>
    <param id="snareDecay" value="0.2"/>
    <param id="snareMix" value="0.75"/>
    <param id="hihatTune" value="0.5"/>
    <param id="hihatDecay" value="0.07"/>
    <param id="hihatTone" value="0.65"/>
    <param id="hihatClick" value="0.5"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Electronic Minimal" category="Electronic" author="AudioForge">
    <param id="kickPitch" value="70.0"/>
    <param id="kickDecay" value="0.22"/>
    <param id="kickClick" value="0.6"/>
    <param id="kickTone" value="0.45"/>
    <param id="kickDrive" value="0.3"/>
    <param id="snareTune" value="0.7"/>
    <param id="snareSnap" value="0.75"/>
    <param id="snareTone" value="0.85"/>
    <param id="snareDecay" value="0.12"/>
    <param id="snareMix" value="0.45"/>
    <param id="hihatTune" value="0.75"/>
    <param id="hihatDecay" value="0.04"/>
    <param id="hihatTone" value="0.95"/>
    <param id="hihatClick" value="0.75"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Electronic Glitch" category="Electronic" author="AudioForge">
    <param id="kickPitch" value="80.0"/>
    <param id="kickDecay" value="0.15"/>
    <param id="kickClick" value="0.8"/>
    <param id="kickTone" value="0.3"/>
    <param id="kickDrive" value="0.7"/>
    <param id="snareTune" value="0.8"/>
    <param id="snareSnap" value="0.85"/>
    <param id="snareTone" value="0.9"/>
    <param id="snareDecay" value="0.1"/>
    <param id="snareMix" value="0.4"/>
    <param id="hihatTune" value="0.85"/>
    <param id="hihatDecay" value="0.03"/>
    <param id="hihatTone" value="0.95"/>
    <param id="hihatClick" value="0.85"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Electronic Hard" category="Electronic" author="AudioForge">
    <param id="kickPitch" value="75.0"/>
    <param id="kickDecay" value="0.25"/>
    <param id="kickClick" value="0.75"/>
    <param id="kickTone" value="0.4"/>
    <param id="kickDrive" value="0.65"/>
    <param id="snareTune" value="0.75"/>
    <param id="snareSnap" value="0.85"/>
    <param id="snareTone" value="0.85"/>
    <param id="snareDecay" value="0.13"/>
    <param id="snareMix" value="0.5"/>
    <param id="hihatTune" value="0.75"/>
    <param id="hihatDecay" value="0.04"/>
    <param id="hihatTone" value="0.9"/>
    <param id="hihatClick" value="0.8"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Electronic Soft" category="Electronic" author="AudioForge">
    <param id="kickPitch" value="55.0"/>
    <param id="kickDecay" value="0.5"/>
    <param id="kickClick" value="0.25"/>
    <param id="kickTone" value="0.7"/>
    <param id="kickDrive" value="0.15"/>
    <param id="snareTune" value="0.5"/>
    <param id="snareSnap" value="0.4"/>
    <param id="snareTone" value="0.65"/>
    <param id="snareDecay" value="0.25"/>
    <param id="snareMix" value="0.75"/>
    <param id="hihatTune" value="0.55"/>
    <param id="hihatDecay" value="0.1"/>
    <param id="hihatTone" value="0.7"/>
    <param id="hihatClick" value="0.45"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Electronic Punchy" category="Electronic" author="AudioForge">
    <param id="kickPitch" value="68.0"/>
    <param id="kickDecay" value="0.2"/>
    <param id="kickClick" value="0.7"/>
    <param id="kickTone" value="0.5"/>
    <param id="kickDrive" value="0.55"/>
    <param id="snareTune" value="0.68"/>
    <param id="snareSnap" value="0.8"/>
    <param id="snareTone" value="0.75"/>
    <param id="snareDecay" value="0.12"/>
    <param id="snareMix" value="0.55"/>
    <param id="hihatTune" value="0.68"/>
    <param id="hihatDecay" value="0.05"/>
    <param id="hihatTone" value="0.85"/>
    <param id="hihatClick" value="0.72"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Electronic Wide" category="Electronic" author="AudioForge">
    <param id="kickPitch" value="60.0"/>
    <param id="kickDecay" value="0.4"/>
    <param id="kickClick" value="0.45"/>
    <param id="kickTone" value="0.6"/>
    <param id="kickDrive" value="0.35"/>
    <param id="snareTune" value="0.6"/>
    <param id="snareSnap" value="0.6"/>
    <param id="snareTone" value="0.7"/>
    <param id="snareDecay" value="0.2"/>
    <param id="snareMix" value="0.65"/>
    <param id="hihatTune" value="0.6"/>
    <param id="hihatDecay" value="0.08"/>
    <param id="hihatTone" value="0.8"/>
    <param id="hihatClick" value="0.6"/>
</preset>)"
    };
}

} // namespace DrumSynthPresets
