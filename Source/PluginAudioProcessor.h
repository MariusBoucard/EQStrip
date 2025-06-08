
/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             GainPlugin
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Gain audio plugin.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_plugin_client, juce_audio_processors,
                   juce_audio_utils, juce_core, juce_data_structures,
                   juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2022

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             AudioProcessor
 mainClass:        GainProcessor

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once
#include "ui/gui.h"
#include "dsp/Processor.h"
#include "dsp/ParameterSetup.h"

class PluginAudioProcessor final : public AudioProcessor
{
public:

    PluginAudioProcessor();
    ~PluginAudioProcessor() override;

    juce::AudioProcessorValueTreeState::ParameterLayout PluginAudioProcessor::createParameterLayout() {
        std::vector<std::unique_ptr<juce::RangedAudioParameter> > params;
        auto attributes = juce::AudioParameterFloatAttributes()
                .withStringFromValueFunction([](float value, int) {
                    return juce::String(value)+" dB";
                })
                .withLabel(" dB");

        auto attrFreq = juce::AudioParameterFloatAttributes()
                .withStringFromValueFunction([](float value, int) {
                    if (value < 1000.0f)
                        return juce::String(juce::roundToInt(value)) + " Hz";
                    else
                        return juce::String(value / 1000.0f, 2) + " kHz";
                })
                .withValueFromStringFunction(juce::AudioParameterFloatAttributes::ValueFromString(
                    [](const juce::String &text) -> float {
                        auto cleaned = text.trim().toLowerCase();
                        if (cleaned.endsWith("khz"))
                            return cleaned.dropLastCharacters(3).getFloatValue() * 1000.0f;
                        else if (cleaned.endsWith("hz"))
                            return cleaned.dropLastCharacters(2).getFloatValue();
                        return cleaned.getFloatValue();
                    }));

        auto attrQ = juce::AudioParameterFloatAttributes()
                .withStringFromValueFunction([](float value, int) {
                    return juce::String(value, 2)+" Q";
                })
                .withValueFromStringFunction(juce::AudioParameterFloatAttributes::ValueFromString(
                    [](const juce::String &text) -> float {
                        return text.trim().getFloatValue();
                    }));

        params.push_back(std::make_unique<juce::AudioParameterFloat>("inputGain",
                                                                     "Input Gain",
                                                                     juce::NormalisableRange<float>(-12.0f,
                                                                         12.0f,
                                                                         0.01),
                                                                     0.0f,
                                                                     attributes));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("outputGain",
                                                                     "Output Gain",
                                                                     juce::NormalisableRange<float>(-12.0f,
                                                                         12.0f,
                                                                         0.01),
                                                                     0.0f,
                                                                     attributes));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"HPF_Freq", 1},
            "HPF Frequency", // Parameter Name
            juce::NormalisableRange<float>(20.0f, 300.0f, 1.0f, 0.3f),
            80.0f,
            attrFreq
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"HPF_Q", 1},
            "HPF Q",
            juce::NormalisableRange<float>(0.3f, 2.0f, 0.01f),
            0.707f, // Butterworth response Q
            attrQ
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"Bell1_Freq", 1},
            "Bell 1 Frequency",
            juce::NormalisableRange<float>(200.0f, 3000.0f, 1.0f, 0.3f),
            500.0f,
            attrFreq
        ));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"Bell1_Gain", 1},
            "Bell 1 Gain",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            -10.0f, // Default gain (no change)
            attributes
        ));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"Bell1_Q", 1},
            "Bell 1 Q",
            juce::NormalisableRange<float>(0.2f, 20.0f, 0.01f),
            1.0f,
            attrQ
        ));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"Bell2_Freq", 1},
            "Bell 2 Freq",
            juce::NormalisableRange<float>(500.f, 12000.0f, 1.f, 0.3f),
            1.0f,
            attrFreq
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"Bell2_Gain", 1},
            "Bell 2 Gain",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f,
            attributes
        ));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"Bell2_Q", 1},
            "Bell 2 Q",
            juce::NormalisableRange<float>(0.2f, 20.0f, 0.01f),
            1.0f,
            attrQ
        ));


        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"HS_Freq", 1},
            "High-Shelf Frequency",
            juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f, 0.35f),
            8000.0f,
            attrFreq
        ));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"HS_Gain", 1},
            "High-Shelf Gain",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f,
            attributes
        ));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"HS_Q", 1},
            "High-Shelf Q/Slope", // Label clarifies Q is for shelf slope
            juce::NormalisableRange<float>(0.1f, 20.0f, 0.01f), // Q range for shelves often differs
            0.707f,
            attrQ
        ));
        return {params.begin(), params.end()};
    }

    void prepareToPlay (double sampleRate, int blockSize) override {
        std::cout << "prepareToPlay" << std::endl;
        mSkeletonProcessor.prepareToPlay(sampleRate, blockSize);
    }
    void releaseResources() override {}

    void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override {
       auto editor = new RootViewComponent(mSkeletonProcessor);
       // auto editor = new GenericAudioProcessorEditor(this);

        //editor->updatePath();
        return editor;
    }
    bool hasEditor() const override                        { return true;   }

    const String getName() const override                  { return "EQStrip"; }
    bool acceptsMidi() const override                      { return false; }
    bool producesMidi() const override                     { return false; }
    double getTailLengthSeconds() const override           { return 0; }

    int getNumPrograms() override                          { return 1; }
    int getCurrentProgram() override                       { return 0; }
    void setCurrentProgram (int) override                  {}
    const String getProgramName (int) override             { return "None"; }
    void changeProgramName (int, const String&) override   {}


    void getStateInformation (MemoryBlock& destData) override
    {
        juce::MemoryOutputStream stream(destData, true);
        mParameters.state.writeToStream(stream);  
    }


    void setStateInformation (const void* data, int sizeInBytes) override
    {
        juce::ValueTree tree = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes));

        if (tree.isValid())
        {
            mParameters.replaceState(tree);
            mParameterSetup.initializeParameters();
        }
    }

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        const auto& mainInLayout  = layouts.getChannelSet (true,  0);
        const auto& mainOutLayout = layouts.getChannelSet (false, 0);

        return (mainInLayout == mainOutLayout && (! mainInLayout.isDisabled()));
    }

    juce::AudioProcessorValueTreeState& PluginAudioProcessor::getCustomParameterTree()
    {
        return mParameters;
    }

private:
    juce::AudioProcessorValueTreeState mParameters; 
    SkeletonAudioProcessor mSkeletonProcessor;
    ParameterSetup mParameterSetup;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessor)
};
