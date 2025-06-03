#include "ParameterSetup.h"
#include <regex>

ParameterSetup::ParameterSetup(juce::AudioProcessorValueTreeState &inApvts)
    : juce::Thread("MappersProcessingThread")
      , mParameters(inApvts)
      , mSetupData1(createSetupData())
      , mSetupData2(createSetupData())
{
    mCurrentParamsForAudio.store(&mSetupData1, std::memory_order_relaxed);
    mNextParamsForProcessing = &mSetupData2;
    initializeParameters();
    *mCurrentParamsForAudio.load(std::memory_order_relaxed) = *mNextParamsForProcessing;

    initParametersListener();
    startThread();
}

ParameterSetup::~ParameterSetup() {
    mTasksEvent.signal();
    mParameters.removeParameterListener("gain", this);
    mParameters.removeParameterListener("lowPassCutoff", this);
    mParameters.removeParameterListener("highPassResonance", this);
    stopThread(10);
}

ParameterSetupData ParameterSetup::createSetupData() {
    ParameterSetupData data;

    data.highPassFilterCoeffs = {0.0, 0.0};
    data.gain = 0.0;
    return data;
}

void ParameterSetup::initParametersListener() {
    mParameters.addParameterListener("gain", this);
    mParameters.addParameterListener("HPF_Freq", this);
    mParameters.addParameterListener("HPF_Q", this);

    // Listeners for Bell Filter 1 parameters
    mParameters.addParameterListener("Bell1_Freq", this);
    mParameters.addParameterListener("Bell1_Gain", this);
    mParameters.addParameterListener("Bell1_Q", this);

    // Listeners for Bell Filter 2 parameters
    mParameters.addParameterListener("Bell2_Freq", this);
    mParameters.addParameterListener("Bell2_Gain", this);
    mParameters.addParameterListener("Bell2_Q", this);

    // Listeners for High-Shelf Filter parameters
    mParameters.addParameterListener("HS_Freq", this);
    mParameters.addParameterListener("HS_Gain", this);
    mParameters.addParameterListener("HS_Q", this);
}

void ParameterSetup::initializeParameters() {
    auto *gainParam = mParameters.getRawParameterValue("gain");
    parameterChanged("gain",gainParam->load()) ;

    if (auto *lpCutoffParam = mParameters.getRawParameterValue("lowPassCutoff"))
        mNextParamsForProcessing->lowPassFilterCoeffs.cutoff = static_cast<double>(lpCutoffParam->load());

    if (auto *hpResonanceParam = mParameters.getRawParameterValue("highPassResonance"))
        mNextParamsForProcessing->highPassFilterCoeffs.resonance = static_cast<double>(hpResonanceParam->load());

    mNextParamsForProcessing->version = 0;
}

const ParameterSetupData *ParameterSetup::getAudioThreadParams() const {
    return mCurrentParamsForAudio.load(std::memory_order_acquire);
}

void ParameterSetup::parameterChanged(const juce::String &parameterID, float newValue) {
    juce::String paramIDCopy = parameterID;

    MapperTask task = [this, paramIDCopy, newValue]() {
        ParameterSetupData *paramsToUpdate = mNextParamsForProcessing; {
            std::lock_guard<std::mutex> lock(mUpdateMutex);
            *paramsToUpdate = *mCurrentParamsForAudio.load(std::memory_order_relaxed);
        }

        bool calculationPerformed = false;

        if (paramIDCopy == "gain") {
            Mappers::setGain(paramsToUpdate->gain, static_cast<double>(newValue));
            calculationPerformed = true;
        }
        std::smatch match;
        std::string paramIDCopyStr = paramIDCopy.toStdString();
        if (std::regex_match(paramIDCopyStr, match, std::regex("^HPF.*"))) {
            if (paramIDCopyStr == "HPF_Freq") {

                auto* hpfParam = mParameters.getRawParameterValue("HPF_Q");
                double hpfValue = hpfParam ? static_cast<double>(*hpfParam) : 0.0;
                Mappers::setHpf(paramsToUpdate->mHighPassCoeffs, static_cast<double>(newValue), hpfValue);
            }
            if (paramIDCopyStr == "HPF_Q") {
                auto* hpfParam = mParameters.getRawParameterValue("HPF_Freq");
                double hpfValue = hpfParam ? static_cast<double>(*hpfParam) : 0.0;
                Mappers::setHpf(paramsToUpdate->mHighPassCoeffs,hpfValue, static_cast<double>(newValue));
            }

            calculationPerformed = true;
        }
        if (calculationPerformed) {
            std::lock_guard<std::mutex> lock(mUpdateMutex);
            paramsToUpdate->version++; // Increment the version of the data that's about to become current.
            performSwap();
        }
    }; {
        std::lock_guard<std::mutex> lock(mTasksQueueMutex);
        mTaskQueue.push_back(std::move(task));
    }
    mTasksEvent.signal();
}

void ParameterSetup::run() {
    while (!threadShouldExit()) {
        mTasksEvent.wait(-1);

        if (threadShouldExit())
            break;

        // Process all tasks currently in the queue.
        while (true) {
            MapperTask taskToExecute; {
                std::lock_guard<std::mutex> lock(mTasksQueueMutex);
                if (mTaskQueue.empty())
                    break;

                taskToExecute = std::move(mTaskQueue.front());
                mTaskQueue.pop_front();
            }

            if (taskToExecute) {
                taskToExecute();
            }
        }
    }
}

void ParameterSetup::performSwap() {
    // mNextParamsForProcessing (which has just been updated) becomes the new mCurrentParamsForAudio.
    // The old mCurrentParamsForAudio becomes the new mNextParamsForProcessing, ready for the next update cycle.
    ParameterSetupData *tempOldCurrentAudioParams = mCurrentParamsForAudio.load(std::memory_order_relaxed);
    mCurrentParamsForAudio.store(mNextParamsForProcessing, std::memory_order_release);
    mNextParamsForProcessing = tempOldCurrentAudioParams;
}
