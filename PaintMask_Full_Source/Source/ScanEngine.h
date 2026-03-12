#pragma once
#include <JuceHeader.h>
#include "PaintCanvas.h"
#include "ColorMapper.h"

struct PaintMidiEvent
{
    int note = 60;
    int velocity = 100;
    double beatPosition = 0.0;
    double durationBeats = 0.25;
    bool isMask = false;
};

class ScanEngine
{
public:
    enum class Mode
    {
        leftToRight,
        pingPong,
        circular,
        spiral,
        wave
    };

    void setMode(Mode m) noexcept { mode = m; }
    Mode getMode() const noexcept { return mode; }

    juce::Array<PaintMidiEvent> buildEvents(const PaintDocument&, double loopBeats, int rootNote, const ColorMapper&) const;

private:
    Mode mode = Mode::leftToRight;
};
