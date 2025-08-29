#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    recordButton.setButtonText("Record"); 
    playButton.setButtonText("Play");
    stopButton.setButtonText("Stop");
    recordButton.onClick = [this] { startRecording(); };
    playButton.onClick = [this] { startPlayback(); }; 
    stopButton.onClick = [this] { stop(); };
    addAndMakeVisible(recordButton); 
    addAndMakeVisible(playButton); 
    addAndMakeVisible(stopButton); 

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate; 
    recordedBuffer.setSize(2, (int)(sampleRate * 10.0)); 
    recordedBuffer.clear(); 
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    const int numSamples = bufferToFill.numSamples; 
    if (isRecording && bufferToFill.buffer -> getNumChannels() >= 1)
    {
        const int remainingSpace = recordedBuffer.getNumSamples() - recordingPosition; 
        const int samplesToRecord = juce::jmin(numSamples, remainingSpace); 

        if (samplesToRecord > 0)
        {
            for (int channel = 0; channel < juce::jmin(2, bufferToFill.buffer -> getNumChannels()); ++channel)
            {
                recordedBuffer.copyFrom(channel, recordingPosition, *bufferToFill.buffer, channel, bufferToFill.startSample, samplesToRecord); 
            }
            recordingPosition += samplesToRecord; 
        }
    }

    if (isPlaying)
    {
        const int remainingSamples = recordedBuffer.getNumSamples() - playbackPosition; 
        const int samplesToPlay = juce::jmin(numSamples, remainingSamples); 

        if (samplesToPlay > 0)
        {
            for (int(channel) = 0; channel < bufferToFill.buffer -> getNumChannels(); ++channel)
            {
                const int sourceChannel = juce::jmin(channel, recordedBuffer.getNumChannels() - 1); 
                bufferToFill.buffer -> copyFrom(channel, bufferToFill.startSample, recordedBuffer, sourceChannel, playbackPosition, samplesToPlay);
            }
            playbackPosition += samplesToPlay;
            if (playbackPosition >= recordedBuffer.getNumSamples())
            {
                stop(); 
            }
        }
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    g.setColour(juce::Colours::white); 
    g.setFont(20.0f);
    g.drawFittedText("hope this works", getLocalBounds().removeFromTop(getHeight() - 120), juce::Justification::centred, 1);
}

void MainComponent::resized()
{
    //these are super off center, I gotta fix that
    recordButton.setBounds(50, getHeight() - 100, 100, 40);
    playButton.setBounds(200, getHeight() - 100, 100, 40);
    stopButton.setBounds(350, getHeight() - 100, 100, 40); 
}

void MainComponent::startRecording() 
{
    if (!isRecording) {
        recordedBuffer.clear(); 
        recordingPosition = 0; 
        isRecording = true; 
        isPlaying = false; 
        recordButton.setButtonText("Recording..."); 
    }

}

void MainComponent::startPlayback()
{
    if (!isPlaying && recordedBuffer.getNumSamples() > 0) {
        playbackPosition = 0; 
        isPlaying = true; 
        isRecording = false; 
        playButton.setButtonText("Playing..."); 
    }
}

void MainComponent::stop()
{
    isRecording = false; 
    isPlaying = false; 
    recordButton.setButtonText("Record"); 
    playButton.setButtonText("Play"); 
}