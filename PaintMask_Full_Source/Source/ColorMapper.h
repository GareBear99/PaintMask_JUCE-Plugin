#pragma once
#include <JuceHeader.h>

struct PaintMappedValues
{
    float velocity = 0.7f;
    float gate = 1.0f;
    float durationBeats = 0.25f;
    int midiChannel = 1;
    int octaveBias = 0;
    float attackBias = 0.0f;
    float sustainBias = 0.0f;
    float spaceBias = 0.0f;
    juce::String family = "neutral";
};

class ColorMapper
{
public:
    PaintMappedValues mapColour(const juce::Colour& colour, float thickness, float opacity) const;
    int colourToScaleDegree(const juce::Colour& colour) const;
};
