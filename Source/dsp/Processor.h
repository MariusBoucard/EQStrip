#pragma once
#include <JuceHeader.h>
#include <random>
#include "Processor.hpp"
#include "Bones/FilterCell.h"
#include "OutputData.h"
//==============================================================================
class SkeletonAudioProcessor final : public AudioProcessor {
public:
    //==============================================================================
    SkeletonAudioProcessor(juce::AudioProcessorValueTreeState &inParameters, ParameterSetup &inParameterSetup);


    ~SkeletonAudioProcessor() override;


    void prepareToPlay(double inSampleRate, int inBlockSize) override {
        mSampleRate = inSampleRate;

        mBlockSize = inBlockSize;
        mProcessorGraph.clear();

        mProcessorGraph.setPlayConfigDetails (getTotalNumInputChannels(),
                                              getTotalNumOutputChannels(),
                                              mSampleRate,
                                              mBlockSize);
        initialiseGraph();
        mProcessorGraph.rebuild();
        mAudioBufferFifo.prepare(2,inBlockSize); // Carefull two channels there
        Mappers::getMapperInstance().setSampleRate(mSampleRate);
        mParameterSetup.initializeParameters();
        mProcessorGraph.prepareToPlay(mSampleRate, mBlockSize);

        mRmsInputLevelLeft.reset(inSampleRate, 0.2f);
        mRmsInputLevelRight.reset(inSampleRate, 0.2f);
        mRmsOutputLevelLeft.reset(inSampleRate, 0.2f);
        mRmsOutputLevelRight.reset(inSampleRate, 0.2f);

        mRmsInputLevelLeft.setCurrentAndTargetValue(-100.f);
        mRmsInputLevelRight.setCurrentAndTargetValue(-100.f);
        mRmsOutputLevelLeft.setCurrentAndTargetValue(-100.0f);
        mRmsOutputLevelRight.setCurrentAndTargetValue(-100.0f);
    }

    void releaseResources() override {
        mProcessorGraph.releaseResources();
    }

    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &) override;

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


    double getRmsInputLevelLeft() const { return mRmsInputLevelLeft.getCurrentValue(); }
    double getRmsInputLevelRight() const { return mRmsInputLevelLeft.getCurrentValue(); }
    double getRmsOutputLevelLeft() const { return mRmsOutputLevelLeft.getCurrentValue(); }
    double getRmsOutputLevelRight() const { return mRmsOutputLevelRight.getCurrentValue(); }
    ParameterSetup& getParameterSetup() { return mParameterSetup; }
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

    AudioBufferFifo &getAudioBufferFifo() {
        return mAudioBufferFifo;
    }

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState &mParameters;
    ParameterSetup &mParameterSetup;

private:
    LinearSmoothedValue<float> mRmsInputLevelLeft{0.0f};
    LinearSmoothedValue<float> mRmsInputLevelRight{0.0f};
    LinearSmoothedValue<float> mRmsOutputLevelLeft{0.0f};
    LinearSmoothedValue<float> mRmsOutputLevelRight{0.0f};

    juce::AudioProcessorGraph mProcessorGraph;

    using AudioInputNode = juce::AudioProcessorGraph::AudioGraphIOProcessor;
    using AudioOutputNode = juce::AudioProcessorGraph::AudioGraphIOProcessor;

    juce::AudioProcessorGraph::Node::Ptr mInputNode;
    juce::AudioProcessorGraph::Node::Ptr mOutputNode;
    juce::AudioProcessorGraph::Node::Ptr mHpfNode;
    juce::AudioProcessorGraph::Node::Ptr mBellNode1;
    juce::AudioProcessorGraph::Node::Ptr mBellNode2;
    juce::AudioProcessorGraph::Node::Ptr mHShelfNode;

    AudioBufferFifo mAudioBufferFifo;

    double mBlockSize;
    double mSampleRate;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SkeletonAudioProcessor)
};
