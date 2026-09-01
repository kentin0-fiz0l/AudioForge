#include <juce_audio_processors/juce_audio_processors.h>
#include "Sample.h"
#include "SampleLibrary.h"
#include "KeyZone.h"
#include "SamplerVoice.h"
#include "PluginProcessor.h"

/**
 * Sampler Plugin Unit Tests
 *
 * Basic test suite for the sampler plugin.
 * Tests core functionality without requiring external audio files.
 */
class SamplerPluginTests : public juce::UnitTest
{
public:
    SamplerPluginTests() : UnitTest("Sampler Plugin Tests") {}

    void runTest() override
    {
        // SampleLibrary Tests
        beginTest("SampleLibrary: Initial state");
        testLibraryInitialState();

        beginTest("SampleLibrary: Memory budget constant");
        testLibraryMemoryBudget();

        // KeyZone Tests
        beginTest("KeyZone: Construction");
        testKeyZoneConstruction();

        beginTest("KeyZone: Velocity layers");
        testKeyZoneVelocityLayers();

        // KeyZoneMap Tests
        beginTest("KeyZoneMap: Add/remove zones");
        testKeyZoneMapOperations();

        beginTest("KeyZoneMap: Zone lookup");
        testKeyZoneMapLookup();

        // SamplerVoice Tests
        beginTest("SamplerVoice: Initial state");
        testVoiceInitialState();

        // PluginProcessor Tests
        beginTest("PluginProcessor: Parameter creation");
        testProcessorParameters();

        beginTest("PluginProcessor: State serialization");
        testProcessorState();
    }

private:
    //==============================================================================
    // SampleLibrary Tests

    void testLibraryInitialState()
    {
        SampleLibrary library;

        expectEquals(library.getNumSamples(), 0, "Library should start empty");
        expectEquals((int)library.getTotalMemoryUsed(), 0, "Memory should be zero");
    }

    void testLibraryMemoryBudget()
    {
        SampleLibrary library;

        // Verify 512 MB memory budget
        expect(SampleLibrary::MAX_MEMORY_BYTES == 512 * 1024 * 1024,
               "Memory budget should be 512 MB");
    }

    //==============================================================================
    // KeyZone Tests

    void testKeyZoneConstruction()
    {
        KeyZone zone(60, 72, 60);  // C4 to C5, root at C4

        expectEquals(zone.noteMin, 60, "Note min should be 60");
        expectEquals(zone.noteMax, 72, "Note max should be 72");
        expectEquals(zone.rootNote, 60, "Root note should be 60");
    }

    void testKeyZoneVelocityLayers()
    {
        KeyZone zone(60, 72, 60);

        // Add velocity layers
        VelocityLayer layer1;
        layer1.sampleIndex = 0;
        layer1.velocityMin = 0.0f;
        layer1.velocityMax = 0.5f;
        zone.velocityLayers.push_back(layer1);

        VelocityLayer layer2;
        layer2.sampleIndex = 1;
        layer2.velocityMin = 0.5f;
        layer2.velocityMax = 1.0f;
        zone.velocityLayers.push_back(layer2);

        expectEquals((int)zone.velocityLayers.size(), 2, "Should have 2 layers");

        // Test velocity lookup
        int softSample = zone.getSampleForVelocity(0.3f);
        expectEquals(softSample, 0, "Soft velocity should select layer 1");

        int hardSample = zone.getSampleForVelocity(0.8f);
        expectEquals(hardSample, 1, "Hard velocity should select layer 2");
    }

    //==============================================================================
    // KeyZoneMap Tests

    void testKeyZoneMapOperations()
    {
        KeyZoneMap zoneMap;

        expectEquals(zoneMap.getNumZones(), 0, "Should start with 0 zones");

        KeyZone zone(60, 72, 60);
        zoneMap.addZone(zone);

        expectEquals(zoneMap.getNumZones(), 1, "Should have 1 zone after adding");

        zoneMap.removeZone(0);
        expectEquals(zoneMap.getNumZones(), 0, "Should have 0 zones after removal");
    }

    void testKeyZoneMapLookup()
    {
        KeyZoneMap zoneMap;

        KeyZone zone1(36, 48, 36);  // Low zone
        KeyZone zone2(60, 72, 60);  // Mid zone

        zoneMap.addZone(zone1);
        zoneMap.addZone(zone2);

        auto* lowZone = zoneMap.getZoneForNote(40);
        expect(lowZone != nullptr, "Should find zone for note 40");
        expectEquals(lowZone->rootNote, 36, "Should return low zone");

        auto* midZone = zoneMap.getZoneForNote(64);
        expect(midZone != nullptr, "Should find zone for note 64");
        expectEquals(midZone->rootNote, 60, "Should return mid zone");

        auto* noZone = zoneMap.getZoneForNote(100);
        expect(noZone == nullptr, "Should not find zone for unmapped note");
    }

    //==============================================================================
    // SamplerVoice Tests

    void testVoiceInitialState()
    {
        SamplerVoice voice;

        expect(!voice.isActive(), "Voice should start inactive");
        expectEquals(voice.getMidiNote(), -1, "MIDI note should be -1");

        voice.noteOn(60, 0.8f, 44100.0);
        expect(voice.isActive(), "Voice should be active after note on");
        expectEquals(voice.getMidiNote(), 60, "MIDI note should be 60");

        voice.reset();
        expect(!voice.isActive(), "Voice should be inactive after reset");
    }

    //==============================================================================
    // PluginProcessor Tests

    void testProcessorParameters()
    {
        SamplerPluginProcessor processor;
        auto& apvts = processor.getAPVTS();

        // Check key parameters exist
        auto* attackParam = apvts.getParameter(SamplerPluginProcessor::PARAM_AMP_ATTACK);
        expect(attackParam != nullptr, "Attack parameter should exist");

        auto* cutoffParam = apvts.getParameter(SamplerPluginProcessor::PARAM_FILTER_CUTOFF);
        expect(cutoffParam != nullptr, "Cutoff parameter should exist");
    }

    void testProcessorState()
    {
        SamplerPluginProcessor processor;

        // Set a parameter value
        auto& apvts = processor.getAPVTS();
        apvts.getParameter(SamplerPluginProcessor::PARAM_AMP_ATTACK)->setValueNotifyingHost(0.5f);

        // Save state
        juce::MemoryBlock stateBlock;
        processor.getStateInformation(stateBlock);

        expect(stateBlock.getSize() > 0, "State block should not be empty");

        // Create new processor and restore state
        SamplerPluginProcessor processor2;
        processor2.setStateInformation(stateBlock.getData(), (int)stateBlock.getSize());

        // Verify parameter was restored
        auto& apvts2 = processor2.getAPVTS();
        float restoredValue = apvts2.getParameter(SamplerPluginProcessor::PARAM_AMP_ATTACK)->getValue();
        expectWithinAbsoluteError(restoredValue, 0.5f, 0.01f, "Parameter should be restored");
    }
};

// Register the test
static SamplerPluginTests samplerTests;
