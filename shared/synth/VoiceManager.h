#pragma once

#include <juce_core/juce_core.h>
#include <array>
#include <algorithm>

namespace AudioForge {

/**
 * Voice Manager
 *
 * Template-based polyphonic voice allocator for synthesizer plugins.
 * Handles note-on/off events, voice stealing, and voice lifecycle management.
 *
 * Template parameter VoiceType must implement:
 *   - void noteOn(int midiNote, float velocity, double sampleRate)
 *   - void noteOff()
 *   - bool isActive() const
 *   - int getMidiNote() const
 *   - float getLevel() const  (for voice stealing by quietest)
 *   - void reset()
 *
 * Usage:
 *   VoiceManager<MySynthVoice, 8> voiceManager;
 *   voiceManager.noteOn(60, 0.8f, 44100.0);
 *   for (auto& voice : voiceManager.getActiveVoices()) {
 *       sample += voice.processSample(...);
 *   }
 */
template<typename VoiceType, size_t MaxVoices = 8>
class VoiceManager
{
public:
    enum class StealingMode
    {
        Oldest,   // Steal the voice that was triggered longest ago
        Quietest  // Steal the quietest currently playing voice
    };

    VoiceManager()
    {
        static_assert(MaxVoices > 0 && MaxVoices <= 32, "Voice count must be 1-32");
    }

    /**
     * Trigger a note-on event.
     * Allocates a free voice or steals one if all voices are in use.
     *
     * @param midiNote MIDI note number (0-127)
     * @param velocity Note velocity (0.0-1.0)
     * @param sampleRate Current sample rate
     */
    void noteOn(int midiNote, float velocity, double sampleRate)
    {
        // Find existing voice playing this note (for retriggering)
        VoiceType* existingVoice = findVoicePlayingNote(midiNote);
        if (existingVoice)
        {
            existingVoice->noteOn(midiNote, velocity, sampleRate);
            return;
        }

        // Find a free voice
        VoiceType* freeVoice = findFreeVoice();
        if (freeVoice)
        {
            freeVoice->noteOn(midiNote, velocity, sampleRate);
            return;
        }

        // No free voices - steal one
        VoiceType* voiceToSteal = (stealingMode == StealingMode::Oldest)
            ? findOldestVoice()
            : findQuietestVoice();

        if (voiceToSteal)
        {
            voiceToSteal->noteOn(midiNote, velocity, sampleRate);
        }
    }

    /**
     * Trigger a note-off event.
     * Releases all voices playing the specified MIDI note.
     *
     * @param midiNote MIDI note number (0-127)
     */
    void noteOff(int midiNote)
    {
        for (auto& voice : voices)
        {
            if (voice.isActive() && voice.getMidiNote() == midiNote)
            {
                voice.noteOff();
            }
        }
    }

    /**
     * Stop all voices immediately (all-notes-off).
     */
    void allNotesOff()
    {
        for (auto& voice : voices)
        {
            voice.reset();
        }
    }

    /**
     * Get all voices (for processing).
     * Process all voices even if inactive - they may be in release phase.
     */
    std::array<VoiceType, MaxVoices>& getVoices()
    {
        return voices;
    }

    /**
     * Get only active voices (for efficient processing).
     */
    std::vector<VoiceType*> getActiveVoices()
    {
        std::vector<VoiceType*> activeVoices;
        activeVoices.reserve(MaxVoices);

        for (auto& voice : voices)
        {
            if (voice.isActive())
            {
                activeVoices.push_back(&voice);
            }
        }

        return activeVoices;
    }

    /**
     * Get number of active voices.
     */
    size_t getActiveVoiceCount() const
    {
        size_t count = 0;
        for (const auto& voice : voices)
        {
            if (voice.isActive())
                ++count;
        }
        return count;
    }

    /**
     * Set voice stealing mode.
     */
    void setStealingMode(StealingMode mode)
    {
        stealingMode = mode;
    }

    /**
     * Get maximum number of voices.
     */
    constexpr size_t getMaxVoices() const { return MaxVoices; }

private:
    std::array<VoiceType, MaxVoices> voices;
    StealingMode stealingMode = StealingMode::Oldest;

    /**
     * Find a voice that's currently playing the specified MIDI note.
     */
    VoiceType* findVoicePlayingNote(int midiNote)
    {
        for (auto& voice : voices)
        {
            if (voice.isActive() && voice.getMidiNote() == midiNote)
            {
                return &voice;
            }
        }
        return nullptr;
    }

    /**
     * Find a free (inactive) voice.
     */
    VoiceType* findFreeVoice()
    {
        for (auto& voice : voices)
        {
            if (!voice.isActive())
            {
                return &voice;
            }
        }
        return nullptr;
    }

    /**
     * Find the oldest active voice (first triggered).
     */
    VoiceType* findOldestVoice()
    {
        // Simple implementation: return first active voice
        // A more sophisticated version could track trigger timestamps
        for (auto& voice : voices)
        {
            if (voice.isActive())
            {
                return &voice;
            }
        }
        return nullptr;
    }

    /**
     * Find the quietest active voice.
     */
    VoiceType* findQuietestVoice()
    {
        VoiceType* quietestVoice = nullptr;
        float lowestLevel = 1.0f;

        for (auto& voice : voices)
        {
            if (voice.isActive())
            {
                float level = voice.getLevel();
                if (level < lowestLevel)
                {
                    lowestLevel = level;
                    quietestVoice = &voice;
                }
            }
        }

        return quietestVoice;
    }
};

} // namespace AudioForge
