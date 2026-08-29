#pragma once

/**
 * Modulation Matrix for Enhanced BasicSynth
 *
 * Routes modulation sources (LFOs, envelopes) to synthesis parameters.
 * Provides flexible modulation routing with depth control.
 */
class ModulationMatrix
{
public:
    enum class Source
    {
        None = 0,
        LFO1 = 1,
        LFO2 = 2,
        AmpEnvelope = 3,
        FilterEnvelope = 4,
        Velocity = 5,
        ModWheel = 6  // MIDI CC 1
    };

    enum class Destination
    {
        None = 0,
        Pitch = 1,           // Oscillator pitch (vibrato)
        FilterCutoff = 2,    // Filter cutoff frequency
        PWM = 3,             // Pulse width modulation
        Amplitude = 4,       // Output level (tremolo)
        PanPosition = 5      // Stereo pan
    };

    /**
     * Modulation routing slot configuration.
     */
    struct Slot
    {
        Source source = Source::None;
        Destination destination = Destination::None;
        float amount = 0.0f;  // Modulation depth (-1.0 to +1.0)
        bool enabled = false;
    };

    /**
     * Container for modulation source values.
     */
    struct SourceValues
    {
        float lfo1 = 0.0f;
        float lfo2 = 0.0f;
        float ampEnvelope = 0.0f;
        float filterEnvelope = 0.0f;
        float velocity = 0.0f;
        float modWheel = 0.0f;
    };

    static constexpr int NUM_SLOTS = 4;

    ModulationMatrix();

    /**
     * Configure a modulation slot.
     *
     * @param index Slot index (0-3)
     * @param slot Slot configuration
     */
    void setSlot(int index, const Slot& slot);

    /**
     * Get modulation value for a specific destination.
     *
     * Sums all slots targeting the destination, scaled by amount.
     *
     * @param dest Destination parameter
     * @param sources Current values of all modulation sources
     * @return Total modulation value for destination
     */
    float getModulation(Destination dest, const SourceValues& sources) const;

    /**
     * Get a specific slot configuration.
     *
     * @param index Slot index (0-3)
     * @return Slot configuration
     */
    const Slot& getSlot(int index) const;

private:
    Slot slots[NUM_SLOTS];

    /**
     * Get source value from SourceValues container.
     */
    float getSourceValue(Source source, const SourceValues& sources) const;
};
