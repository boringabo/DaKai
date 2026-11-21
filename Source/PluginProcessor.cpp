/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DaKaiAudioProcessor::DaKaiAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}

DaKaiAudioProcessor::~DaKaiAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout DaKaiAudioProcessor::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameters;
    
    // Input Gain
    //
    juce::NormalisableRange<float> inputGainRange(0.0f, 2.0f, 0.05f);
    inputGainRange.setSkewForCentre(1.0f);
    parameters.add(std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID("inputGain", 1),
                "Input Gain",
                inputGainRange,
                1.0f));

    // Bandwidth
    //
    juce::NormalisableRange<float> bandwidthRange(7500.0f, 44100.0f, 1.0f);
    bandwidthRange.setSkewForCentre(16000.0f);
    parameters.add(std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID("bandwidth", 1),
                "Bandwidth",
                bandwidthRange,
                44100.0f));

    juce::NormalisableRange<float> cutoffRange(80.0f, 19000.0f, 1.0f);
    cutoffRange.setSkewForCentre(2000.0f);
    parameters.add(std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID("cutoff", 1),
                "Cutoff",
                cutoffRange,
                19000.0f));

    // Out Gain
    //
    juce::NormalisableRange<float> outputGainRange(0.0f, 1.0f, 0.05f, 0.7f);
    parameters.add(std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID("outputGain", 1),
                "Output Gain",
                outputGainRange,
                1.0f));

    return parameters;
}
//==============================================================================
const juce::String DaKaiAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DaKaiAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DaKaiAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DaKaiAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DaKaiAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DaKaiAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DaKaiAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DaKaiAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DaKaiAudioProcessor::getProgramName (int index)
{
    return {};
}

void DaKaiAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DaKaiAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    //
    // Get channel count
    int numChannels = getTotalNumInputChannels();
    
    // Pass Sample Rate
    aaFilter.prepare(sampleRate);
    bandwidth.prepare(sampleRate, numChannels);
    lpfStage1.prepare(sampleRate);
    lpfStage2.prepare(sampleRate);
    lpfStage3.prepare(sampleRate);
 
    // Set Q
    lpfStage1.setQ(0.51763809f);
    lpfStage2.setQ(0.70710678f);
    lpfStage3.setQ(1.93185165f);

}

void DaKaiAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DaKaiAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DaKaiAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
   
    updateParameters();
    inputGain.process(buffer);
    bitcrusher.process(buffer);
    aaFilter.process(buffer);
    bandwidth.process(buffer);
    lpfStage1.process(buffer);
    lpfStage2.process(buffer);
    lpfStage3.process(buffer);
    outputGain.process(buffer);
}

void DaKaiAudioProcessor::updateParameters()
{
    float inputValue = apvts.getRawParameterValue("inputGain")->load();
    float outputValue = apvts.getRawParameterValue("outputGain")->load();
    float bandwidthValue = apvts.getRawParameterValue("bandwidth")->load();
    float trackingCutoff = bandwidthValue * 0.42f;
    float cutoffValue = apvts.getRawParameterValue("cutoff")->load();

    inputGain.setGain(inputValue);
    outputGain.setGain(outputValue);
    aaFilter.setFrequency(trackingCutoff);
    bandwidth.setTargetSampleRate(bandwidthValue);
    lpfStage1.setFrequency(cutoffValue);
    lpfStage2.setFrequency(cutoffValue);
    lpfStage3.setFrequency(cutoffValue);
}

//==============================================================================
bool DaKaiAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DaKaiAudioProcessor::createEditor()
{
   return new juce::GenericAudioProcessorEditor (*this);
   // return new DaKaiAudioProcessorEditor (*this);
}

//==============================================================================
void DaKaiAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DaKaiAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DaKaiAudioProcessor();
}
