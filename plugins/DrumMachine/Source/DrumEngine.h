/*
  DrumEngine.h - Drum Pattern Generation Engine
*/

#pragma once
#include <vector>
#include <string>
#include <array>
#include <random>

class DrumEngine
{
public:
    enum DrumVoice {
        KICK = 0,
        SNARE = 1,
        HIHAT = 2,
        TOM = 3,
        CRASH = 4,
        NUM_VOICES = 5
    };

    struct DrumHit {
        DrumVoice voice;
        int step;           // 0-15 (16th notes)
        float velocity;     // 0.0-1.0
    };

    struct DrumPattern {
        std::array<std::vector<DrumHit>, NUM_VOICES> voices;
        int numSteps;       // Usually 16
        std::string style;
    };

    struct PatternParams {
        std::string style;      // Rock, Hip-Hop, Jazz, etc.
        int complexity;         // 1-10
        float swing;            // 0.0-1.0 (swing amount)
        bool fillsEnabled;      // Add fills at end of pattern
    };

    DrumEngine();

    // Generate drum pattern
    DrumPattern generatePattern(const PatternParams& params);

    // Get all hits at a specific step
    std::vector<DrumHit> getHitsAtStep(const DrumPattern& pattern, int step) const;

private:
    std::mt19937 rng_;

    // Style-specific generators
    DrumPattern generateRockPattern(const PatternParams& params);
    DrumPattern generateHipHopPattern(const PatternParams& params);
    DrumPattern generateJazzPattern(const PatternParams& params);
    DrumPattern generateEDMPattern(const PatternParams& params);
    DrumPattern generateFunkPattern(const PatternParams& params);
    DrumPattern generateLatinPattern(const PatternParams& params);
    DrumPattern generateBluesPattern(const PatternParams& params);

    // Helper functions
    void addHit(DrumPattern& pattern, DrumVoice voice, int step, float velocity);
    void addHumanization(DrumPattern& pattern);
    void addFill(DrumPattern& pattern, int startStep);
    float randomVelocity(float base, float variation);
};
