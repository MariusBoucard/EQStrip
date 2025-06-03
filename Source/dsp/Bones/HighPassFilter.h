#pragma once

#include <JuceHeader.h>

#include "../ParameterSetup.h"

//==============================================================================
/**
*/
class HighPassFilter  : public juce::AudioProcessor
{
public:
    //==============================================================================
    HighPassFilter(juce::AudioProcessorValueTreeState& inParameters, ParameterSetup& inParameterSetup)
        : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                           .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
        , mParameters(inParameters)
        , mParameterSetup(inParameterSetup)
    {
    }

    ~HighPassFilter() override
    {

    }

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::ignoreUnused (sampleRate, samplesPerBlock);
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

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override {
        juce::ignoreUnused (midiMessages);
        auto coeffs = mParameterSetup.getAudioThreadParams()->mHighPassCoeffs;

        if (coeffs.begin() != coeffs.end()) {
            auto it = coeffs.begin();
            for (int ch = 0; ch < 2; ++ch) {
                *mHighPassFilters[ch].coefficients = **it;  // double dereference: iterator -> shared_ptr -> object
            }
        }
        if (coeffs.begin() != coeffs.end()) {
            juce::dsp::AudioBlock<float> block(buffer);
            // Process each channel separately
            for (size_t ch = 0; ch < 2; ++ch) {
                auto channelBlock = block.getSingleChannelBlock(ch);
                juce::dsp::ProcessContextReplacing<float> context(channelBlock);
            //    mHighPassFilters[ch].process(context);
            }
        }

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
    juce::AudioProcessorValueTreeState& mParameters;
    ParameterSetup& mParameterSetup;
    std::array<juce::dsp::IIR::Filter<float>, 2> mHighPassFilters;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighPassFilter)
};
