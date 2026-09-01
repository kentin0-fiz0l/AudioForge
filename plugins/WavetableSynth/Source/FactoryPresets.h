#pragma once

#include <juce_core/juce_core.h>
#include <vector>

namespace WavetableSynthPresets {

/**
 * Factory Presets for WavetableSynth
 *
 * 25 presets covering diverse synthesis styles:
 * - Init (1)
 * - EDM Lead (6)
 * - Bass (5)
 * - Pad (5)
 * - Digital (5)
 * - Pluck (4)
 *
 * Parameters:
 * - wtPosition: 0.0-1.0 (wavetable position)
 * - filterCutoff: 20-20000 Hz
 * - filterRes: 0.5-10.0
 * - filterType: 0=LP, 1=HP, 2=BP, 3=Notch
 * - filterEnv: 0.0-1.0 (envelope modulation amount)
 * - ampAttack, ampDecay, ampSustain, ampRelease
 * - filtAttack, filtDecay, filtSustain, filtRelease
 */

inline std::vector<juce::String> getFactoryPresets()
{
    return {
        // ===== INIT =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Init" category="Init" author="AudioForge">
    <param id="wtPosition" value="0.0"/>
    <param id="filterCutoff" value="1000.0"/>
    <param id="filterRes" value="0.707"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.5"/>
    <param id="ampAttack" value="0.01"/>
    <param id="ampDecay" value="0.1"/>
    <param id="ampSustain" value="0.8"/>
    <param id="ampRelease" value="0.3"/>
    <param id="filtAttack" value="0.05"/>
    <param id="filtDecay" value="0.2"/>
    <param id="filtSustain" value="0.5"/>
    <param id="filtRelease" value="0.3"/>
</preset>)",

        // ===== EDM LEAD (6) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Rave Lead" category="EDM Lead" author="AudioForge">
    <param id="wtPosition" value="0.5"/>
    <param id="filterCutoff" value="3000.0"/>
    <param id="filterRes" value="2.5"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.8"/>
    <param id="ampAttack" value="0.005"/>
    <param id="ampDecay" value="0.2"/>
    <param id="ampSustain" value="0.7"/>
    <param id="ampRelease" value="0.15"/>
    <param id="filtAttack" value="0.01"/>
    <param id="filtDecay" value="0.3"/>
    <param id="filtSustain" value="0.3"/>
    <param id="filtRelease" value="0.2"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Supersaw Lead" category="EDM Lead" author="AudioForge">
    <param id="wtPosition" value="0.75"/>
    <param id="filterCutoff" value="5000.0"/>
    <param id="filterRes" value="1.5"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.6"/>
    <param id="ampAttack" value="0.01"/>
    <param id="ampDecay" value="0.15"/>
    <param id="ampSustain" value="0.8"/>
    <param id="ampRelease" value="0.2"/>
    <param id="filtAttack" value="0.02"/>
    <param id="filtDecay" value="0.25"/>
    <param id="filtSustain" value="0.4"/>
    <param id="filtRelease" value="0.25"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Acid Lead" category="EDM Lead" author="AudioForge">
    <param id="wtPosition" value="0.25"/>
    <param id="filterCutoff" value="1500.0"/>
    <param id="filterRes" value="6.0"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.9"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="0.1"/>
    <param id="ampSustain" value="0.6"/>
    <param id="ampRelease" value="0.1"/>
    <param id="filtAttack" value="0.001"/>
    <param id="filtDecay" value="0.15"/>
    <param id="filtSustain" value="0.2"/>
    <param id="filtRelease" value="0.15"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Digital Stab" category="EDM Lead" author="AudioForge">
    <param id="wtPosition" value="0.85"/>
    <param id="filterCutoff" value="4000.0"/>
    <param id="filterRes" value="3.0"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.7"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="0.3"/>
    <param id="ampSustain" value="0.5"/>
    <param id="ampRelease" value="0.2"/>
    <param id="filtAttack" value="0.001"/>
    <param id="filtDecay" value="0.4"/>
    <param id="filtSustain" value="0.3"/>
    <param id="filtRelease" value="0.25"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Wobble Lead" category="EDM Lead" author="AudioForge">
    <param id="wtPosition" value="0.6"/>
    <param id="filterCutoff" value="800.0"/>
    <param id="filterRes" value="8.0"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.95"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="0.05"/>
    <param id="ampSustain" value="0.9"/>
    <param id="ampRelease" value="0.1"/>
    <param id="filtAttack" value="0.001"/>
    <param id="filtDecay" value="0.08"/>
    <param id="filtSustain" value="0.1"/>
    <param id="filtRelease" value="0.1"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Harmonic Lead" category="EDM Lead" author="AudioForge">
    <param id="wtPosition" value="0.35"/>
    <param id="filterCutoff" value="6000.0"/>
    <param id="filterRes" value="1.0"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.5"/>
    <param id="ampAttack" value="0.005"/>
    <param id="ampDecay" value="0.2"/>
    <param id="ampSustain" value="0.75"/>
    <param id="ampRelease" value="0.25"/>
    <param id="filtAttack" value="0.01"/>
    <param id="filtDecay" value="0.3"/>
    <param id="filtSustain" value="0.5"/>
    <param id="filtRelease" value="0.3"/>
</preset>)",

        // ===== BASS (5) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Deep Sub" category="Bass" author="AudioForge">
    <param id="wtPosition" value="0.0"/>
    <param id="filterCutoff" value="400.0"/>
    <param id="filterRes" value="1.5"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.3"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="0.3"/>
    <param id="ampSustain" value="0.9"/>
    <param id="ampRelease" value="0.15"/>
    <param id="filtAttack" value="0.005"/>
    <param id="filtDecay" value="0.2"/>
    <param id="filtSustain" value="0.4"/>
    <param id="filtRelease" value="0.1"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Reese Bass" category="Bass" author="AudioForge">
    <param id="wtPosition" value="0.2"/>
    <param id="filterCutoff" value="600.0"/>
    <param id="filterRes" value="3.0"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.6"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="0.15"/>
    <param id="ampSustain" value="0.85"/>
    <param id="ampRelease" value="0.2"/>
    <param id="filtAttack" value="0.001"/>
    <param id="filtDecay" value="0.2"/>
    <param id="filtSustain" value="0.3"/>
    <param id="filtRelease" value="0.15"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Pluck Bass" category="Bass" author="AudioForge">
    <param id="wtPosition" value="0.15"/>
    <param id="filterCutoff" value="1200.0"/>
    <param id="filterRes" value="2.0"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.8"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="0.4"/>
    <param id="ampSustain" value="0.3"/>
    <param id="ampRelease" value="0.2"/>
    <param id="filtAttack" value="0.001"/>
    <param id="filtDecay" value="0.3"/>
    <param id="filtSustain" value="0.15"/>
    <param id="filtRelease" value="0.15"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Growl Bass" category="Bass" author="AudioForge">
    <param id="wtPosition" value="0.4"/>
    <param id="filterCutoff" value="500.0"/>
    <param id="filterRes" value="5.0"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.85"/>
    <param id="ampAttack" value="0.005"/>
    <param id="ampDecay" value="0.2"/>
    <param id="ampSustain" value="0.7"/>
    <param id="ampRelease" value="0.25"/>
    <param id="filtAttack" value="0.01"/>
    <param id="filtDecay" value="0.15"/>
    <param id="filtSustain" value="0.2"/>
    <param id="filtRelease" value="0.2"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="FM Bass" category="Bass" author="AudioForge">
    <param id="wtPosition" value="0.9"/>
    <param id="filterCutoff" value="800.0"/>
    <param id="filterRes" value="2.5"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.5"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="0.25"/>
    <param id="ampSustain" value="0.6"/>
    <param id="ampRelease" value="0.2"/>
    <param id="filtAttack" value="0.005"/>
    <param id="filtDecay" value="0.2"/>
    <param id="filtSustain" value="0.35"/>
    <param id="filtRelease" value="0.15"/>
</preset>)",

        // ===== PAD (5) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Warm Pad" category="Pad" author="AudioForge">
    <param id="wtPosition" value="0.1"/>
    <param id="filterCutoff" value="2000.0"/>
    <param id="filterRes" value="0.8"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.4"/>
    <param id="ampAttack" value="0.8"/>
    <param id="ampDecay" value="0.5"/>
    <param id="ampSustain" value="0.9"/>
    <param id="ampRelease" value="1.5"/>
    <param id="filtAttack" value="1.0"/>
    <param id="filtDecay" value="0.6"/>
    <param id="filtSustain" value="0.7"/>
    <param id="filtRelease" value="1.2"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Bright Strings" category="Pad" author="AudioForge">
    <param id="wtPosition" value="0.3"/>
    <param id="filterCutoff" value="4000.0"/>
    <param id="filterRes" value="1.2"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.3"/>
    <param id="ampAttack" value="0.6"/>
    <param id="ampDecay" value="0.4"/>
    <param id="ampSustain" value="0.85"/>
    <param id="ampRelease" value="1.0"/>
    <param id="filtAttack" value="0.8"/>
    <param id="filtDecay" value="0.5"/>
    <param id="filtSustain" value="0.6"/>
    <param id="filtRelease" value="0.8"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Ethereal Pad" category="Pad" author="AudioForge">
    <param id="wtPosition" value="0.65"/>
    <param id="filterCutoff" value="1500.0"/>
    <param id="filterRes" value="0.707"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.5"/>
    <param id="ampAttack" value="1.2"/>
    <param id="ampDecay" value="0.6"/>
    <param id="ampSustain" value="0.95"/>
    <param id="ampRelease" value="2.0"/>
    <param id="filtAttack" value="1.5"/>
    <param id="filtDecay" value="0.8"/>
    <param id="filtSustain" value="0.8"/>
    <param id="filtRelease" value="1.8"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Dark Atmosphere" category="Pad" author="AudioForge">
    <param id="wtPosition" value="0.45"/>
    <param id="filterCutoff" value="800.0"/>
    <param id="filterRes" value="1.5"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.6"/>
    <param id="ampAttack" value="1.0"/>
    <param id="ampDecay" value="0.8"/>
    <param id="ampSustain" value="0.9"/>
    <param id="ampRelease" value="2.5"/>
    <param id="filtAttack" value="1.2"/>
    <param id="filtDecay" value="1.0"/>
    <param id="filtSustain" value="0.5"/>
    <param id="filtRelease" value="2.0"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Choir Pad" category="Pad" author="AudioForge">
    <param id="wtPosition" value="0.95"/>
    <param id="filterCutoff" value="3000.0"/>
    <param id="filterRes" value="1.0"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.35"/>
    <param id="ampAttack" value="0.7"/>
    <param id="ampDecay" value="0.5"/>
    <param id="ampSustain" value="0.88"/>
    <param id="ampRelease" value="1.2"/>
    <param id="filtAttack" value="0.9"/>
    <param id="filtDecay" value="0.6"/>
    <param id="filtSustain" value="0.65"/>
    <param id="filtRelease" value="1.0"/>
</preset>)",

        // ===== DIGITAL (5) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Metallic Sweep" category="Digital" author="AudioForge">
    <param id="wtPosition" value="0.92"/>
    <param id="filterCutoff" value="2500.0"/>
    <param id="filterRes" value="4.0"/>
    <param id="filterType" value="2"/>
    <param id="filterEnv" value="0.9"/>
    <param id="ampAttack" value="0.01"/>
    <param id="ampDecay" value="0.4"/>
    <param id="ampSustain" value="0.6"/>
    <param id="ampRelease" value="0.5"/>
    <param id="filtAttack" value="0.02"/>
    <param id="filtDecay" value="0.5"/>
    <param id="filtSustain" value="0.25"/>
    <param id="filtRelease" value="0.4"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Bell Tone" category="Digital" author="AudioForge">
    <param id="wtPosition" value="0.88"/>
    <param id="filterCutoff" value="8000.0"/>
    <param id="filterRes" value="2.0"/>
    <param id="filterType" value="1"/>
    <param id="filterEnv" value="0.4"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="1.0"/>
    <param id="ampSustain" value="0.3"/>
    <param id="ampRelease" value="0.8"/>
    <param id="filtAttack" value="0.001"/>
    <param id="filtDecay" value="0.8"/>
    <param id="filtSustain" value="0.2"/>
    <param id="filtRelease" value="0.6"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Glass Chime" category="Digital" author="AudioForge">
    <param id="wtPosition" value="0.98"/>
    <param id="filterCutoff" value="12000.0"/>
    <param id="filterRes" value="3.5"/>
    <param id="filterType" value="1"/>
    <param id="filterEnv" value="0.3"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="0.8"/>
    <param id="ampSustain" value="0.2"/>
    <param id="ampRelease" value="1.0"/>
    <param id="filtAttack" value="0.001"/>
    <param id="filtDecay" value="0.6"/>
    <param id="filtSustain" value="0.15"/>
    <param id="filtRelease" value="0.8"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Formant Voice" category="Digital" author="AudioForge">
    <param id="wtPosition" value="0.78"/>
    <param id="filterCutoff" value="1800.0"/>
    <param id="filterRes" value="6.0"/>
    <param id="filterType" value="2"/>
    <param id="filterEnv" value="0.7"/>
    <param id="ampAttack" value="0.05"/>
    <param id="ampDecay" value="0.3"/>
    <param id="ampSustain" value="0.75"/>
    <param id="ampRelease" value="0.4"/>
    <param id="filtAttack" value="0.08"/>
    <param id="filtDecay" value="0.35"/>
    <param id="filtSustain" value="0.5"/>
    <param id="filtRelease" value="0.35"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Alien Voice" category="Digital" author="AudioForge">
    <param id="wtPosition" value="0.82"/>
    <param id="filterCutoff" value="1200.0"/>
    <param id="filterRes" value="7.0"/>
    <param id="filterType" value="2"/>
    <param id="filterEnv" value="0.85"/>
    <param id="ampAttack" value="0.02"/>
    <param id="ampDecay" value="0.25"/>
    <param id="ampSustain" value="0.7"/>
    <param id="ampRelease" value="0.3"/>
    <param id="filtAttack" value="0.03"/>
    <param id="filtDecay" value="0.3"/>
    <param id="filtSustain" value="0.4"/>
    <param id="filtRelease" value="0.25"/>
</preset>)",

        // ===== PLUCK (4) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Marimba" category="Pluck" author="AudioForge">
    <param id="wtPosition" value="0.05"/>
    <param id="filterCutoff" value="3500.0"/>
    <param id="filterRes" value="1.2"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.6"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="0.6"/>
    <param id="ampSustain" value="0.1"/>
    <param id="ampRelease" value="0.4"/>
    <param id="filtAttack" value="0.001"/>
    <param id="filtDecay" value="0.5"/>
    <param id="filtSustain" value="0.1"/>
    <param id="filtRelease" value="0.3"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Harp Pluck" category="Pluck" author="AudioForge">
    <param id="wtPosition" value="0.12"/>
    <param id="filterCutoff" value="6000.0"/>
    <param id="filterRes" value="0.9"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.5"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="0.8"/>
    <param id="ampSustain" value="0.05"/>
    <param id="ampRelease" value="0.6"/>
    <param id="filtAttack" value="0.001"/>
    <param id="filtDecay" value="0.7"/>
    <param id="filtSustain" value="0.05"/>
    <param id="filtRelease" value="0.5"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Pizzicato" category="Pluck" author="AudioForge">
    <param id="wtPosition" value="0.25"/>
    <param id="filterCutoff" value="4500.0"/>
    <param id="filterRes" value="1.5"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.7"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="0.4"/>
    <param id="ampSustain" value="0.15"/>
    <param id="ampRelease" value="0.3"/>
    <param id="filtAttack" value="0.001"/>
    <param id="filtDecay" value="0.35"/>
    <param id="filtSustain" value="0.12"/>
    <param id="filtRelease" value="0.25"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Electric Pluck" category="Pluck" author="AudioForge">
    <param id="wtPosition" value="0.55"/>
    <param id="filterCutoff" value="5500.0"/>
    <param id="filterRes" value="2.5"/>
    <param id="filterType" value="0"/>
    <param id="filterEnv" value="0.75"/>
    <param id="ampAttack" value="0.001"/>
    <param id="ampDecay" value="0.5"/>
    <param id="ampSustain" value="0.2"/>
    <param id="ampRelease" value="0.35"/>
    <param id="filtAttack" value="0.001"/>
    <param id="filtDecay" value="0.4"/>
    <param id="filtSustain" value="0.18"/>
    <param id="filtRelease" value="0.3"/>
</preset>)"
    };
}

} // namespace WavetableSynthPresets
