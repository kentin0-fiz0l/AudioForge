#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "MIDILearnManager.h"

namespace AudioForge {

/**
 * Enhanced slider with MIDI learn capability via right-click menu
 */
class MIDILearnSlider : public juce::Slider, private juce::Timer {
public:
    MIDILearnSlider() : midiLearnManager_(nullptr) {}

    MIDILearnSlider(MIDILearnManager* manager, const juce::String& parameterID)
        : midiLearnManager_(manager), parameterID_(parameterID) {
        setupCallbacks();
    }

    void setMidiLearnManager(MIDILearnManager* manager) {
        midiLearnManager_ = manager;
        if (midiLearnManager_ && !parameterID_.isEmpty())
            setupCallbacks();
    }

    void setParameterID(const juce::String& id) {
        parameterID_ = id;
        if (midiLearnManager_ && !parameterID_.isEmpty())
            setupCallbacks();
    }

private:
    MIDILearnManager* midiLearnManager_;
    juce::String parameterID_;

    void setupCallbacks() {
        if (!midiLearnManager_) return;

        // Set up callbacks for visual feedback
        midiLearnManager_->onMappingCreated = [this](const juce::String& id, int cc) {
            if (id == parameterID_) {
                updateMappingLabel();
                repaint();
            }
        };

        midiLearnManager_->onMappingRemoved = [this](const juce::String& id) {
            if (id == parameterID_) {
                updateMappingLabel();
                repaint();
            }
        };
    }
    
    void mouseDown(const juce::MouseEvent& e) override {
        if (e.mods.isPopupMenu()) {
            showMIDILearnMenu();
        } else {
            juce::Slider::mouseDown(e);
        }
    }
    
    void paint(juce::Graphics& g) override {
        juce::Slider::paint(g);
        
        // Visual indicator if parameter is mapped
        if (midiLearnManager_->hasMapping(parameterID_)) {
            g.setColour(juce::Colours::lightgreen.withAlpha(0.3f));
            g.fillRect(getLocalBounds().removeFromTop(3));
        }
        
        // Visual indicator during learning mode
        if (midiLearnManager_->isLearning() && 
            midiLearnManager_->getLearningParameterID() == parameterID_) {
            g.setColour(juce::Colours::orange.withAlpha(0.5f));
            g.drawRect(getLocalBounds(), 2);
        }
    }
    
    void showMIDILearnMenu() {
        if (!midiLearnManager_) return;

        juce::PopupMenu menu;
        
        bool hasMIDIMapping = midiLearnManager_->hasMapping(parameterID_);
        bool isLearning = midiLearnManager_->isLearning();
        
        if (hasMIDIMapping) {
            auto* mapping = midiLearnManager_->getMapping(parameterID_);
            juce::String mappingText = "Mapped to CC " + juce::String(mapping->midiCC);
            menu.addSectionHeader(mappingText);
            menu.addSeparator();
            menu.addItem(2, "Remove MIDI Mapping");
            menu.addItem(1, "Re-learn MIDI CC");
        } else {
            menu.addItem(1, "MIDI Learn...", !isLearning);
        }
        
        menu.showMenuAsync(juce::PopupMenu::Options(), [this, hasMIDIMapping](int result) {
            if (result == 1) {
                // Start MIDI learn
                midiLearnManager_->startLearning(parameterID_);
                startTimer(30); // Visual feedback at 30fps
            } else if (result == 2) {
                // Remove mapping
                midiLearnManager_->removeMapping(parameterID_);
            }
        });
    }
    
    void updateMappingLabel() {
        // Trigger repaint to update visual indicators
        repaint();
    }
    
    void timerCallback() override {
        repaint(); // Update learning mode indicator
        
        if (!midiLearnManager_->isLearning()) {
            stopTimer();
        }
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDILearnSlider)
};

} // namespace AudioForge
