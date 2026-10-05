#pragma once

/**
 * Which drum a MIDI note plays.
 *
 * Follows the General MIDI drum map, so drum clips written for other
 * instruments work here: C1 kick, D1 snare, F#1 closed hat, A#1 open hat.
 * The pedal hat (G#1) plays the closed hat.
 */
enum class Drum
{
    none,
    kick,
    snare,
    closedHat,
    openHat
};

inline Drum drumForNote(int note)
{
    switch (note)
    {
        case 36: return Drum::kick;
        case 38: return Drum::snare;
        case 42: return Drum::closedHat;
        case 44: return Drum::closedHat;
        case 46: return Drum::openHat;
        default: return Drum::none;
    }
}
