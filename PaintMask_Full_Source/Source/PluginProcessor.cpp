#include "PluginProcessor.h"
#include "PluginEditor.h"

PaintMaskAudioProcessor::PaintMaskAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMS", createParameterLayout())
{
    for (const auto& param : apvts.processor.getParameters())
        apvts.addParameterListener(param->getParameterID(), this);

    rebuildGeneratedEvents();
}

PaintMaskAudioProcessor::~PaintMaskAudioProcessor()
{
    for (const auto& param : apvts.processor.getParameters())
        apvts.removeParameterListener(param->getParameterID(), this);
}

juce::AudioProcessorValueTreeState::ParameterLayout PaintMaskAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterChoice>("scanMode", "Scan Mode", juce::StringArray{ "L->R", "PingPong", "Circular", "Spiral", "Wave" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("loopBeats", "Loop Beats", juce::NormalisableRange<float>(1.0f, 16.0f, 1.0f), 4.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>("rootNote", "Root Note", 24, 84, 48));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("swing", "Swing", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("density", "Density", 0.0f, 1.0f, 1.0f));
    return { params.begin(), params.end() };
}

void PaintMaskAudioProcessor::prepareToPlay(double sampleRate, int)
{
    currentSampleRate = sampleRate;
}

void PaintMaskAudioProcessor::releaseResources() {}

bool PaintMaskAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void PaintMaskAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const auto playHead = getPlayHead();
    juce::AudioPlayHead::CurrentPositionInfo info;
    double bpm = 120.0;
    if (playHead != nullptr && playHead->getCurrentPosition(info))
    {
        if (info.bpm > 0.0)
            bpm = info.bpm;
        currentPpq = info.ppqPosition;
    }

    pushEventsForBlock(midi, buffer.getNumSamples(), bpm);
}

void PaintMaskAudioProcessor::pushEventsForBlock(juce::MidiBuffer& midi, int numSamples, double)
{
    if (generatedEvents.isEmpty())
        return;

    const auto loopBeats = *apvts.getRawParameterValue("loopBeats");
    const auto density = *apvts.getRawParameterValue("density");
    const double blockStartBeat = std::fmod(currentPpq, loopBeats);
    const double blockEndBeat = blockStartBeat + 0.25; // simple preview assumption

    for (const auto& event : generatedEvents)
    {
        if (event.isMask)
            continue;
        if (juce::Random::getSystemRandom().nextFloat() > density)
            continue;

        const double wrappedBeat = event.beatPosition;
        if (wrappedBeat >= blockStartBeat && wrappedBeat < blockEndBeat)
        {
            const auto rel = (wrappedBeat - blockStartBeat) / juce::jmax(0.001, blockEndBeat - blockStartBeat);
            const int samplePos = juce::jlimit(0, numSamples - 1, int(rel * numSamples));
            midi.addEvent(juce::MidiMessage::noteOn(1, event.note, (juce::uint8) event.velocity), samplePos);
            midi.addEvent(juce::MidiMessage::noteOff(1, event.note), juce::jmin(numSamples - 1, samplePos + 120));
        }
    }
}

juce::AudioProcessorEditor* PaintMaskAudioProcessor::createEditor()
{
    return new PaintMaskAudioProcessorEditor(*this);
}

void PaintMaskAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty("paintDocument", serializer.toJson(document), nullptr);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PaintMaskAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml == nullptr)
        return;

    const auto state = juce::ValueTree::fromXml(*xml);
    if (! state.isValid())
        return;

    apvts.replaceState(state);
    const auto json = state.getProperty("paintDocument").toString();
    if (json.isNotEmpty())
        serializer.fromJson(json, document);

    rebuildGeneratedEvents();
}

void PaintMaskAudioProcessor::rebuildGeneratedEvents()
{
    const auto modeChoice = int(*apvts.getRawParameterValue("scanMode"));
    switch (modeChoice)
    {
        case 1: scanEngine.setMode(ScanEngine::Mode::pingPong); break;
        case 2: scanEngine.setMode(ScanEngine::Mode::circular); break;
        case 3: scanEngine.setMode(ScanEngine::Mode::spiral); break;
        case 4: scanEngine.setMode(ScanEngine::Mode::wave); break;
        default: scanEngine.setMode(ScanEngine::Mode::leftToRight); break;
    }

    generatedEvents = scanEngine.buildEvents(document,
                                             *apvts.getRawParameterValue("loopBeats"),
                                             int(*apvts.getRawParameterValue("rootNote")),
                                             colourMapper);
}

void PaintMaskAudioProcessor::parameterChanged(const juce::String&, float)
{
    rebuildGeneratedEvents();
}
