#pragma once

/**
 * ADSR Envelope Generator
 *
 * Classic Attack-Decay-Sustain-Release envelope for synthesis.
 * Used for amplitude and filter modulation.
 */
class ADSREnvelope
{
public:
    enum class Stage
    {
        Off,      // Envelope not triggered
        Attack,   // Rising from 0 to 1
        Decay,    // Falling from 1 to sustain level
        Sustain,  // Holding at sustain level
        Release   // Falling from current level to 0
    };

    ADSREnvelope();

    /**
     * Trigger the envelope (note on).
     * Starts attack phase.
     */
    void trigger();

    /**
     * Release the envelope (note off).
     * Enters release phase from current level.
     */
    void release();

    /**
     * Get the next envelope value.
     *
     * @param deltaTime Time since last sample (1.0 / sampleRate)
     * @return Envelope level (0.0 to 1.0)
     */
    float getNextValue(float deltaTime);

    /**
     * Set ADSR parameters.
     */
    void setAttack(float seconds);
    void setDecay(float seconds);
    void setSustain(float level);    // 0.0 to 1.0
    void setRelease(float seconds);

    /**
     * Get current envelope level.
     */
    float getLevel() const { return level; }

    /**
     * Get current envelope stage.
     */
    Stage getStage() const { return stage; }

    /**
     * Check if envelope is active (not in Off stage).
     */
    bool isActive() const { return stage != Stage::Off; }

    /**
     * Reset envelope to off state.
     */
    void reset();

private:
    // ADSR parameters
    float attackTime = 0.01f;    // Attack time in seconds
    float decayTime = 0.1f;      // Decay time in seconds
    float sustainLevel = 0.7f;   // Sustain level (0.0-1.0)
    float releaseTime = 0.3f;    // Release time in seconds

    // State
    Stage stage = Stage::Off;
    float level = 0.0f;          // Current envelope level (0.0-1.0)
    float stageTime = 0.0f;      // Time in current stage
    float releaseStartLevel = 0.0f; // Level when release started

    /**
     * Update envelope for current stage.
     */
    void updateStage(float deltaTime);
};
