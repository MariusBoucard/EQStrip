

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
    mRmsInputLevelLeft.skip(inBuffer.getNumSamples());
    {
        auto value = Decibels::gainToDecibels(inBuffer.getRMSLevel(0,0,inBuffer.getNumSamples()));
        if (value < mRmsInputLevelLeft.getCurrentValue())
        {
            mRmsInputLevelLeft.setTargetValue(value);
        }
        else
        {
            mRmsInputLevelLeft.setCurrentAndTargetValue(value);
        }
    }
    {
        auto value = Decibels::gainToDecibels(inBuffer.getRMSLevel(1,0,inBuffer.getNumSamples()));
        if (value < mRmsInputLevelRight.getCurrentValue())
        {
            mRmsInputLevelRight.setTargetValue(value);
        }
        else
        {
            mRmsInputLevelRight.setCurrentAndTargetValue(value);
        }
    }
    mProcessorGraph.processBlock(inBuffer, inMidiBuffer);

    {
        float gainInDecibels = mParameters.getParameterAsValue("outputGain").getValue();
        auto inGain = std::pow(10.0f, (gainInDecibels) / 20.0f); // If we can migrate this pow to mappers
        inBuffer.applyGain(inGain);
    }

    // Metering
    mRmsOutputLevelLeft.skip(inBuffer.getNumSamples());
    {
        auto value = Decibels::gainToDecibels(inBuffer.getRMSLevel(0,0,inBuffer.getNumSamples()));
        if (value < mRmsOutputLevelLeft.getCurrentValue())
        {
            mRmsOutputLevelLeft.setTargetValue(value);
        }
        else
        {
            mRmsOutputLevelLeft.setCurrentAndTargetValue(value);
        }
    }
    mRmsOutputLevelRight.skip(inBuffer.getNumSamples());
    {
        auto value = Decibels::gainToDecibels(inBuffer.getRMSLevel(1,0,inBuffer.getNumSamples()));
        if (value < mRmsOutputLevelRight.getCurrentValue())
        {
            mRmsOutputLevelRight.setTargetValue(value);
        }
        else
        {
            mRmsOutputLevelRight.setCurrentAndTargetValue(value);
        }
    }

    mAudioBufferFifo.push(inBuffer);
}
