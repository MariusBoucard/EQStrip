

#include "Processor.h"
#include <filesystem>
#include "Bones/GainBone.h"
#include <iostream>



SkeletonAudioProcessor::SkeletonAudioProcessor(juce::AudioProcessorValueTreeState& inParameters, ParameterSetup& inParameterSetup)
    : AudioProcessor(BusesProperties().withInput("Input", AudioChannelSet::stereo())
        .withOutput("Output", AudioChannelSet::stereo()))
    , mParameters(inParameters)
    , mParameterSetup(inParameterSetup)
    , mBlockSize(256)
    , mSampleRate(44100)
{
}
SkeletonAudioProcessor::~SkeletonAudioProcessor()
{

}  
void SkeletonAudioProcessor::updateMeter(bool isOutput, AudioBuffer<float>& buffer,int numSamples,int numChannels)
{
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getReadPointer(channel);
        float sum = 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            sum += channelData[i] * channelData[i];
        }

        float rms = std::sqrt(sum / numSamples);

        if (channel == 0)
        {
            mRmsLevelLeft.store(rms);
            mRmsLevelRight.store(rms);
        }
    }
}

void SkeletonAudioProcessor::processBlock(AudioBuffer<float>& inBuffer, MidiBuffer& inMidiBuffer)
{
    juce::ScopedNoDenormals noDenormals;
   // inBuffer.applyGain(mParameterSetup.getAudioThreadParams()->gain);
    mProcessorGraph.processBlock(inBuffer, inMidiBuffer);
    updateMeter(true, inBuffer, inBuffer.getNumSamples(), getTotalNumOutputChannels());
}
