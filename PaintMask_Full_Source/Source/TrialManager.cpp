#include "TrialManager.h"

static juce::File getPaintMaskStateFile()
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("PaintMask")
        .getChildFile("trial_state.json");
}

TrialManager::TrialManager() : stateFile(getPaintMaskStateFile())
{
    load();
}

void TrialManager::load()
{
    stateFile.getParentDirectory().createDirectory();

    if (stateFile.existsAsFile())
    {
        const auto json = stateFile.loadFileAsString();
        const auto parsed = juce::JSON::parse(json);
        if (! parsed.isVoid())
        {
            state = parsed;
            return;
        }
    }

    auto* obj = new juce::DynamicObject();
    obj->setProperty("firstRunMs", juce::Time::getCurrentTime().toMilliseconds());
    obj->setProperty("license", juce::String());
    state = juce::var(obj);
    save();
}

void TrialManager::save() const
{
    if (auto json = juce::JSON::toString(state); json.isNotEmpty())
        stateFile.replaceWithText(json);
}

bool TrialManager::isStudioUnlocked() const
{
    const auto license = state.getProperty("license", {}).toString();
    return license.isNotEmpty();
}

bool TrialManager::isTrialActive() const
{
    if (isStudioUnlocked())
        return true;

    const auto firstRunMs = (int64) state.getProperty("firstRunMs", 0);
    const auto nowMs = juce::Time::getCurrentTime().toMilliseconds();
    const auto days = int((nowMs - firstRunMs) / (1000 * 60 * 60 * 24));
    return days < 30;
}

int TrialManager::getDaysRemaining() const
{
    if (isStudioUnlocked())
        return 9999;

    const auto firstRunMs = (int64) state.getProperty("firstRunMs", 0);
    const auto nowMs = juce::Time::getCurrentTime().toMilliseconds();
    const auto days = int((nowMs - firstRunMs) / (1000 * 60 * 60 * 24));
    return juce::jmax(0, 30 - days);
}

void TrialManager::unlockStudio(const juce::String& key)
{
    if (auto* obj = state.getDynamicObject())
    {
        obj->setProperty("license", key.trim());
        save();
    }
}
