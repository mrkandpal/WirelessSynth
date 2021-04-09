/*
  ==============================================================================

    OSCReceive.h
    Created: 8 Jan 2021 2:08:21pm
    Author:  Devansh

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>

class OSCClass : public juce::Component, private juce::OSCReceiver, private juce::OSCReceiver::ListenerWithOSCAddress<juce::OSCReceiver::MessageLoopCallback>
{
public:
    
    short oscDataArray[12] = {0};
    short oscDataArrayZ1[12] = {0};
    bool buttonStateChanged[12];
    
    OSCClass(){
        // specify here on which UDP port number to receive incoming OSC messages
        if (!connect(9999)){
            int a = 1;
        }
            //showConnectionErrorMessage("Error: could not connect to UDP port 9999.");

        // tell the component to listen for OSC messages matching this address:
        addListener(this, "/Key");
        
    }
    
    ~OSCClass(){
        
    }
    
    void oscMessageReceived(const juce::OSCMessage& message) override
    {
        auto it = message.begin();

        for (int i = 0; i < message.size(); i++)
        {
            if (message.size() == 12)
            {
                oscDataArrayZ1[i] = oscDataArray[i];
                oscDataArray[i] = message[i].getInt32();
            }
        }
        
    }
    
    void updateKeyState(){
        for(int i = 0; i<12; ++i){
            if(oscDataArray[i] == oscDataArrayZ1[i]){
                buttonStateChanged[i] = false;
            }
            else{
                buttonStateChanged[i] = true;
            }
        }
    }

};
    
