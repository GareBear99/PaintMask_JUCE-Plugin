#include "ColorMapper.h"

PaintMappedValues ColorMapper::mapColour(const juce::Colour& colour, float thickness, float opacity) const
{
    PaintMappedValues v;
    const float hue = colour.getHue();
    const float sat = colour.getSaturation();
    const float bri = colour.getBrightness();

    v.velocity = juce::jlimit(0.05f, 1.0f, 0.25f + bri * 0.75f);
    v.gate = juce::jlimit(0.05f, 1.0f, opacity);
    v.durationBeats = juce::jlimit(0.0625f, 2.0f, 0.125f + thickness * 1.25f);

    if (sat < 0.1f)
    {
        v.family = "neutral";
        return v;
    }

    if (hue < 0.06f || hue > 0.93f)
    {
        v.family = "red";
        v.attackBias = 0.9f;
        v.durationBeats *= 0.7f;
    }
    else if (hue < 0.17f)
    {
        v.family = "yellow";
        v.attackBias = 0.45f;
        v.velocity = juce::jmin(1.0f, v.velocity + 0.15f);
    }
    else if (hue < 0.42f)
    {
        v.family = "green";
        v.sustainBias = 0.85f;
        v.durationBeats *= 1.2f;
    }
    else if (hue < 0.62f)
    {
        v.family = "cyan";
        v.spaceBias = 0.7f;
    }
    else if (hue < 0.77f)
    {
        v.family = "blue";
        v.spaceBias = 1.0f;
        v.velocity *= 0.9f;
    }
    else
    {
        v.family = "magenta";
        v.attackBias = 0.35f;
        v.spaceBias = 0.45f;
    }

    return v;
}

int ColorMapper::colourToScaleDegree(const juce::Colour& colour) const
{
    const auto hue = colour.getHue();
    if (hue < 1.0f / 7.0f) return 0;
    if (hue < 2.0f / 7.0f) return 1;
    if (hue < 3.0f / 7.0f) return 2;
    if (hue < 4.0f / 7.0f) return 3;
    if (hue < 5.0f / 7.0f) return 4;
    if (hue < 6.0f / 7.0f) return 5;
    return 6;
}
