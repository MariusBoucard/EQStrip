#pragma once
#include "../looknfeel/StripKnob_look_n_feel.h"
#include "../looknfeel/ToogleButtonLookAndFeel.h"
#include "../looknfeel/TopBarLookAndFeel.h"
#include "components/TopBarComponent.h"
#include <JuceHeader.h>
#include "components/AnalyzerComponent.h"
#include "components/EQKnobComponent.h"
#include "components/VerticalMeterComponent.h"

class RootViewComponent : public juce::AudioProcessorEditor, public Timer
{
public:
    // SHOULD BE FZCTORIZED TODO
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

    RootViewComponent(SkeletonAudioProcessor& processor);
    ~RootViewComponent() override;  

    void setSliderAttachement(juce::AudioProcessor& inProcessor);

    void configureKnob(juce::Slider& inKnob, KnobLayout& inLayout, const void* inImage, int inImageSize)
    {
        inKnob.setBounds(inLayout.outLayout.x,inLayout.outLayout.y,inLayout.outLayout.sliderWidth,inLayout.outLayout.sliderHeight);
        inKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        auto newLookAndFeel = std::make_unique<KnobLookAndFeel>();
        newLookAndFeel->setImage(inImage, inImageSize);
        inKnob.setLookAndFeel(newLookAndFeel.get());
        mKnobLookAndFeels.push_back(std::move(newLookAndFeel));
        inKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        inKnob.setPopupDisplayEnabled(true,true, this);
        addAndMakeVisible(inKnob);
    }

    void computeKnobLayout(KnobLayout& inKnobLayout)
    {

        inKnobLayout.outLayout.sliderWidth = inKnobLayout.inLayout.frameWidth *mScale;
        inKnobLayout.outLayout.sliderHeight = inKnobLayout.inLayout.frameHeight * mScale;
        inKnobLayout.outLayout.x = inKnobLayout.inLayout.x*mScale;
        inKnobLayout.outLayout.y = inKnobLayout.inLayout.y *mScale;
	}
    void timerCallback() override;
    void updatePath();
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Image mImage;
    SkeletonAudioProcessor& mProcessor;

    KnobLookAndFeel mKnobLookAndFeel; 
    DropDownLookAndFeel mDropDownLookAndFeel; 
    ToggleButtonLookAndFeel mToggleLookAndFeel;
    float mScale;

    juce::Slider mInputGainKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mInputGainKnobAttachement;
    KnobLayout mInputGainKnobLayout;
    std::vector<std::unique_ptr<KnobLookAndFeel>> mKnobLookAndFeels;

    juce::Slider mOutputGainKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mOutputGainKnobAttachement;
    KnobLayout mOutputGainKnobLayout;

    TopBarComponent mTopBar;
    KnobLayout mTopBarLayout;

    EQKnobComponent mEQStrip;
    KnobLayout mEQStripLayout;

    GUI::VerticalMeterComponent mInputMeterLeft;
    GUI::VerticalMeterComponent mInputMeterRight;
    KnobLayout mInputMeterLeftLayout;
    KnobLayout mInputMeterRightLayout;

    GUI::VerticalMeterComponent mOutputMeterLeft;
    GUI::VerticalMeterComponent mOutputMeterRight;
    KnobLayout mOutputMeterLeftLayout;
    KnobLayout mOutputMeterRightLayout;

    ResponseCurveComponent mAnalyzer;
    KnobLayout mAnalyzerLayout;

#include "gui.hpp"
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RootViewComponent)
};
