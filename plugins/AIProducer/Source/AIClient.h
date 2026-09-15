#pragma once

#include <string>
#include <functional>
#include <memory>
#include <vector>

// AI-generated track structure
struct TrackStructure {
    std::string genre;
    int bpm;
    std::string key;           // e.g., "C", "Am", "F#"
    std::string scale;         // e.g., "Major", "Minor"
    std::string progression;   // e.g., "I-V-vi-IV"

    // Arrangement sections (in bars)
    struct Section {
        std::string type;      // "intro", "build", "drop", "breakdown", "outro"
        int startBar;
        int lengthBars;
        int energy;            // 1-10 scale
    };
    std::vector<Section> sections;

    // Instrumentation
    bool hasDrums;
    bool hasBass;
    bool hasChords;
    bool hasLead;
    bool hasPads;

    // Style notes
    std::string styleNotes;
};

class AIClient {
public:
    enum class Provider {
        OpenAI,
        Anthropic,
        OllamaLocal
    };

    AIClient(Provider provider = Provider::OllamaLocal);
    ~AIClient();

    // Set API configuration
    void setProvider(Provider provider);
    void setAPIKey(const std::string& key);
    void setOllamaEndpoint(const std::string& endpoint); // Default: http://localhost:11434

    // Generate track structure from prompt
    // This is async - completion callback is called when done
    void generateTrack(const std::string& prompt,
                      std::function<void(TrackStructure)> onComplete,
                      std::function<void(std::string)> onError);

    // Cancel ongoing generation
    void cancel();

    // Check if generation is in progress
    bool isGenerating() const { return isGenerating_; }

private:
    void generateWithOpenAI(const std::string& prompt,
                           std::function<void(TrackStructure)> onComplete,
                           std::function<void(std::string)> onError);

    void generateWithAnthropic(const std::string& prompt,
                              std::function<void(TrackStructure)> onComplete,
                              std::function<void(std::string)> onError);

    void generateWithOllama(const std::string& prompt,
                           std::function<void(TrackStructure)> onComplete,
                           std::function<void(std::string)> onError);

    TrackStructure parseResponse(const std::string& jsonResponse);

    Provider provider_;
    std::string apiKey_;
    std::string ollamaEndpoint_;
    bool isGenerating_;

    // HTTP client will be implemented with JUCE's URL class
    std::unique_ptr<class HTTPClient> httpClient_;
};
