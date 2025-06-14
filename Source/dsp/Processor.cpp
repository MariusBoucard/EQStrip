

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
    mProcessorGraph.clear();
    mInputNode = nullptr;
    mOutputNode = nullptr;
    mHpfNode = nullptr;
    mBellNode1 = nullptr;
    mBellNode2 = nullptr;
    mHShelfNode = nullptr;
}

void SkeletonAudioProcessor::processBlock(AudioBuffer<float>& inBuffer, MidiBuffer& inMidiBuffer)
{
    juce::ScopedNoDenormals noDenormals;
    {
        float gainInDecibels = mParameters.getParameterAsValue("inputGain").getValue();
        auto inGain = std::pow(10.0f, (gainInDecibels) / 20.0f); // If we can migrate this pow to mappers
        inBuffer.applyGain(inGain);
    }
    mRmsInputLevelLeft = Decibels::gainToDecibels(inBuffer.getRMSLevel(0,0,inBuffer.getNumSamples()));
    mRmsInputLevelRight = Decibels::gainToDecibels(inBuffer.getRMSLevel(1,0,inBuffer.getNumSamples()));

    mProcessorGraph.processBlock(inBuffer, inMidiBuffer);

    {
        float gainInDecibels = mParameters.getParameterAsValue("outputGain").getValue();
        auto inGain = std::pow(10.0f, (gainInDecibels) / 20.0f); // If we can migrate this pow to mappers
        inBuffer.applyGain(inGain);
    }

    // Metering
    mRmsOutputLevelLeft = Decibels::gainToDecibels(inBuffer.getRMSLevel(0,0,inBuffer.getNumSamples()));
    mRmsOutputLevelRight = Decibels::gainToDecibels(inBuffer.getRMSLevel(1,0,inBuffer.getNumSamples()));
    mAudioBufferFifo.push(inBuffer);
}
