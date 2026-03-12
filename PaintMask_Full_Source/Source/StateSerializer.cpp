#include "StateSerializer.h"

static juce::String toolToString(PaintStroke::Tool t)
{
    switch (t)
    {
        case PaintStroke::Tool::brush: return "brush";
        case PaintStroke::Tool::splash: return "splash";
        case PaintStroke::Tool::line: return "line";
        case PaintStroke::Tool::circle: return "circle";
        case PaintStroke::Tool::triangle: return "triangle";
        case PaintStroke::Tool::star: return "star";
        case PaintStroke::Tool::spiral: return "spiral";
        case PaintStroke::Tool::mask: return "mask";
        case PaintStroke::Tool::motion: return "motion";
    }
    return "brush";
}

static PaintStroke::Tool stringToTool(const juce::String& s)
{
    if (s == "splash") return PaintStroke::Tool::splash;
    if (s == "line") return PaintStroke::Tool::line;
    if (s == "circle") return PaintStroke::Tool::circle;
    if (s == "triangle") return PaintStroke::Tool::triangle;
    if (s == "star") return PaintStroke::Tool::star;
    if (s == "spiral") return PaintStroke::Tool::spiral;
    if (s == "mask") return PaintStroke::Tool::mask;
    if (s == "motion") return PaintStroke::Tool::motion;
    return PaintStroke::Tool::brush;
}

juce::String StateSerializer::toJson(const PaintDocument& document) const
{
    juce::Array<juce::var> strokeArray;

    for (const auto& s : document.getStrokes())
    {
        auto* obj = new juce::DynamicObject();
        obj->setProperty("id", s.id.toString());
        obj->setProperty("tool", toolToString(s.tool));
        obj->setProperty("colour", s.colour.toDisplayString(true));
        obj->setProperty("thickness", s.thickness);
        obj->setProperty("isMask", s.isMask);
        obj->setProperty("seed", s.seed);

        juce::Array<juce::var> points;
        for (const auto& p : s.points)
        {
            auto* pObj = new juce::DynamicObject();
            pObj->setProperty("x", p.position.x);
            pObj->setProperty("y", p.position.y);
            pObj->setProperty("pressure", p.pressure);
            pObj->setProperty("opacity", p.opacity);
            pObj->setProperty("timestampMs", p.timestampMs);
            points.add(juce::var(pObj));
        }

        obj->setProperty("points", points);
        strokeArray.add(juce::var(obj));
    }

    auto* root = new juce::DynamicObject();
    root->setProperty("format", "paintmask-state-v1");
    root->setProperty("strokes", strokeArray);
    return juce::JSON::toString(juce::var(root), true);
}

bool StateSerializer::fromJson(const juce::String& json, PaintDocument& document) const
{
    const auto parsed = juce::JSON::parse(json);
    const auto* root = parsed.getDynamicObject();
    if (root == nullptr)
        return false;

    const auto strokesVar = root->getProperty("strokes");
    if (! strokesVar.isArray())
        return false;

    document.clear();
    const auto* arr = strokesVar.getArray();
    for (const auto& item : *arr)
    {
        const auto* obj = item.getDynamicObject();
        if (obj == nullptr)
            continue;

        PaintStroke s;
        s.id = juce::Uuid(obj->getProperty("id").toString());
        s.tool = stringToTool(obj->getProperty("tool").toString());
        s.colour = juce::Colour::fromString(obj->getProperty("colour").toString());
        s.thickness = (float) obj->getProperty("thickness");
        s.isMask = (bool) obj->getProperty("isMask");
        s.seed = (int) obj->getProperty("seed");

        if (const auto pointsVar = obj->getProperty("points"); pointsVar.isArray())
        {
            for (const auto& pItem : *pointsVar.getArray())
            {
                const auto* pObj = pItem.getDynamicObject();
                if (pObj == nullptr)
                    continue;

                PaintPoint p;
                p.position.x = (float) pObj->getProperty("x");
                p.position.y = (float) pObj->getProperty("y");
                p.pressure = (float) pObj->getProperty("pressure");
                p.opacity = (float) pObj->getProperty("opacity");
                p.timestampMs = (double) pObj->getProperty("timestampMs");
                s.points.add(p);
            }
        }

        document.addStroke(s);
    }

    return true;
}
