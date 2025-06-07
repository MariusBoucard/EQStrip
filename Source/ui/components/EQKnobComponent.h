#pragma once
#include <JuceHeader.h>
#include "../../dsp/Processor.h"
#include "../../looknfeel/StripKnob_look_n_feel.h"

class EQKnobComponent : public juce::Component
{
    struct KnobLayout {
        struct input {
            int x;
            int y;
            int frameWidth ;
            int frameHeight;
            float ratio;
            int textboxHeight;
            int textboxPadding;
        };
        struct output {
            int x;
            int y;
            int sliderWidth;
            int sliderHeight;
        };
        input inLayout;
        output outLayout;
    };
public:
    EQKnobComponent(juce::AudioProcessor& processor)
        : mProcessor(processor)
    , mScale(0.5)
    {
        mBell1Freq.setRange(500.f, 3000.0f); // Example, adapt to your parameter range
        defineKnobLayout();
        configureNodes(processor);
        setSliderAttachement(processor);


    }

    ~EQKnobComponent() override
    {
        setLookAndFeel(nullptr);
    }
    void computeKnobLayout(KnobLayout& inKnobLayout)
    {

        inKnobLayout.outLayout.sliderWidth = inKnobLayout.inLayout.frameWidth *mScale;
        inKnobLayout.outLayout.sliderHeight = inKnobLayout.inLayout.frameHeight * mScale;
        inKnobLayout.outLayout.x = inKnobLayout.inLayout.x*mScale;
        inKnobLayout.outLayout.y = inKnobLayout.inLayout.y *mScale;
    }


    void EQKnobComponent::paint(juce::Graphics& g)
    {
        g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
        juce::Rectangle<float> bounds(0, 0, getWidth(), getHeight());
        g.fillRoundedRectangle(bounds, 5.0f);

    }

    void setScale(int inScale) {
        mScale = inScale;
    }
    void resized() override
    {
        auto buttonSize = getHeight();

    }

private:
    KnobLookAndFeel mButtonLookNFeel;

    juce::AudioProcessor& mProcessor;

    juce::Slider mLCFreq;
    juce::Slider mHSFreq;
    juce::Slider mBell1Freq;
    juce::Slider mBell2Freq;

    juce::Slider mLCQ;
    juce::Slider mHSCQ;
    juce::Slider mBell1CQ;
    juce::Slider mBell2CQ;

    juce::Slider mHSHighGain;
    juce::Slider mBell1HighGain;
    juce::Slider mBell2HighGain;

    double mScale;
    KnobLayout mLCFreqLayout;
    KnobLayout mHSFreqLayout;
    KnobLayout mBell1FreqLayout;
    KnobLayout mBell2FreqLayout;
    KnobLayout mLCQLayout;
    KnobLayout mHSCQLayout;
    KnobLayout mBell1CQLayout;
    KnobLayout mBell2CQLayout;

    KnobLayout mHSHighGainLayout;
    KnobLayout mBell1HighGainLayout;
    KnobLayout mBell2HighGainLayout;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mBell1FreqAttachement;
#include "EQKnobComponent.hpp"

};
