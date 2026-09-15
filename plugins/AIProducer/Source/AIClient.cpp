#include "AIClient.h"

// Stub implementation - will be expanded to call actual AI APIs

AIClient::AIClient(Provider provider)
    : provider_(provider),
      ollamaEndpoint_("http://localhost:11434"),
      isGenerating_(false) {
}

AIClient::~AIClient() {}

void AIClient::setProvider(Provider provider) {
    provider_ = provider;
}

void AIClient::setAPIKey(const std::string& key) {
    apiKey_ = key;
}

void AIClient::setOllamaEndpoint(const std::string& endpoint) {
    ollamaEndpoint_ = endpoint;
}

void AIClient::generateTrack(const std::string& prompt,
                            std::function<void(TrackStructure)> onComplete,
                            std::function<void(std::string)> onError) {
    // For MVP: Just call the completion callback with a default structure
    // Later: Actually call AI API asynchronously

    TrackStructure structure;
    structure.genre = "House";
    structure.bpm = 128;
    structure.key = "Am";
    structure.scale = "Minor";
    structure.progression = "i-VI-III-VII";

    onComplete(structure);
}

void AIClient::cancel() {
    isGenerating_ = false;
}

void AIClient::generateWithOpenAI(const std::string& prompt,
                                 std::function<void(TrackStructure)> onComplete,
                                 std::function<void(std::string)> onError) {
    // TODO: Implement OpenAI API call
}

void AIClient::generateWithAnthropic(const std::string& prompt,
                                    std::function<void(TrackStructure)> onComplete,
                                    std::function<void(std::string)> onError) {
    // TODO: Implement Anthropic API call
}

void AIClient::generateWithOllama(const std::string& prompt,
                                 std::function<void(TrackStructure)> onComplete,
                                 std::function<void(std::string)> onError) {
    // TODO: Implement Ollama API call
}

TrackStructure AIClient::parseResponse(const std::string& jsonResponse) {
    // TODO: Parse JSON response from AI
    TrackStructure structure;
    return structure;
}
