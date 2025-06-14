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
    mParameters.removeParameterListener("gain",this);
    mParameters.removeParameterListener("HPF_Freq",this);
    mParameters.removeParameterListener("HPF_Q",this);
    mParameters.removeParameterListener("Bell1_Freq",this);
    mParameters.removeParameterListener("Bell1_Gain",this);
    mParameters.removeParameterListener("Bell1_Q",this);
    mParameters.removeParameterListener("Bell2_Freq",this);
    mParameters.removeParameterListener("Bell2_Gain",this);
    mParameters.removeParameterListener("Bell2_Q",this);
    mParameters.removeParameterListener("HS_Freq",this);
    mParameters.removeParameterListener("HS_Gain",this);
    mParameters.removeParameterListener("HS_Q",this);
    stopThread(100);
}

ParameterSetupData ParameterSetup::createSetupData() {
    ParameterSetupData data;
    ;
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
    parameterChanged("HPF_Freq",mParameters.getRawParameterValue("HPF_Freq")->load());
    parameterChanged("HPF_Freq", mParameters.getRawParameterValue("HPF_Freq")->load());
    parameterChanged("HPF_Q", mParameters.getRawParameterValue("HPF_Q")->load());

    // Bell Filter 1 (Mid1)
    parameterChanged("Bell1_Freq", mParameters.getRawParameterValue("Bell1_Freq")->load());
    parameterChanged("Bell1_Gain", mParameters.getRawParameterValue("Bell1_Gain")->load());
    parameterChanged("Bell1_Q", mParameters.getRawParameterValue("Bell1_Q")->load());

    parameterChanged("Bell2_Freq", mParameters.getRawParameterValue("Bell2_Freq")->load());
    parameterChanged("Bell2_Gain", mParameters.getRawParameterValue("Bell2_Gain")->load());
    parameterChanged("Bell2_Q", mParameters.getRawParameterValue("Bell2_Q")->load());

    // High-Shelf Filter (HSF)
    parameterChanged("HS_Freq", mParameters.getRawParameterValue("HS_Freq")->load());
    parameterChanged("HS_Gain", mParameters.getRawParameterValue("HS_Gain")->load());
    parameterChanged("HS_Q", mParameters.getRawParameterValue("HS_Q")->load());
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

        std::smatch match;
        std::string paramIDCopyStr = paramIDCopy.toStdString();
        if (std::regex_match(paramIDCopyStr, match, std::regex("^HPF.*"))) {
            if (paramIDCopyStr == "HPF_Freq") {
                auto* hpfParam = mParameters.getRawParameterValue("HPF_Q");
                double hpfValue = hpfParam ? static_cast<double>(*hpfParam) : 2.0;
                Mappers::setHpf(paramsToUpdate->mHighPassCoeffs, static_cast<double>(newValue), hpfValue);
            }
            if (paramIDCopyStr == "HPF_Q") {
                auto* hpfParam = mParameters.getRawParameterValue("HPF_Freq");
                double hpfValue = hpfParam ? static_cast<double>(*hpfParam) : 100.0;
                Mappers::setHpf(paramsToUpdate->mHighPassCoeffs,hpfValue, static_cast<double>(newValue));
            }
            calculationPerformed = true;
        }

        if (std::regex_match(paramIDCopyStr, match, std::regex("^Bell1.*"))) {
            if (paramIDCopyStr == "Bell1_Freq") {
                auto* qParam = mParameters.getRawParameterValue("Bell1_Q");
                double q = qParam ? mParameters.getRawParameterValue("Bell1_Q")->load() : 0.707;

                auto gainParam = mParameters.getRawParameterValue("Bell1_Gain");
                double gain = gainParam ? mParameters.getRawParameterValue("Bell1_Gain")->load() : 2.0;
                Mappers::setBell(paramsToUpdate->mBell1Coeffs,newValue,q,gain);
            }
            if (paramIDCopyStr == "Bell1_Gain") {
                auto* qParam = mParameters.getRawParameterValue("Bell1_Q");
                double q = qParam ? mParameters.getRawParameterValue("Bell1_Q")->load() : 0.707;

                auto freqParam = mParameters.getRawParameterValue("Bell1_Freq") ;
                auto freq = freqParam ? mParameters.getRawParameterValue("Bell1_Freq")->load() : 2.0;
                Mappers::setBell(paramsToUpdate->mBell1Coeffs,freq,q,newValue);
            }
            if (paramIDCopyStr == "Bell1_Q") {
                auto gainParam = mParameters.getRawParameterValue("Bell1_Gain");
                double gain = gainParam ? mParameters.getRawParameterValue("Bell1_Gain")->load() : 2.0;

                auto freqParam = mParameters.getRawParameterValue("Bell1_Freq") ;
                auto freq = freqParam ? mParameters.getRawParameterValue("Bell1_Freq")->load() : 2.0;
                Mappers::setBell(paramsToUpdate->mBell1Coeffs,freq,newValue,gain);
            }
            calculationPerformed = true;
        }
        if (std::regex_match(paramIDCopyStr, match, std::regex("^Bell2.*"))) {
            if (paramIDCopyStr == "Bell2_Freq") {
                auto qParam = mParameters.getRawParameterValue("Bell2_Q");
                auto q = qParam ? mParameters.getRawParameterValue("Bell2_Q")->load() : 2.0;
                auto gainParam = mParameters.getRawParameterValue("Bell2_Gain");
                auto gain = gainParam ? mParameters.getRawParameterValue("Bell2_Gain")->load() : 2.0;
                Mappers::setBell(paramsToUpdate->mBell2Coeffs,newValue,q,gain);
            }
            if (paramIDCopyStr == "Bell2_Gain") {
                auto qParam = mParameters.getRawParameterValue("Bell2_Q");
                auto q = qParam ? mParameters.getRawParameterValue("Bell2_Q")->load() : 2.0;
                auto freqParam = mParameters.getRawParameterValue("Bell2_Freq");
                auto freq = freqParam? mParameters.getRawParameterValue("Bell2_Freq")->load() : 2.0;
                Mappers::setBell(paramsToUpdate->mBell2Coeffs,freq,q,newValue);
            }
            if (paramIDCopyStr == "Bell2_Q") {
                auto gainParam = mParameters.getRawParameterValue("Bell2_Gain");
                auto gain = gainParam ? mParameters.getRawParameterValue("Bell2_Gain")->load() : 2.0;
                auto freqParam = mParameters.getRawParameterValue("Bell2_Freq");
                auto freq = freqParam? mParameters.getRawParameterValue("Bell2_Freq")->load() : 2.0;
                Mappers::setBell(paramsToUpdate->mBell2Coeffs,freq,newValue,gain);
            }
            calculationPerformed = true;
        }

        if (std::regex_match(paramIDCopyStr, match, std::regex("^HS.*"))) {
            if (paramIDCopyStr == "HS_Freq") {
                auto qParam = mParameters.getRawParameterValue("HS_Q");
                auto q = qParam ? mParameters.getRawParameterValue("HS_Q")->load() : 2.0;
                auto gainParam = mParameters.getRawParameterValue("HS_Gain");
                auto gain = gainParam ? mParameters.getRawParameterValue("HS_Gain")->load() : 2.0;
                Mappers::setHighShelf(paramsToUpdate->mHighShelfCoeffs,newValue,q,gain);
            }
            if (paramIDCopyStr == "HS_Gain") {
                auto qParam = mParameters.getRawParameterValue("HS_Q");
                auto q = qParam ? mParameters.getRawParameterValue("HS_Q")->load() : 2.0;
                auto freqParam = mParameters.getRawParameterValue("HS_Freq");
                auto freq = freqParam ? mParameters.getRawParameterValue("HS_Freq")->load() : 2.0;
                Mappers::setHighShelf(paramsToUpdate->mHighShelfCoeffs,freq,q,newValue);
            }
            if (paramIDCopyStr == "HS_Q") {
                auto gainParam = mParameters.getRawParameterValue("HS_Gain");
                auto gain = gainParam ? mParameters.getRawParameterValue("HS_Gain")->load() : 2.0;
                auto freqParam = mParameters.getRawParameterValue("HS_Freq");
                auto freq = freqParam ? mParameters.getRawParameterValue("HS_Freq")->load() : 2.0;                Mappers::setHighShelf(paramsToUpdate->mHighShelfCoeffs,freq,newValue,gain);
            }
            calculationPerformed = true;
        }

        if (calculationPerformed) {
            std::lock_guard<std::mutex> lock(mUpdateMutex);
            paramsToUpdate->version++;
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
    ParameterSetupData *tempOldCurrentAudioParams = mCurrentParamsForAudio.load(std::memory_order_relaxed);
    mCurrentParamsForAudio.store(mNextParamsForProcessing, std::memory_order_release);
    mNextParamsForProcessing = tempOldCurrentAudioParams;
}
