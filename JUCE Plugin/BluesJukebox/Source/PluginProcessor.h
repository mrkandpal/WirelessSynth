/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DspFaust.h"
#include "OSCClass.h"

//==============================================================================
/**
*/
class BluesJukeboxAudioProcessor  : public juce::AudioProcessor, juce::HighResolutionTimer, juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    BluesJukeboxAudioProcessor();
    ~BluesJukeboxAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    DspFaust dspFaust;
    
    OSCClass receiveMessage;
    void hiResTimerCallback() override;
    void mapDSP();

    std::string faustAddresses[12] = {"/exfaust100/Key_0/Trigger" ,
        "/exfaust100/Key_1/Trigger",
        "/exfaust100/Key_2/Trigger",
        "/exfaust100/Key_3/Trigger",
        "/exfaust100/Key_4/Trigger",
        "/exfaust100/Key_5/Trigger",
        "/exfaust100/Key_6/Trigger",
        "/exfaust100/Key_7/Trigger",
        "/exfaust100/Key_8/Trigger",
        "/exfaust100/Key_9/Trigger",
        "/exfaust100/Key_10/Trigger",
        "/exfaust100/Key_11/Trigger",
            };
    
     std::string frequencyAddresses[12] = {"/exfaust100/Key_0/Freq",
           "/exfaust100/Key_1/Freq",
           "/exfaust100/Key_2/Freq",
           "/exfaust100/Key_3/Freq",
           "/exfaust100/Key_4/Freq",
           "/exfaust100/Key_5/Freq",
           "/exfaust100/Key_6/Freq",
           "/exfaust100/Key_7/Freq",
           "/exfaust100/Key_8/Freq",
           "/exfaust100/Key_9/Freq",
           "/exfaust100/Key_10/Freq",
           "/exfaust100/Key_11/Freq",
       };
    
    float octaveFrequencies[12] = {32.7,34.65,36.71,38.89,41.20,43.65,46.25,49.0,51.91,55.00,58.27,61.74};
    
private:
    
    juce::AudioProcessorValueTreeState treeState;
    foleys::MagicProcessorState magicState { *this, treeState };
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() const;
    
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BluesJukeboxAudioProcessor)
};
