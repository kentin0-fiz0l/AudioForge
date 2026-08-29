#include "ModulationMatrix.h"

ModulationMatrix::ModulationMatrix()
{
    // Initialize all slots to default (disabled)
    for (int i = 0; i < NUM_SLOTS; ++i)
    {
        slots[i] = Slot();
    }
}

void ModulationMatrix::setSlot(int index, const Slot& slot)
{
    if (index >= 0 && index < NUM_SLOTS)
    {
        slots[index] = slot;
    }
}

float ModulationMatrix::getModulation(Destination dest, const SourceValues& sources) const
{
    float totalModulation = 0.0f;

    // Sum modulation from all enabled slots targeting this destination
    for (int i = 0; i < NUM_SLOTS; ++i)
    {
        const Slot& slot = slots[i];

        if (!slot.enabled)
            continue;

        if (slot.destination != dest)
            continue;

        // Get source value and scale by amount
        float sourceValue = getSourceValue(slot.source, sources);
        totalModulation += sourceValue * slot.amount;
    }

    return totalModulation;
}

const ModulationMatrix::Slot& ModulationMatrix::getSlot(int index) const
{
    if (index >= 0 && index < NUM_SLOTS)
        return slots[index];

    // Return empty slot if index invalid
    static Slot emptySlot;
    return emptySlot;
}

float ModulationMatrix::getSourceValue(Source source, const SourceValues& sources) const
{
    switch (source)
    {
        case Source::LFO1:
            return sources.lfo1;

        case Source::LFO2:
            return sources.lfo2;

        case Source::AmpEnvelope:
            return sources.ampEnvelope;

        case Source::FilterEnvelope:
            return sources.filterEnvelope;

        case Source::Velocity:
            return sources.velocity;

        case Source::ModWheel:
            return sources.modWheel;

        case Source::None:
        default:
            return 0.0f;
    }
}
