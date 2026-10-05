#pragma once

#include <array>

/**
 * BasicSynth factory patches
 *
 * Each patch is a value for every parameter, in the units its control
 * shows. The first patch is what a new instance starts with: the parameter
 * defaults are read from it.
 *
 * Values have to sit on their parameter's steps, or choosing the first
 * patch would not give back exactly what a new instance has.
 *
 * Volumes are set so that the loudest single full-velocity note peaks at
 * about -9 dBFS, the level every AudioForge instrument is calibrated to.
 * A test checks it, so re-measure after changing a patch's tone.
 */
namespace BasicSynthPatches
{
    struct Patch
    {
        const char* name;

        int waveform;            // 0 sine, 1 sawtooth, 2 square
        float volume;            // 0 to 1

        float attack;            // seconds
        float decay;             // seconds
        float sustain;           // 0 to 1
        float release;           // seconds

        float filterCutoff;      // Hz
        float filterResonance;   // Q in steps of 0.1; 0.7 is flat
        int filterType;          // 0 low-pass, 1 high-pass, 2 band-pass, 3 notch

        float chorusRate;        // Hz
        float chorusDepth;       // 0 to 1
        float chorusMix;         // 0 to 1

        float reverbSize;        // 0 to 1
        float reverbDamping;     // 0 to 1
        float reverbMix;         // 0 to 1

        float saturationDrive;   // 0 to 1
        float saturationMix;     // 0 to 1
        int saturationType;      // 0 soft clip, 1 hard clip, 2 tube
    };

    inline constexpr std::array<Patch, 4> all
    {{
        //          wave  vol    A       D      S     R      cutoff    Q       type  chorus             reverb             saturation
        { "Init",   1,    0.70f, 0.010f, 0.10f, 0.7f, 0.30f, 20000.0f, 0.7f,   0,    0.5f, 0.5f, 0.3f,  0.5f, 0.5f, 0.3f,  0.0f, 0.0f, 0 },

        // Dark, fast and dry, with a little tube drive so it reads on small speakers
        { "Bass",   1,    0.47f, 0.005f, 0.25f, 0.6f, 0.08f, 500.0f,   1.2f,   0,    0.5f, 0.5f, 0.0f,  0.5f, 0.5f, 0.0f,  0.3f, 0.4f, 2 },

        // Slow to open and slow to fade, wide from the chorus, in a large room
        { "Pad",    1,    0.39f, 0.600f, 0.80f, 0.8f, 1.50f, 2500.0f,  0.7f,   0,    0.4f, 0.7f, 0.5f,  0.8f, 0.4f, 0.4f,  0.0f, 0.0f, 0 },

        // A square with some edge, and enough chorus and room to sit above a mix
        { "Lead",   2,    0.42f, 0.005f, 0.20f, 0.7f, 0.20f, 4500.0f,  1.5f,   0,    0.8f, 0.3f, 0.2f,  0.4f, 0.5f, 0.15f, 0.2f, 0.3f, 0 },
    }};
}
