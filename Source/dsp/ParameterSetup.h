#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <mutex>
#include <deque>
#include <functional>
#include <juce_dsp/juce_dsp.h>
#include "Mappers.h"

enum class FilterType
{
    HighPass,
    LowPass,
    Bell1,
    Bell2,
    HighShelf
};

struct ParameterSetupData
{
    juce::ReferenceCountedObjectPtr<juce::dsp::IIR::Coefficients<float>> mHighPassCoeffs;
    juce::ReferenceCountedObjectPtr<juce::dsp::IIR::Coefficients<float>> mHighShelfCoeffs;
    juce::ReferenceCountedObjectPtr<juce::dsp::IIR::Coefficients<float>> mBell1Coeffs;
    juce::ReferenceCountedObjectPtr<juce::dsp::IIR::Coefficients<float>> mBell2Coeffs;

    double gain = 0.0;
    uint64_t version = 0;
};

class ParameterSetup : public juce::Thread, public juce::AudioProcessorValueTreeState::Listener
{
public:
    using MapperTask = std::function<void()>;

    ParameterSetup(juce::AudioProcessorValueTreeState& inApvts);
    ~ParameterSetup() override;

    ParameterSetupData createSetupData();

    void parameterChanged(const juce::String& parameterID, float newValue) override; // TODO Move this logic to another class

    const ParameterSetupData* getAudioThreadParams() const;

    void initializeParameters();

private:
    void run() override;

    void initParametersListener();

    void performSwap();

    ParameterSetupData mSetupData1;
    ParameterSetupData mSetupData2;

    juce::AudioProcessorValueTreeState& mParameters;

    std::atomic<ParameterSetupData*> mCurrentParamsForAudio;
    ParameterSetupData* mNextParamsForProcessing;

    std::mutex mUpdateMutex;

    std::deque<MapperTask> mTaskQueue;
    std::mutex mTasksQueueMutex;
    juce::WaitableEvent mTasksEvent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterSetup)
};