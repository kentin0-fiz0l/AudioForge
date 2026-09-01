#pragma once

#include <juce_core/juce_core.h>
#include <vector>

namespace OrganEmulatorPresets
{

/**
 * Factory Presets for OrganEmulator
 *
 * 25 presets covering classic organ sounds:
 * - Jazz (6 presets)
 * - Gospel (6 presets)
 * - Rock (6 presets)
 * - Vintage (7 presets)
 *
 * Parameters:
 * - 9 drawbars (16', 5⅓', 8', 4', 2⅔', 2', 1⅗', 1⅓', 1') each 0.0-1.0
 * - leslieSpeed: false=slow, true=fast
 * - leslieDepth: 0.0-1.0
 * - leslieMix: 0.0-1.0
 * - percussionClick: 0.0-1.0
 * - percussionDecay: 0.01-0.5s
 */

inline std::vector<juce::String> getFactoryPresets()
{
    return {
        // ===== JAZZ (6) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Jazz Full" category="Jazz" author="AudioForge">
    <param id="drawbar16" value="0.8"/>
    <param id="drawbar5_3" value="0.6"/>
    <param id="drawbar8" value="1.0"/>
    <param id="drawbar4" value="0.5"/>
    <param id="drawbar2_3" value="0.4"/>
    <param id="drawbar2" value="0.3"/>
    <param id="drawbar1_3_5" value="0.2"/>
    <param id="drawbar1_1_3" value="0.2"/>
    <param id="drawbar1" value="0.1"/>
    <param id="leslieSpeed" value="0.0"/>
    <param id="leslieDepth" value="0.5"/>
    <param id="leslieMix" value="0.7"/>
    <param id="percussionClick" value="0.3"/>
    <param id="percussionDecay" value="0.05"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Jazz Mellow" category="Jazz" author="AudioForge">
    <param id="drawbar16" value="0.6"/>
    <param id="drawbar5_3" value="0.8"/>
    <param id="drawbar8" value="0.9"/>
    <param id="drawbar4" value="0.3"/>
    <param id="drawbar2_3" value="0.2"/>
    <param id="drawbar2" value="0.1"/>
    <param id="drawbar1_3_5" value="0.0"/>
    <param id="drawbar1_1_3" value="0.0"/>
    <param id="drawbar1" value="0.0"/>
    <param id="leslieSpeed" value="0.0"/>
    <param id="leslieDepth" value="0.4"/>
    <param id="leslieMix" value="0.6"/>
    <param id="percussionClick" value="0.2"/>
    <param id="percussionDecay" value="0.08"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Jazz Bright" category="Jazz" author="AudioForge">
    <param id="drawbar16" value="0.5"/>
    <param id="drawbar5_3" value="0.4"/>
    <param id="drawbar8" value="0.9"/>
    <param id="drawbar4" value="0.7"/>
    <param id="drawbar2_3" value="0.6"/>
    <param id="drawbar2" value="0.5"/>
    <param id="drawbar1_3_5" value="0.4"/>
    <param id="drawbar1_1_3" value="0.3"/>
    <param id="drawbar1" value="0.2"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.6"/>
    <param id="leslieMix" value="0.8"/>
    <param id="percussionClick" value="0.5"/>
    <param id="percussionDecay" value="0.04"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Jazz Ballad" category="Jazz" author="AudioForge">
    <param id="drawbar16" value="0.7"/>
    <param id="drawbar5_3" value="0.7"/>
    <param id="drawbar8" value="0.8"/>
    <param id="drawbar4" value="0.4"/>
    <param id="drawbar2_3" value="0.3"/>
    <param id="drawbar2" value="0.2"/>
    <param id="drawbar1_3_5" value="0.1"/>
    <param id="drawbar1_1_3" value="0.1"/>
    <param id="drawbar1" value="0.0"/>
    <param id="leslieSpeed" value="0.0"/>
    <param id="leslieDepth" value="0.3"/>
    <param id="leslieMix" value="0.5"/>
    <param id="percussionClick" value="0.1"/>
    <param id="percussionDecay" value="0.1"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Jazz Percussive" category="Jazz" author="AudioForge">
    <param id="drawbar16" value="0.4"/>
    <param id="drawbar5_3" value="0.3"/>
    <param id="drawbar8" value="1.0"/>
    <param id="drawbar4" value="0.6"/>
    <param id="drawbar2_3" value="0.5"/>
    <param id="drawbar2" value="0.4"/>
    <param id="drawbar1_3_5" value="0.3"/>
    <param id="drawbar1_1_3" value="0.2"/>
    <param id="drawbar1" value="0.1"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.5"/>
    <param id="leslieMix" value="0.7"/>
    <param id="percussionClick" value="0.8"/>
    <param id="percussionDecay" value="0.03"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Jazz Combo" category="Jazz" author="AudioForge">
    <param id="drawbar16" value="0.6"/>
    <param id="drawbar5_3" value="0.5"/>
    <param id="drawbar8" value="0.9"/>
    <param id="drawbar4" value="0.5"/>
    <param id="drawbar2_3" value="0.4"/>
    <param id="drawbar2" value="0.3"/>
    <param id="drawbar1_3_5" value="0.2"/>
    <param id="drawbar1_1_3" value="0.1"/>
    <param id="drawbar1" value="0.0"/>
    <param id="leslieSpeed" value="0.0"/>
    <param id="leslieDepth" value="0.4"/>
    <param id="leslieMix" value="0.6"/>
    <param id="percussionClick" value="0.4"/>
    <param id="percussionDecay" value="0.05"/>
</preset>)",

        // ===== GOSPEL (6) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Gospel Full" category="Gospel" author="AudioForge">
    <param id="drawbar16" value="1.0"/>
    <param id="drawbar5_3" value="0.8"/>
    <param id="drawbar8" value="1.0"/>
    <param id="drawbar4" value="0.7"/>
    <param id="drawbar2_3" value="0.6"/>
    <param id="drawbar2" value="0.5"/>
    <param id="drawbar1_3_5" value="0.4"/>
    <param id="drawbar1_1_3" value="0.3"/>
    <param id="drawbar1" value="0.2"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.7"/>
    <param id="leslieMix" value="0.9"/>
    <param id="percussionClick" value="0.4"/>
    <param id="percussionDecay" value="0.05"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Gospel Warm" category="Gospel" author="AudioForge">
    <param id="drawbar16" value="0.9"/>
    <param id="drawbar5_3" value="0.7"/>
    <param id="drawbar8" value="0.9"/>
    <param id="drawbar4" value="0.5"/>
    <param id="drawbar2_3" value="0.4"/>
    <param id="drawbar2" value="0.3"/>
    <param id="drawbar1_3_5" value="0.2"/>
    <param id="drawbar1_1_3" value="0.1"/>
    <param id="drawbar1" value="0.0"/>
    <param id="leslieSpeed" value="0.0"/>
    <param id="leslieDepth" value="0.5"/>
    <param id="leslieMix" value="0.7"/>
    <param id="percussionClick" value="0.2"/>
    <param id="percussionDecay" value="0.07"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Gospel Praise" category="Gospel" author="AudioForge">
    <param id="drawbar16" value="0.8"/>
    <param id="drawbar5_3" value="0.6"/>
    <param id="drawbar8" value="1.0"/>
    <param id="drawbar4" value="0.6"/>
    <param id="drawbar2_3" value="0.5"/>
    <param id="drawbar2" value="0.4"/>
    <param id="drawbar1_3_5" value="0.3"/>
    <param id="drawbar1_1_3" value="0.2"/>
    <param id="drawbar1" value="0.1"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.6"/>
    <param id="leslieMix" value="0.8"/>
    <param id="percussionClick" value="0.5"/>
    <param id="percussionDecay" value="0.04"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Gospel Soft" category="Gospel" author="AudioForge">
    <param id="drawbar16" value="0.7"/>
    <param id="drawbar5_3" value="0.8"/>
    <param id="drawbar8" value="0.8"/>
    <param id="drawbar4" value="0.4"/>
    <param id="drawbar2_3" value="0.3"/>
    <param id="drawbar2" value="0.2"/>
    <param id="drawbar1_3_5" value="0.1"/>
    <param id="drawbar1_1_3" value="0.0"/>
    <param id="drawbar1" value="0.0"/>
    <param id="leslieSpeed" value="0.0"/>
    <param id="leslieDepth" value="0.4"/>
    <param id="leslieMix" value="0.6"/>
    <param id="percussionClick" value="0.1"/>
    <param id="percussionDecay" value="0.08"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Gospel Bright" category="Gospel" author="AudioForge">
    <param id="drawbar16" value="0.6"/>
    <param id="drawbar5_3" value="0.5"/>
    <param id="drawbar8" value="0.9"/>
    <param id="drawbar4" value="0.8"/>
    <param id="drawbar2_3" value="0.7"/>
    <param id="drawbar2" value="0.6"/>
    <param id="drawbar1_3_5" value="0.5"/>
    <param id="drawbar1_1_3" value="0.4"/>
    <param id="drawbar1" value="0.3"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.7"/>
    <param id="leslieMix" value="0.85"/>
    <param id="percussionClick" value="0.6"/>
    <param id="percussionDecay" value="0.03"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Gospel Church" category="Gospel" author="AudioForge">
    <param id="drawbar16" value="0.85"/>
    <param id="drawbar5_3" value="0.75"/>
    <param id="drawbar8" value="0.95"/>
    <param id="drawbar4" value="0.6"/>
    <param id="drawbar2_3" value="0.5"/>
    <param id="drawbar2" value="0.4"/>
    <param id="drawbar1_3_5" value="0.3"/>
    <param id="drawbar1_1_3" value="0.2"/>
    <param id="drawbar1" value="0.1"/>
    <param id="leslieSpeed" value="0.0"/>
    <param id="leslieDepth" value="0.5"/>
    <param id="leslieMix" value="0.75"/>
    <param id="percussionClick" value="0.3"/>
    <param id="percussionDecay" value="0.06"/>
</preset>)",

        // ===== ROCK (6) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Rock Classic" category="Rock" author="AudioForge">
    <param id="drawbar16" value="0.9"/>
    <param id="drawbar5_3" value="0.7"/>
    <param id="drawbar8" value="1.0"/>
    <param id="drawbar4" value="0.7"/>
    <param id="drawbar2_3" value="0.6"/>
    <param id="drawbar2" value="0.5"/>
    <param id="drawbar1_3_5" value="0.4"/>
    <param id="drawbar1_1_3" value="0.3"/>
    <param id="drawbar1" value="0.2"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.8"/>
    <param id="leslieMix" value="0.9"/>
    <param id="percussionClick" value="0.6"/>
    <param id="percussionDecay" value="0.04"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Rock Driven" category="Rock" author="AudioForge">
    <param id="drawbar16" value="1.0"/>
    <param id="drawbar5_3" value="0.8"/>
    <param id="drawbar8" value="1.0"/>
    <param id="drawbar4" value="0.8"/>
    <param id="drawbar2_3" value="0.7"/>
    <param id="drawbar2" value="0.6"/>
    <param id="drawbar1_3_5" value="0.5"/>
    <param id="drawbar1_1_3" value="0.4"/>
    <param id="drawbar1" value="0.3"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.9"/>
    <param id="leslieMix" value="1.0"/>
    <param id="percussionClick" value="0.7"/>
    <param id="percussionDecay" value="0.03"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Rock Ballad" category="Rock" author="AudioForge">
    <param id="drawbar16" value="0.7"/>
    <param id="drawbar5_3" value="0.6"/>
    <param id="drawbar8" value="0.9"/>
    <param id="drawbar4" value="0.5"/>
    <param id="drawbar2_3" value="0.4"/>
    <param id="drawbar2" value="0.3"/>
    <param id="drawbar1_3_5" value="0.2"/>
    <param id="drawbar1_1_3" value="0.1"/>
    <param id="drawbar1" value="0.0"/>
    <param id="leslieSpeed" value="0.0"/>
    <param id="leslieDepth" value="0.5"/>
    <param id="leslieMix" value="0.7"/>
    <param id="percussionClick" value="0.3"/>
    <param id="percussionDecay" value="0.06"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Rock Bright" category="Rock" author="AudioForge">
    <param id="drawbar16" value="0.5"/>
    <param id="drawbar5_3" value="0.4"/>
    <param id="drawbar8" value="0.9"/>
    <param id="drawbar4" value="0.8"/>
    <param id="drawbar2_3" value="0.7"/>
    <param id="drawbar2" value="0.7"/>
    <param id="drawbar1_3_5" value="0.6"/>
    <param id="drawbar1_1_3" value="0.5"/>
    <param id="drawbar1" value="0.4"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.7"/>
    <param id="leslieMix" value="0.85"/>
    <param id="percussionClick" value="0.8"/>
    <param id="percussionDecay" value="0.02"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Rock Deep" category="Rock" author="AudioForge">
    <param id="drawbar16" value="1.0"/>
    <param id="drawbar5_3" value="0.9"/>
    <param id="drawbar8" value="1.0"/>
    <param id="drawbar4" value="0.6"/>
    <param id="drawbar2_3" value="0.5"/>
    <param id="drawbar2" value="0.4"/>
    <param id="drawbar1_3_5" value="0.3"/>
    <param id="drawbar1_1_3" value="0.2"/>
    <param id="drawbar1" value="0.1"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.6"/>
    <param id="leslieMix" value="0.8"/>
    <param id="percussionClick" value="0.4"/>
    <param id="percussionDecay" value="0.05"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Rock Lead" category="Rock" author="AudioForge">
    <param id="drawbar16" value="0.6"/>
    <param id="drawbar5_3" value="0.5"/>
    <param id="drawbar8" value="1.0"/>
    <param id="drawbar4" value="0.7"/>
    <param id="drawbar2_3" value="0.6"/>
    <param id="drawbar2" value="0.6"/>
    <param id="drawbar1_3_5" value="0.5"/>
    <param id="drawbar1_1_3" value="0.4"/>
    <param id="drawbar1" value="0.3"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.75"/>
    <param id="leslieMix" value="0.9"/>
    <param id="percussionClick" value="0.7"/>
    <param id="percussionDecay" value="0.03"/>
</preset>)",

        // ===== VINTAGE (7) =====
        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Vintage B3" category="Vintage" author="AudioForge">
    <param id="drawbar16" value="0.8"/>
    <param id="drawbar5_3" value="0.6"/>
    <param id="drawbar8" value="1.0"/>
    <param id="drawbar4" value="0.5"/>
    <param id="drawbar2_3" value="0.4"/>
    <param id="drawbar2" value="0.3"/>
    <param id="drawbar1_3_5" value="0.2"/>
    <param id="drawbar1_1_3" value="0.2"/>
    <param id="drawbar1" value="0.1"/>
    <param id="leslieSpeed" value="0.0"/>
    <param id="leslieDepth" value="0.5"/>
    <param id="leslieMix" value="0.7"/>
    <param id="percussionClick" value="0.3"/>
    <param id="percussionDecay" value="0.05"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Vintage C3" category="Vintage" author="AudioForge">
    <param id="drawbar16" value="0.9"/>
    <param id="drawbar5_3" value="0.7"/>
    <param id="drawbar8" value="0.9"/>
    <param id="drawbar4" value="0.6"/>
    <param id="drawbar2_3" value="0.5"/>
    <param id="drawbar2" value="0.4"/>
    <param id="drawbar1_3_5" value="0.3"/>
    <param id="drawbar1_1_3" value="0.2"/>
    <param id="drawbar1" value="0.1"/>
    <param id="leslieSpeed" value="0.0"/>
    <param id="leslieDepth" value="0.4"/>
    <param id="leslieMix" value="0.6"/>
    <param id="percussionClick" value="0.2"/>
    <param id="percussionDecay" value="0.06"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Vintage 60s" category="Vintage" author="AudioForge">
    <param id="drawbar16" value="0.7"/>
    <param id="drawbar5_3" value="0.5"/>
    <param id="drawbar8" value="0.9"/>
    <param id="drawbar4" value="0.6"/>
    <param id="drawbar2_3" value="0.5"/>
    <param id="drawbar2" value="0.4"/>
    <param id="drawbar1_3_5" value="0.3"/>
    <param id="drawbar1_1_3" value="0.2"/>
    <param id="drawbar1" value="0.1"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.6"/>
    <param id="leslieMix" value="0.8"/>
    <param id="percussionClick" value="0.5"/>
    <param id="percussionDecay" value="0.04"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Vintage 70s" category="Vintage" author="AudioForge">
    <param id="drawbar16" value="0.85"/>
    <param id="drawbar5_3" value="0.7"/>
    <param id="drawbar8" value="1.0"/>
    <param id="drawbar4" value="0.7"/>
    <param id="drawbar2_3" value="0.6"/>
    <param id="drawbar2" value="0.5"/>
    <param id="drawbar1_3_5" value="0.4"/>
    <param id="drawbar1_1_3" value="0.3"/>
    <param id="drawbar1" value="0.2"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.7"/>
    <param id="leslieMix" value="0.85"/>
    <param id="percussionClick" value="0.4"/>
    <param id="percussionDecay" value="0.05"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Vintage Smooth" category="Vintage" author="AudioForge">
    <param id="drawbar16" value="0.7"/>
    <param id="drawbar5_3" value="0.8"/>
    <param id="drawbar8" value="0.8"/>
    <param id="drawbar4" value="0.4"/>
    <param id="drawbar2_3" value="0.3"/>
    <param id="drawbar2" value="0.2"/>
    <param id="drawbar1_3_5" value="0.1"/>
    <param id="drawbar1_1_3" value="0.1"/>
    <param id="drawbar1" value="0.0"/>
    <param id="leslieSpeed" value="0.0"/>
    <param id="leslieDepth" value="0.3"/>
    <param id="leslieMix" value="0.5"/>
    <param id="percussionClick" value="0.1"/>
    <param id="percussionDecay" value="0.08"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Vintage Fat" category="Vintage" author="AudioForge">
    <param id="drawbar16" value="1.0"/>
    <param id="drawbar5_3" value="0.8"/>
    <param id="drawbar8" value="1.0"/>
    <param id="drawbar4" value="0.6"/>
    <param id="drawbar2_3" value="0.5"/>
    <param id="drawbar2" value="0.4"/>
    <param id="drawbar1_3_5" value="0.3"/>
    <param id="drawbar1_1_3" value="0.2"/>
    <param id="drawbar1" value="0.1"/>
    <param id="leslieSpeed" value="0.0"/>
    <param id="leslieDepth" value="0.5"/>
    <param id="leslieMix" value="0.7"/>
    <param id="percussionClick" value="0.3"/>
    <param id="percussionDecay" value="0.05"/>
</preset>)",

        R"(<?xml version="1.0" encoding="UTF-8"?>
<preset name="Vintage Blues" category="Vintage" author="AudioForge">
    <param id="drawbar16" value="0.75"/>
    <param id="drawbar5_3" value="0.65"/>
    <param id="drawbar8" value="0.95"/>
    <param id="drawbar4" value="0.6"/>
    <param id="drawbar2_3" value="0.5"/>
    <param id="drawbar2" value="0.4"/>
    <param id="drawbar1_3_5" value="0.3"/>
    <param id="drawbar1_1_3" value="0.2"/>
    <param id="drawbar1" value="0.1"/>
    <param id="leslieSpeed" value="1.0"/>
    <param id="leslieDepth" value="0.6"/>
    <param id="leslieMix" value="0.75"/>
    <param id="percussionClick" value="0.4"/>
    <param id="percussionDecay" value="0.05"/>
</preset>)"
    };
}

} // namespace OrganEmulatorPresets
