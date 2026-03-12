#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PaintCanvas.h"

class PaintMaskAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      private PaintCanvasComponent::Listener,
                                      private juce::Button::Listener,
                                      private juce::ComboBox::Listener,
                                      private juce::Slider::Listener
{
public:
    explicit PaintMaskAudioProcessorEditor(PaintMaskAudioProcessor&);
    ~PaintMaskAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    PaintMaskAudioProcessor& processor;
    PaintCanvasComponent canvas;

    juce::TextButton brushButton { "Brush" }, splashButton { "Splash" }, lineButton { "Line" },
                     circleButton { "Circle" }, triangleButton { "Triangle" }, maskButton { "Mask" },
                     clearButton { "Clear" }, unlockButton { "Unlock Studio" };
    juce::ComboBox colourBox, scanModeBox;
    juce::Slider thicknessSlider;
    juce::Label titleLabel, statusLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scanModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thicknessAttachment;

    void documentChanged() override;
    void buttonClicked(juce::Button*) override;
    void comboBoxChanged(juce::ComboBox*) override;
    void sliderValueChanged(juce::Slider*) override;

    void setTool(PaintStroke::Tool);
    void refreshStatus();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PaintMaskAudioProcessorEditor)
};
