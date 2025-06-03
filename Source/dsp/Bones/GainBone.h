#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class GainProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GainProcessor(juce::AudioProcessorValueTreeState& inParameters)
        : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                           .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
        , mParameters(inParameters)
        , gainParameter(*inParameters.getParameter("gain"))
    {
        addParameter (&gainParameter);
    }

    ~GainProcessor() override
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

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        juce::ignoreUnused (midiMessages);

        juce::ScopedNoDenormals noDenormals;
        auto totalNumInputChannels  = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();
        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear (i, 0, buffer.getNumSamples());

        const float currentGain = gainParameter.getValue();

        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                channelData[sample] *= currentGain;
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

    juce::RangedAudioParameter& gainParameter;

private:
    juce::AudioProcessorValueTreeState& mParameters;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainProcessor)
};
