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
    EQKnobComponent(juce::AudioProcessor &processor, int x, int y)
        : mProcessor(processor)
          , mScale(0.5)
        , mPositionInParent(Point<int>(x, y))
    {
        mBell1Freq.setRange(500.f, 3000.0f);
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

    void setScale(float inScale) {
        mScale = inScale;
    }

    void resized() override
    {
        mBell1FreqLayout.inLayout.ratio = mScale;
        computeKnobLayout(mBell1FreqLayout);
        mBell1Freq.setBounds(mBell1FreqLayout.outLayout.x,mBell1FreqLayout.outLayout.y,mBell1FreqLayout.outLayout.sliderWidth,mBell1FreqLayout.outLayout.sliderHeight);

        mBell1QLayout.inLayout.ratio = mScale;
        computeKnobLayout(mBell1QLayout);
        mBell1Q.setBounds(mBell1QLayout.outLayout.x, mBell1QLayout.outLayout.y, mBell1QLayout.outLayout.sliderWidth, mBell1QLayout.outLayout.sliderHeight);

        mBell1GainLayout.inLayout.ratio = mScale;
        computeKnobLayout(mBell1GainLayout);
        mBell1Gain.setBounds(mBell1GainLayout.outLayout.x, mBell1GainLayout.outLayout.y, mBell1GainLayout.outLayout.sliderWidth, mBell1GainLayout.outLayout.sliderHeight);

        mBell2QLayout.inLayout.ratio = mScale;
        computeKnobLayout(mBell2QLayout);
        mBell2Q.setBounds(mBell2QLayout.outLayout.x, mBell2QLayout.outLayout.y, mBell2QLayout.outLayout.sliderWidth, mBell2QLayout.outLayout.sliderHeight);

        mBell2FreqLayout.inLayout.ratio = mScale;
        computeKnobLayout(mBell2FreqLayout);
        mBell2Freq.setBounds(mBell2FreqLayout.outLayout.x, mBell2FreqLayout.outLayout.y, mBell2FreqLayout.outLayout.sliderWidth, mBell2FreqLayout.outLayout.sliderHeight);

        mBell2GainLayout.inLayout.ratio = mScale;
        computeKnobLayout(mBell2GainLayout);
        mBell2Gain.setBounds(mBell2GainLayout.outLayout.x, mBell2GainLayout.outLayout.y, mBell2GainLayout.outLayout.sliderWidth, mBell2GainLayout.outLayout.sliderHeight);

        mLCFreqLayout.inLayout.ratio = mScale;
        computeKnobLayout(mLCFreqLayout);
        mLCFreq.setBounds(mLCFreqLayout.outLayout.x, mLCFreqLayout.outLayout.y, mLCFreqLayout.outLayout.sliderWidth, mLCFreqLayout.outLayout.sliderHeight);

        mLCQLayout.inLayout.ratio = mScale;
        computeKnobLayout(mLCQLayout);
        mLCQ.setBounds(mLCQLayout.outLayout.x, mLCQLayout.outLayout.y, mLCQLayout.outLayout.sliderWidth, mLCQLayout.outLayout.sliderHeight);

        mHSFreqLayout.inLayout.ratio = mScale;
        computeKnobLayout(mHSFreqLayout);
        mHSFreq.setBounds(mHSFreqLayout.outLayout.x, mHSFreqLayout.outLayout.y, mHSFreqLayout.outLayout.sliderWidth, mHSFreqLayout.outLayout.sliderHeight);

        mHSQLayout.inLayout.ratio = mScale;
        computeKnobLayout(mHSQLayout);
        mHSQ.setBounds(mHSQLayout.outLayout.x, mHSQLayout.outLayout.y, mHSQLayout.outLayout.sliderWidth, mHSQLayout.outLayout.sliderHeight);

        mHSGainLayout.inLayout.ratio = mScale;
        computeKnobLayout(mHSGainLayout);
        mHSGain.setBounds(mHSGainLayout.outLayout.x, mHSGainLayout.outLayout.y, mHSGainLayout.outLayout.sliderWidth, mHSGainLayout.outLayout.sliderHeight);
    }

private:
    KnobLookAndFeel mButtonLookNFeel;

    juce::AudioProcessor& mProcessor;

    juce::Slider mLCFreq;
    juce::Slider mHSFreq;
    juce::Slider mBell1Freq;
    juce::Slider mBell2Freq;

    juce::Slider mLCQ;
    juce::Slider mHSQ;
    juce::Slider mBell1Q;
    juce::Slider mBell2Q;

    juce::Slider mHSGain;
    juce::Slider mBell1Gain;
    juce::Slider mBell2Gain;

    double mScale;
    KnobLayout mLCFreqLayout;
    KnobLayout mHSFreqLayout;
    KnobLayout mBell1FreqLayout;
    KnobLayout mBell2FreqLayout;
    KnobLayout mLCQLayout;
    KnobLayout mHSQLayout;
    KnobLayout mBell1QLayout;
    KnobLayout mBell2QLayout;

    KnobLayout mHSGainLayout;
    KnobLayout mBell1GainLayout;
    KnobLayout mBell2GainLayout;

    Point<int> mPositionInParent;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mLcQAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mHSQAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mBell1QAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mBell1FreqAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mBell1GainAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mBell2QAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mBell2FreqAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mBell2GainAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mHSGainAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mLcFreqAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mHSFreqAttachement;
    #include "EQKnobComponent.hpp"

};
