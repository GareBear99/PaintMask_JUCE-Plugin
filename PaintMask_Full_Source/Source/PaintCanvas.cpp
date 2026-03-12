#include "PaintCanvas.h"

void PaintDocument::clear()
{
    strokes.clear();
}

void PaintDocument::addStroke(const PaintStroke& stroke)
{
    strokes.add(stroke);
}

PaintCanvasComponent::PaintCanvasComponent(PaintDocument& d) : document(d)
{
    setOpaque(true);
}

void PaintCanvasComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(8, 8, 12));

    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour::fromRGBA(255, 255, 255, 12));
    for (int i = 1; i < 8; ++i)
    {
        const auto x = bounds.getX() + bounds.getWidth() * (float(i) / 8.0f);
        const auto y = bounds.getY() + bounds.getHeight() * (float(i) / 8.0f);
        g.drawVerticalLine(int(x), bounds.getY(), bounds.getBottom());
        g.drawHorizontalLine(int(y), bounds.getX(), bounds.getRight());
    }

    for (const auto& s : document.getStrokes())
        drawStroke(g, s, false);

    if (activeStroke.hasValue())
        drawStroke(g, *activeStroke, true);
}

void PaintCanvasComponent::resized() {}

PaintPoint PaintCanvasComponent::makePoint(const juce::MouseEvent& e) const
{
    const auto bounds = getLocalBounds().toFloat();
    auto pos = e.position;
    pos.x = juce::jlimit(bounds.getX(), bounds.getRight(), pos.x);
    pos.y = juce::jlimit(bounds.getY(), bounds.getBottom(), pos.y);

    PaintPoint p;
    p.position = { pos.x / bounds.getWidth(), pos.y / bounds.getHeight() };
    p.pressure = e.pressure > 0.0f ? e.pressure : 1.0f;
    p.opacity = 1.0f;
    p.timestampMs = juce::Time::getMillisecondCounterHiRes();
    return p;
}

void PaintCanvasComponent::mouseDown(const juce::MouseEvent& e)
{
    PaintStroke s;
    s.id = juce::Uuid();
    s.tool = currentTool;
    s.colour = currentColour;
    s.thickness = currentThickness;
    s.isMask = (currentTool == PaintStroke::Tool::mask);
    s.seed = int(juce::Time::getMillisecondCounter() & 0x7fffffff);
    s.points.add(makePoint(e));
    activeStroke = s;
    repaint();
}

void PaintCanvasComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (! activeStroke.hasValue())
        return;

    activeStroke->points.add(makePoint(e));
    repaint();
}

void PaintCanvasComponent::mouseUp(const juce::MouseEvent& e)
{
    if (! activeStroke.hasValue())
        return;

    activeStroke->points.add(makePoint(e));
    document.addStroke(*activeStroke);
    activeStroke.reset();
    if (listener != nullptr)
        listener->documentChanged();
    repaint();
}

void PaintCanvasComponent::setCurrentTool(PaintStroke::Tool newTool)
{
    currentTool = newTool;
}

void PaintCanvasComponent::setCurrentColour(juce::Colour newColour)
{
    currentColour = newColour;
}

void PaintCanvasComponent::setCurrentThickness(float newThickness)
{
    currentThickness = juce::jlimit(0.002f, 0.2f, newThickness);
}

void PaintCanvasComponent::drawStroke(juce::Graphics& g, const PaintStroke& s, bool active) const
{
    if (s.points.isEmpty())
        return;

    auto area = getLocalBounds().toFloat();
    juce::Path p;
    auto first = juce::Point<float>(s.points[0].position.x * area.getWidth(), s.points[0].position.y * area.getHeight());
    p.startNewSubPath(first);

    for (int i = 1; i < s.points.size(); ++i)
    {
        const auto point = juce::Point<float>(s.points[i].position.x * area.getWidth(), s.points[i].position.y * area.getHeight());
        p.lineTo(point);
    }

    auto c = s.isMask ? juce::Colours::black.withAlpha(0.85f)
                      : s.colour.withAlpha(active ? 0.95f : 0.75f);

    g.setColour(c);
    const float width = juce::jmax(1.0f, s.thickness * juce::jmin(area.getWidth(), area.getHeight()));
    g.strokePath(p, juce::PathStrokeType(width, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    if (! s.isMask)
    {
        g.setColour(s.colour.withAlpha(0.08f));
        g.strokePath(p, juce::PathStrokeType(width * 2.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
}
