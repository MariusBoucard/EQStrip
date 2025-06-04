#pragma once

#include <JuceHeader.h>

#include "../ParameterSetup.h"
// Could be templated on the FILTER TYPE ! // TODO
class FilterCell : public juce::AudioProcessor {
public:
    //==============================================================================
    FilterCell(juce::AudioProcessorValueTreeState &inParameters,
                   ParameterSetup &inParameterSetup,
                   FilterType inType)
        : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true))
          , mParameters(inParameters)
          , mParameterSetup(inParameterSetup)
          , mType(inType)
    {
    }

    ~FilterCell() override
    {

    }

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = samplesPerBlock;
        spec.numChannels = getTotalNumOutputChannels();

        mHighPassFilters.prepare(spec);
        mHighPassFilters.reset();
        _sleep(20);
    }

    void releaseResources() override
    {
    }

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
      #if JucePlugin_IsMidiEffect
        juce::ignoreUnused (layouts);
        return true;
      #else
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
         && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

       #if ! JucePlugin_IsSynth
        if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
            return false;
       #endif

        return true;
      #endif
    }
   #endif

    void updateFilter()
    {
        // TODO Pass the coeff through hierarchy to factorize memory blocking.
        // This will imply overriding the processorGraph to handle the coeff passing.
        //mHighPassFilters.state = mCoefficientsProvider();
       if (mType == FilterType::HighPass) {
           *mHighPassFilters.state = *mParameterSetup.getAudioThreadParams()->mHighPassCoeffs;
       }
        if (mType == FilterType::Bell1) {
            *mHighPassFilters.state = *mParameterSetup.getAudioThreadParams()->mBell1Coeffs;
        }
        if (mType == FilterType::Bell2) {
            *mHighPassFilters.state = *mParameterSetup.getAudioThreadParams()->mBell2Coeffs;
        }
        if (mType == FilterType::HighShelf) {
            *mHighPassFilters.state = *mParameterSetup.getAudioThreadParams()->mHighShelfCoeffs;
        }
    };
    void processBlock (AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override {
        juce::ignoreUnused (midiMessages);
        auto totalNumInputChannels  = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();
        updateFilter();

        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear (i, 0, buffer.getNumSamples());

        dsp::AudioBlock <float> block (buffer);
        updateFilter();
        mHighPassFilters.process(dsp::ProcessContextReplacing<float> (block));
    }

    //==============================================================================
    bool hasEditor() const override                  { return false; }
    juce::AudioProcessorEditor* createEditor() override     { return nullptr; }

    //==============================================================================
    const juce::String getName() const override          { return JucePlugin_Name; }

    bool acceptsMidi() const override   { return false; }
    bool producesMidi() const override  { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override                     { return 1; }
    int getCurrentProgram() override                  { return 0; }
    void setCurrentProgram (int index) override       { juce::ignoreUnused (index); }
    const juce::String getProgramName (int index) override { juce::ignoreUnused (index); return {}; }
    void changeProgramName (int index, const juce::String& newName) override { juce::ignoreUnused (index, newName); }

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override
    {
        // You'll need to implement this if you want to save/restore the processor's state
        juce::ignoreUnused (destData);
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        // You'll need to implement this if you want to save/restore the processor's state
        juce::ignoreUnused (data, sizeInBytes);
    }

private:
    FilterType mType;
    juce::AudioProcessorValueTreeState& mParameters;
    ParameterSetup& mParameterSetup;
    dsp::ProcessorDuplicator <dsp::IIR::Filter<float>, dsp::IIR::Coefficients <float>> mHighPassFilters;    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterCell)
};
