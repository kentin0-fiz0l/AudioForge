/*
  DrumEngine.cpp - Drum Pattern Generation Implementation
*/

#include "DrumEngine.h"
#include <algorithm>

DrumEngine::DrumEngine()
{
    std::random_device rd;
    rng_.seed(rd());
}

void DrumEngine::addHit(DrumPattern& pattern, DrumVoice voice, int step, float velocity)
{
    if (step >= 0 && step < pattern.numSteps) {
        pattern.voices[voice].push_back({voice, step, velocity});
    }
}

float DrumEngine::randomVelocity(float base, float variation)
{
    std::uniform_real_distribution<float> dist(-variation, variation);
    return std::max(0.0f, std::min(1.0f, base + dist(rng_)));
}

void DrumEngine::addHumanization(DrumPattern& pattern)
{
    std::uniform_real_distribution<float> velDist(-0.05f, 0.05f);

    for (auto& voiceHits : pattern.voices) {
        for (auto& hit : voiceHits) {
            hit.velocity = std::max(0.0f, std::min(1.0f, hit.velocity + velDist(rng_)));
        }
    }
}

void DrumEngine::addFill(DrumPattern& pattern, int startStep)
{
    // Tom fill in last 2 beats
    for (int i = 0; i < 4; ++i) {
        addHit(pattern, TOM, startStep + i, 0.7f + (i * 0.05f));
    }
    // Crash at the end
    addHit(pattern, CRASH, startStep + 4, 0.9f);
}

std::vector<DrumEngine::DrumHit> DrumEngine::getHitsAtStep(const DrumPattern& pattern, int step) const
{
    std::vector<DrumHit> hits;
    for (int voice = 0; voice < NUM_VOICES; ++voice) {
        for (const auto& hit : pattern.voices[voice]) {
            if (hit.step == step) {
                hits.push_back(hit);
            }
        }
    }
    return hits;
}

DrumEngine::DrumPattern DrumEngine::generatePattern(const PatternParams& params)
{
    if (params.style == "Rock")
        return generateRockPattern(params);
    else if (params.style == "Hip-Hop")
        return generateHipHopPattern(params);
    else if (params.style == "Jazz")
        return generateJazzPattern(params);
    else if (params.style == "EDM")
        return generateEDMPattern(params);
    else if (params.style == "Funk")
        return generateFunkPattern(params);
    else if (params.style == "Latin")
        return generateLatinPattern(params);
    else if (params.style == "Blues")
        return generateBluesPattern(params);

    return generateRockPattern(params);
}

DrumEngine::DrumPattern DrumEngine::generateRockPattern(const PatternParams& params)
{
    DrumPattern pattern;
    pattern.numSteps = 16;
    pattern.style = "Rock";

    // Kick: 4-on-floor (steps 0, 4, 8, 12) or variations
    addHit(pattern, KICK, 0, 0.9f);
    addHit(pattern, KICK, 4, 0.9f);
    addHit(pattern, KICK, 8, 0.9f);
    addHit(pattern, KICK, 12, 0.9f);

    // Snare: backbeat (steps 4, 12)
    addHit(pattern, SNARE, 4, 0.85f);
    addHit(pattern, SNARE, 12, 0.85f);

    // Hihat: 8th notes or 16th notes based on complexity
    if (params.complexity > 5) {
        // 16th notes
        for (int i = 0; i < 16; ++i) {
            float vel = (i % 2 == 0) ? 0.7f : 0.5f;  // Accent on beats
            addHit(pattern, HIHAT, i, vel);
        }
    } else {
        // 8th notes
        for (int i = 0; i < 16; i += 2) {
            addHit(pattern, HIHAT, i, 0.7f);
        }
    }

    // Add fill if enabled
    if (params.fillsEnabled) {
        addFill(pattern, 12);
    }

    addHumanization(pattern);
    return pattern;
}

DrumEngine::DrumPattern DrumEngine::generateHipHopPattern(const PatternParams& params)
{
    DrumPattern pattern;
    pattern.numSteps = 16;
    pattern.style = "Hip-Hop";

    // Kick: boom-bap pattern
    addHit(pattern, KICK, 0, 0.95f);
    addHit(pattern, KICK, 6, 0.7f);   // Syncopated
    addHit(pattern, KICK, 10, 0.8f);

    // Snare: on 4 and 12 (backbeat)
    addHit(pattern, SNARE, 4, 0.9f);
    addHit(pattern, SNARE, 12, 0.9f);

    // Ghost snares for complexity
    if (params.complexity > 6) {
        addHit(pattern, SNARE, 7, 0.3f);
        addHit(pattern, SNARE, 14, 0.35f);
    }

    // Hihat: varied pattern (closed)
    for (int i = 0; i < 16; i += 2) {
        float vel = (i % 4 == 0) ? 0.75f : 0.55f;
        addHit(pattern, HIHAT, i, vel);
    }

    // Add offbeat hihats for complexity
    if (params.complexity > 4) {
        addHit(pattern, HIHAT, 3, 0.4f);
        addHit(pattern, HIHAT, 11, 0.4f);
    }

    addHumanization(pattern);
    return pattern;
}

DrumEngine::DrumPattern DrumEngine::generateJazzPattern(const PatternParams& params)
{
    DrumPattern pattern;
    pattern.numSteps = 16;
    pattern.style = "Jazz";

    // Kick: sparse, on 1 and 3
    addHit(pattern, KICK, 0, 0.7f);
    addHit(pattern, KICK, 8, 0.65f);

    // Snare: light backbeat, ghost notes
    addHit(pattern, SNARE, 4, 0.6f);
    addHit(pattern, SNARE, 12, 0.6f);

    // Ghost snares (feathering)
    if (params.complexity > 3) {
        addHit(pattern, SNARE, 2, 0.25f);
        addHit(pattern, SNARE, 6, 0.3f);
        addHit(pattern, SNARE, 10, 0.25f);
        addHit(pattern, SNARE, 14, 0.3f);
    }

    // Ride/Hihat: swing feel (triplet-based)
    // Approximating swing with 16th grid
    for (int i = 0; i < 16; i += 2) {
        addHit(pattern, HIHAT, i, 0.65f);
        if (params.swing > 0.3f) {
            // Add swing (delayed upbeat)
            addHit(pattern, HIHAT, i + 1, 0.45f);
        }
    }

    addHumanization(pattern);
    return pattern;
}

DrumEngine::DrumPattern DrumEngine::generateEDMPattern(const PatternParams& params)
{
    DrumPattern pattern;
    pattern.numSteps = 16;
    pattern.style = "EDM";

    // Kick: 4-on-floor (every beat)
    for (int i = 0; i < 16; i += 4) {
        addHit(pattern, KICK, i, 1.0f);  // Maximum impact
    }

    // Snare/Clap: on 2 and 4 (backbeat)
    addHit(pattern, SNARE, 4, 0.9f);
    addHit(pattern, SNARE, 12, 0.9f);

    // Hihat: 16th notes (straight, no swing)
    for (int i = 0; i < 16; ++i) {
        float vel = (i % 4 == 0) ? 0.8f : 0.6f;
        addHit(pattern, HIHAT, i, vel);
    }

    // Add crash on 1 if high complexity
    if (params.complexity > 7) {
        addHit(pattern, CRASH, 0, 0.85f);
    }

    // No humanization for EDM (quantized)
    return pattern;
}

DrumEngine::DrumPattern DrumEngine::generateFunkPattern(const PatternParams& params)
{
    DrumPattern pattern;
    pattern.numSteps = 16;
    pattern.style = "Funk";

    // Kick: syncopated funk pattern
    addHit(pattern, KICK, 0, 0.9f);
    addHit(pattern, KICK, 3, 0.7f);   // Syncopated
    addHit(pattern, KICK, 6, 0.75f);
    addHit(pattern, KICK, 10, 0.8f);

    // Snare: backbeat with ghost notes
    addHit(pattern, SNARE, 4, 0.85f);
    addHit(pattern, SNARE, 12, 0.85f);

    // Heavy ghost notes
    addHit(pattern, SNARE, 1, 0.3f);
    addHit(pattern, SNARE, 5, 0.25f);
    addHit(pattern, SNARE, 7, 0.35f);
    addHit(pattern, SNARE, 9, 0.3f);
    addHit(pattern, SNARE, 13, 0.25f);

    // Hihat: 16th notes with accents
    for (int i = 0; i < 16; ++i) {
        float vel = 0.5f;
        if (i % 4 == 0) vel = 0.75f;       // Downbeat
        else if (i % 4 == 2) vel = 0.65f;  // Offbeat accent
        addHit(pattern, HIHAT, i, vel);
    }

    addHumanization(pattern);
    return pattern;
}

DrumEngine::DrumPattern DrumEngine::generateLatinPattern(const PatternParams& params)
{
    DrumPattern pattern;
    pattern.numSteps = 16;
    pattern.style = "Latin";

    // Kick: 3-2 clave pattern
    addHit(pattern, KICK, 0, 0.85f);
    addHit(pattern, KICK, 3, 0.8f);
    addHit(pattern, KICK, 6, 0.85f);
    addHit(pattern, KICK, 10, 0.8f);
    addHit(pattern, KICK, 12, 0.85f);

    // Snare/Rim: clave accents
    addHit(pattern, SNARE, 4, 0.7f);
    addHit(pattern, SNARE, 8, 0.7f);
    addHit(pattern, SNARE, 14, 0.7f);

    // Hihat: tresillo pattern
    addHit(pattern, HIHAT, 0, 0.7f);
    addHit(pattern, HIHAT, 3, 0.65f);
    addHit(pattern, HIHAT, 6, 0.7f);
    addHit(pattern, HIHAT, 8, 0.7f);
    addHit(pattern, HIHAT, 11, 0.65f);
    addHit(pattern, HIHAT, 14, 0.7f);

    addHumanization(pattern);
    return pattern;
}

DrumEngine::DrumPattern DrumEngine::generateBluesPattern(const PatternParams& params)
{
    DrumPattern pattern;
    pattern.numSteps = 16;
    pattern.style = "Blues";

    // Kick: shuffle pattern (triplet feel)
    addHit(pattern, KICK, 0, 0.85f);
    addHit(pattern, KICK, 6, 0.75f);   // Shuffle
    addHit(pattern, KICK, 8, 0.8f);
    addHit(pattern, KICK, 14, 0.7f);

    // Snare: backbeat with shuffle
    addHit(pattern, SNARE, 4, 0.8f);
    addHit(pattern, SNARE, 12, 0.8f);

    // Hihat: shuffle pattern (approximated on 16th grid)
    for (int i = 0; i < 16; i += 3) {
        addHit(pattern, HIHAT, i, 0.7f);
        if (i + 2 < 16) {
            addHit(pattern, HIHAT, i + 2, 0.5f);  // Shuffle feel
        }
    }

    addHumanization(pattern);
    return pattern;
}
