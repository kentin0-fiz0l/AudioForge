#pragma once

/**
 * AudioForge DSP Library
 *
 * Shared DSP components for AudioForge audio plugins.
 *
 * This header includes all DSP utilities in the library.
 * Include only what you need in your plugins, or include this file
 * to get everything.
 */

// Core utilities
#include "Utilities.h"

// Parameter smoothing
#include "ParameterSmoothing.h"

// Metering
#include "Metering.h"

// Panning and stereo processing
#include "PanningAlgorithms.h"

// Filter design
#include "FilterDesign.h"

/**
 * AudioForge namespace contains all library code.
 */
namespace AudioForge {
    /**
     * DSP namespace contains all signal processing code.
     */
    namespace DSP {
        // All classes are defined in their respective headers
    }
}
