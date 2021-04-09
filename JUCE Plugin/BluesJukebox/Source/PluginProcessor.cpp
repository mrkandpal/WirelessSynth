/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BluesJukeboxAudioProcessor::BluesJukeboxAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       
                       ), treeState(*this, nullptr, ProjectInfo::projectName, createParameterLayout())
#endif
{
    dspFaust.start();
    startTimer(1);
    treeState.addParameterListener("OctaveChanger", this);
}

BluesJukeboxAudioProcessor::~BluesJukeboxAudioProcessor()
{
    dspFaust.stop();
}

//==============================================================================
const juce::String BluesJukeboxAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BluesJukeboxAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BluesJukeboxAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BluesJukeboxAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BluesJukeboxAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BluesJukeboxAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BluesJukeboxAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BluesJukeboxAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BluesJukeboxAudioProcessor::getProgramName (int index)
{
    return {};
}

void BluesJukeboxAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BluesJukeboxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void BluesJukeboxAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BluesJukeboxAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void BluesJukeboxAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool BluesJukeboxAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BluesJukeboxAudioProcessor::createEditor()
{
    return new foleys::MagicPluginEditor (magicState);

}

//==============================================================================
void BluesJukeboxAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BluesJukeboxAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BluesJukeboxAudioProcessor();
}

void BluesJukeboxAudioProcessor::hiResTimerCallback(){
    receiveMessage.updateKeyState();
    mapDSP();
}

void BluesJukeboxAudioProcessor::mapDSP(){
    for(int i = 0; i<12; i++){
        if(receiveMessage.buttonStateChanged[i]){
            dspFaust.setParamValue(faustAddresses[i].c_str(), receiveMessage.oscDataArray[i]);
        }
    }
}


juce::AudioProcessorValueTreeState::ParameterLayout BluesJukeboxAudioProcessor::createParameterLayout() const{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    
    params.add(std::make_unique<juce::AudioParameterInt>("OctaveChanger", "Octave", 1, 8, 4));
    
    return params;
}

void BluesJukeboxAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue){
    if(parameterID == "OctaveChanger"){
        for (int i = 0; i<12; i++){
            dspFaust.setParamValue(frequencyAddresses[i].c_str(), (newValue) * octaveFrequencies[i]);
        }
    }
}

