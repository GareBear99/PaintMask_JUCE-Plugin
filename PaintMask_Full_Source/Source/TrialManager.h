#pragma once
#include <JuceHeader.h>

class TrialManager
{
public:
    TrialManager();

    bool isStudioUnlocked() const;
    bool isTrialActive() const;
    int getDaysRemaining() const;
    void unlockStudio(const juce::String& key);

private:
    juce::File stateFile;
    juce::var state;

    void load();
    void save() const;
};
