#pragma once
#include <JuceHeader.h>
#include "PaintCanvas.h"

class StateSerializer
{
public:
    juce::String toJson(const PaintDocument&) const;
    bool fromJson(const juce::String&, PaintDocument&) const;
};
