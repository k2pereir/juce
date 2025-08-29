#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //recording functionalities
    juce::AudioBuffer<float> recordedBuffer; 
    int recordingPosition = 0; 
    bool isRecording = false; 
    bool isPlaying = false; 
    int playbackPosition = 0; 

    //for the ui
    juce::TextButton recordButton; 
    juce::TextButton playButton; 
    juce::TextButton stopButton; 

    double currentSampleRate = 44100.0; 

    void startRecording(); 
    void startPlayback(); 
    void stop(); 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
