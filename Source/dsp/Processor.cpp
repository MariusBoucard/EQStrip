

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
    {
        float gainInDecibels = mParameters.getParameterAsValue("inputGain").getValue();
        auto inGain = std::pow(10.0f, (gainInDecibels) / 20.0f); // If we can migrate this pow to mappers
        inBuffer.applyGain(inGain);
    }

    mProcessorGraph.processBlock(inBuffer, inMidiBuffer);

    {
        float gainInDecibels = mParameters.getParameterAsValue("outputGain").getValue();
        auto inGain = std::pow(10.0f, (gainInDecibels) / 20.0f); // If we can migrate this pow to mappers
        inBuffer.applyGain(inGain);
    }

    // Metering
    updateMeter(true, inBuffer, inBuffer.getNumSamples(), getTotalNumOutputChannels());
    mLeftChannelFifo.update(inBuffer);
}
