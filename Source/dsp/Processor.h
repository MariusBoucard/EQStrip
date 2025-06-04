#pragma once
#include <JuceHeader.h>
#include <random>
#include "Processor.hpp"
#include "Bones/FilterCell.h"
//==============================================================================
class SkeletonAudioProcessor final : public AudioProcessor {
public:
    //==============================================================================
    SkeletonAudioProcessor(juce::AudioProcessorValueTreeState &inParameters, ParameterSetup &inParameterSetup);


    ~SkeletonAudioProcessor() override;


    void prepareToPlay(double inSampleRate, int inBlockSize) override {
        mSampleRate = inSampleRate;
        mBlockSize = inBlockSize;
        auto sp = getSampleRate();
        mProcessorGraph.clear();

        mProcessorGraph.setPlayConfigDetails (getTotalNumInputChannels(),
                                              getTotalNumOutputChannels(),
                                              mSampleRate,
                                              mBlockSize);
        initialiseGraph();
        mProcessorGraph.rebuild();
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
    const String getName() const override { return "EQStrip"; }
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

        mHpfNode = mProcessorGraph.addNode(std::make_unique<FilterCell>(mParameters, mParameterSetup,FilterType::HighPass));

        mBellNode1 = mProcessorGraph.addNode(std::make_unique<FilterCell>(mParameters, mParameterSetup,FilterType::Bell1));

        mBellNode2 = mProcessorGraph.addNode(std::make_unique<FilterCell>(mParameters, mParameterSetup,
                                                                          FilterType::Bell2));
        mHShelfNode = mProcessorGraph.addNode(std::make_unique<FilterCell>(mParameters, mParameterSetup,
                                                                 FilterType::HighShelf));

        jassert(mInputNode != nullptr);
        jassert(mOutputNode != nullptr);
         jassert(mHpfNode != nullptr);


         int numChannels = mProcessorGraph.getNumInputChannels();

        for (int channel = 0; channel < numChannels; ++channel) {
            mProcessorGraph.addConnection({
                {mInputNode->nodeID, channel},
                {mHpfNode->nodeID, channel}
            });

            mProcessorGraph.addConnection({
                {mHpfNode->nodeID, channel},
                {mBellNode1->nodeID, channel}
            });
            mProcessorGraph.addConnection({
                {mBellNode1->nodeID, channel},
                {mBellNode2->nodeID, channel}
            });
            mProcessorGraph.addConnection({
                {mBellNode2->nodeID, channel},
                {mHShelfNode->nodeID, channel}
        });

            mProcessorGraph.addConnection({
                {mHShelfNode->nodeID, channel},
                {mOutputNode->nodeID, channel}
            });
        }

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

    juce::AudioProcessorGraph::Node::Ptr mInputNode;
    juce::AudioProcessorGraph::Node::Ptr mOutputNode;
    juce::AudioProcessorGraph::Node::Ptr mHpfNode;
    juce::AudioProcessorGraph::Node::Ptr mBellNode1;
    juce::AudioProcessorGraph::Node::Ptr mBellNode2;
    juce::AudioProcessorGraph::Node::Ptr mHShelfNode;

    double mBlockSize;
    double mSampleRate;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SkeletonAudioProcessor)
};
