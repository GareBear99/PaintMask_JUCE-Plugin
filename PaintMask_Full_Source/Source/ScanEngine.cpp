#include "ScanEngine.h"

juce::Array<PaintMidiEvent> ScanEngine::buildEvents(const PaintDocument& document,
                                                    double loopBeats,
                                                    int rootNote,
                                                    const ColorMapper& mapper) const
{
    juce::Array<PaintMidiEvent> out;

    for (const auto& stroke : document.getStrokes())
    {
        if (stroke.points.isEmpty())
            continue;

        for (const auto& p : stroke.points)
        {
            PaintMidiEvent e;
            const auto mapped = mapper.mapColour(stroke.colour, stroke.thickness, p.opacity);
            const int degree = mapper.colourToScaleDegree(stroke.colour);
            static constexpr int scale[] = { 0, 2, 4, 5, 7, 9, 11 };
            e.note = rootNote + scale[degree % 7] + int((1.0f - p.position.y) * 24.0f) + mapped.octaveBias * 12;
            e.velocity = juce::jlimit(1, 127, int(mapped.velocity * mapped.gate * 127.0f));
            e.durationBeats = mapped.durationBeats;
            e.isMask = stroke.isMask;

            switch (mode)
            {
                case Mode::leftToRight: e.beatPosition = p.position.x * loopBeats; break;
                case Mode::pingPong:    e.beatPosition = std::abs(std::sin(p.position.x * juce::MathConstants<double>::pi)) * loopBeats; break;
                case Mode::circular:    e.beatPosition = std::fmod(std::atan2(p.position.y - 0.5f, p.position.x - 0.5f) / juce::MathConstants<double>::twoPi + 1.0, 1.0) * loopBeats; break;
                case Mode::spiral:      e.beatPosition = juce::jlimit(0.0, loopBeats, (p.position.x + p.position.y) * 0.5 * loopBeats); break;
                case Mode::wave:        e.beatPosition = juce::jlimit(0.0, loopBeats, (p.position.x * 0.7 + std::sin(p.position.y * juce::MathConstants<float>::twoPi) * 0.15 + 0.15) * loopBeats); break;
            }

            out.add(e);
        }
    }

    std::sort(out.begin(), out.end(), [](const PaintMidiEvent& a, const PaintMidiEvent& b)
    {
        return a.beatPosition < b.beatPosition;
    });

    return out;
}
