#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <mutex>
#include <deque>
#include <functional>
#include <juce_dsp/juce_dsp.h>
#include "Mappers.h"

struct FilterCoefficients
{
    double cutoff = 2000.0;
    double resonance = 0.707;
};

struct ParameterSetupData
{
    juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>> mHighPassCoeffs;
    juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>> mHighShelfCoeffs;
    juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>> mBell1Coeffs;
    juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>> mBell2Coeffs;

    FilterCoefficients lowPassFilterCoeffs;
    FilterCoefficients highPassFilterCoeffs;
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

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    const ParameterSetupData* getAudioThreadParams() const;

private:
    void run() override;

    void initParametersListener();
    void initializeParameters();
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