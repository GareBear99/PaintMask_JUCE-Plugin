#pragma once
#include <JuceHeader.h>
#include "PaintCanvas.h"
#include "ScanEngine.h"
#include "ColorMapper.h"
#include "TrialManager.h"
#include "StateSerializer.h"

class PaintMaskAudioProcessor : public juce::AudioProcessor,
                                public juce::AudioProcessorValueTreeState::Listener
{
public:
    PaintMaskAudioProcessor();
    ~PaintMaskAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts; }
    PaintDocument& getDocument() noexcept { return document; }
    ScanEngine& getScanEngine() noexcept { return scanEngine; }
    TrialManager& getTrialManager() noexcept { return trialManager; }
    StateSerializer& getStateSerializer() noexcept { return serializer; }

    void rebuildGeneratedEvents();
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    juce::AudioProcessorValueTreeState apvts;
    PaintDocument document;
    ScanEngine scanEngine;
    ColorMapper colourMapper;
    TrialManager trialManager;
    StateSerializer serializer;
    juce::Array<PaintMidiEvent> generatedEvents;

    double currentSampleRate = 44100.0;
    double currentPpq = 0.0;

    void pushEventsForBlock(juce::MidiBuffer& midi, int numSamples, double bpm);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PaintMaskAudioProcessor)
};
