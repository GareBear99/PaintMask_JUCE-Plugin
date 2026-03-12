#include "PluginEditor.h"

PaintMaskAudioProcessorEditor::PaintMaskAudioProcessorEditor(PaintMaskAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p), canvas(p.getDocument())
{
    setSize(1100, 760);

    titleLabel.setText("PaintMask", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(28.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(titleLabel);

    addAndMakeVisible(statusLabel);
    statusLabel.setJustificationType(juce::Justification::centredRight);

    addAndMakeVisible(canvas);
    canvas.setListener(this);

    for (auto* b : { &brushButton, &splashButton, &lineButton, &circleButton, &triangleButton, &maskButton, &clearButton, &unlockButton })
    {
        addAndMakeVisible(b);
        b->addListener(this);
    }

    addAndMakeVisible(colourBox);
    colourBox.addItem("Red", 1);
    colourBox.addItem("Green", 2);
    colourBox.addItem("Blue", 3);
    colourBox.addItem("Yellow", 4);
    colourBox.addItem("Magenta", 5);
    colourBox.addItem("Cyan", 6);
    colourBox.setSelectedId(1);
    colourBox.addListener(this);

    addAndMakeVisible(scanModeBox);
    scanModeBox.addItemList({ "L->R", "PingPong", "Circular", "Spiral", "Wave" }, 1);
    scanModeBox.addListener(this);
    scanModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.getValueTreeState(), "scanMode", scanModeBox);

    addAndMakeVisible(thicknessSlider);
    thicknessSlider.setRange(0.01, 0.10, 0.001);
    thicknessSlider.setValue(0.02);
    thicknessSlider.addListener(this);

    setTool(PaintStroke::Tool::brush);
    refreshStatus();
}

PaintMaskAudioProcessorEditor::~PaintMaskAudioProcessorEditor()
{
    for (auto* b : { &brushButton, &splashButton, &lineButton, &circleButton, &triangleButton, &maskButton, &clearButton, &unlockButton })
        b->removeListener(this);
}

void PaintMaskAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(11, 10, 16));

    auto area = getLocalBounds().toFloat();
    juce::ColourGradient grad(juce::Colour::fromRGB(42, 18, 42), area.getTopLeft(),
                              juce::Colour::fromRGB(8, 8, 12), area.getBottomRight(), false);
    g.setGradientFill(grad);
    g.fillRect(area.removeFromTop(80.0f));
}

void PaintMaskAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(12);
    auto header = area.removeFromTop(60);
    titleLabel.setBounds(header.removeFromLeft(260));
    statusLabel.setBounds(header.removeFromRight(340));

    auto toolbar = area.removeFromTop(42);
    auto place = [&toolbar](juce::Component& c, int w)
    {
        c.setBounds(toolbar.removeFromLeft(w).reduced(2));
    };

    place(brushButton, 82); place(splashButton, 82); place(lineButton, 72); place(circleButton, 78); place(triangleButton, 86); place(maskButton, 72);
    place(clearButton, 72);
    place(colourBox, 100);
    place(scanModeBox, 100);
    place(thicknessSlider, 130);
    place(unlockButton, 130);

    canvas.setBounds(area.reduced(0, 8));
}

void PaintMaskAudioProcessorEditor::documentChanged()
{
    processor.rebuildGeneratedEvents();
}

void PaintMaskAudioProcessorEditor::buttonClicked(juce::Button* b)
{
    if (b == &brushButton) setTool(PaintStroke::Tool::brush);
    else if (b == &splashButton) setTool(PaintStroke::Tool::splash);
    else if (b == &lineButton) setTool(PaintStroke::Tool::line);
    else if (b == &circleButton) setTool(PaintStroke::Tool::circle);
    else if (b == &triangleButton) setTool(PaintStroke::Tool::triangle);
    else if (b == &maskButton) setTool(PaintStroke::Tool::mask);
    else if (b == &clearButton)
    {
        processor.getDocument().clear();
        processor.rebuildGeneratedEvents();
        canvas.repaint();
    }
    else if (b == &unlockButton)
    {
        processor.getTrialManager().unlockStudio("PM-SUPPORTER-MANUAL");
        refreshStatus();
    }
}

void PaintMaskAudioProcessorEditor::comboBoxChanged(juce::ComboBox* box)
{
    if (box == &colourBox)
    {
        switch (colourBox.getSelectedId())
        {
            case 2: canvas.setCurrentColour(juce::Colours::green); break;
            case 3: canvas.setCurrentColour(juce::Colours::blue); break;
            case 4: canvas.setCurrentColour(juce::Colours::yellow); break;
            case 5: canvas.setCurrentColour(juce::Colours::magenta); break;
            case 6: canvas.setCurrentColour(juce::Colours::cyan); break;
            default: canvas.setCurrentColour(juce::Colours::red); break;
        }
    }
}

void PaintMaskAudioProcessorEditor::sliderValueChanged(juce::Slider* s)
{
    if (s == &thicknessSlider)
        canvas.setCurrentThickness((float) thicknessSlider.getValue());
}

void PaintMaskAudioProcessorEditor::setTool(PaintStroke::Tool tool)
{
    canvas.setCurrentTool(tool);
}

void PaintMaskAudioProcessorEditor::refreshStatus()
{
    if (processor.getTrialManager().isStudioUnlocked())
        statusLabel.setText("Studio unlocked", juce::dontSendNotification);
    else if (processor.getTrialManager().isTrialActive())
        statusLabel.setText("Studio trial: " + juce::String(processor.getTrialManager().getDaysRemaining()) + " days left", juce::dontSendNotification);
    else
        statusLabel.setText("Lite mode active", juce::dontSendNotification);
}
