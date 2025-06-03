#pragma once
#include <JuceHeader.h>
#include <random>
#include "Processor.hpp"
#include "Bones/HighPassFilter.h"
//==============================================================================
class SkeletonAudioProcessor final : public AudioProcessor {
public:
    //==============================================================================
    SkeletonAudioProcessor(juce::AudioProcessorValueTreeState &inParameters, ParameterSetup &inParameterSetup);


    ~SkeletonAudioProcessor() override;


    void prepareToPlay(double, int) override {
        mSampleRate = getSampleRate();
        mBlockSize = getBlockSize();

        Mappers::getMapperInstance().setSampleRate(mSampleRate);
        mProcessorGraph.prepareToPlay(mSampleRate, mBlockSize);
    }

    void releaseResources() override {
        mProcessorGraph.releaseResources();
    }

    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &) override;

    void updateMeter(bool isOutput, AudioBuffer<float> &buffer, int numSamples, int numChannels);

    //==============================================================================
    AudioProcessorEditor *createEditor() override {
        return nullptr;
    }

    bool hasEditor() const override { return false; }

    //==============================================================================
    const String getName() const override { return "Radiator"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }

    void setCurrentProgram(int) override {
    }

    const String getProgramName(int) override { return "None"; }

    void changeProgramName(int, const String &) override {
    }


    double getRmsLevelLeft() const { return mRmsLevelLeft.load(); }
    double getRmsLevelRight() const { return mRmsLevelRight.load(); }
    double getRmsOutputLevelLeft() const { return mRmsOutputLevelLeft.load(); }
    double getRmsOutputLevelRight() const { return mRmsOutputLevelRight.load(); }

    //==============================================================================
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override {
        return true;
        const auto &mainInLayout = layouts.getChannelSet(true, 0);
        const auto &mainOutLayout = layouts.getChannelSet(false, 0);

        return (mainInLayout == mainOutLayout && (!mainInLayout.isDisabled()));
    }

    juce::AudioProcessorValueTreeState &SkeletonAudioProcessor::getCustomParameterTree() {
        return mParameters;
    }


    void initState() {
    }

    void getStateInformation(MemoryBlock &destData) override {
    }

    void setStateInformation(const void *data, int sizeInBytes) override {
        juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
        auto newState = juce::ValueTree::readFromStream(stream);
    }

    //==============================================================================
    void initialiseGraph() {
        mInputNode = mProcessorGraph.addNode(std::make_unique<AudioInputNode>(AudioInputNode::audioInputNode));
        mOutputNode = mProcessorGraph.addNode(std::make_unique<AudioOutputNode>(AudioOutputNode::audioOutputNode));
        //mGainNode = mProcessorGraph.addNode(std::make_unique<GainProcessor>(mParameters));
        mHpfNode = mProcessorGraph.addNode(std::make_unique<HighPassFilter>(mParameters,mParameterSetup));
        for (int channel = 0; channel < 2; ++channel)
        {
            mProcessorGraph.addConnection({
                {mInputNode->nodeID, channel},
                {mHpfNode->nodeID, channel}
            });

            mProcessorGraph.addConnection({
                {mHpfNode->nodeID, channel},
                {mOutputNode->nodeID, channel}
            });
        }
    }

    void setRateAndBufferSizeDetails(double sampleRate, int bufferSize) {
        mSampleRate = sampleRate;
        mBlockSize = bufferSize;
    }

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState &mParameters;
    ParameterSetup &mParameterSetup;

private:
    std::atomic<float> mRmsLevelLeft{0.0f};
    std::atomic<float> mRmsLevelRight{0.0f};
    std::atomic<float> mRmsOutputLevelLeft{0.0f};
    std::atomic<float> mRmsOutputLevelRight{0.0f};

    juce::AudioProcessorGraph mProcessorGraph;

    using AudioInputNode = juce::AudioProcessorGraph::AudioGraphIOProcessor;
    using AudioOutputNode = juce::AudioProcessorGraph::AudioGraphIOProcessor;

    // using NamNode         = VotreClasseNAMProcessor; // Hérite de juce::AudioProcessor

    juce::AudioProcessorGraph::Node::Ptr mInputNode;
    juce::AudioProcessorGraph::Node::Ptr mOutputNode;
   // juce::AudioProcessorGraph::Node::Ptr mGainNode;
    juce::AudioProcessorGraph::Node::Ptr mHpfNode;
    double mBlockSize;
    double mSampleRate;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SkeletonAudioProcessor)
};
