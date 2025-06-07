#pragma once
#include "../looknfeel/StripKnob_look_n_feel.h"
#include "../looknfeel/ToogleButtonLookAndFeel.h"
#include "../looknfeel/TopBarLookAndFeel.h"
#include "components/TopBarComponent.h"
#include <JuceHeader.h>
#include "components/AnalyzerComponent.h"
class RootViewComponent : public juce::AudioProcessorEditor
{
public:
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

    RootViewComponent(juce::AudioProcessor& processor);
    ~RootViewComponent() override;  

    void setSliderAttachement(juce::AudioProcessor& inProcessor);
    void computeKnobLayout(KnobLayout& inKnobLayout)
    {

        inKnobLayout.outLayout.sliderWidth = inKnobLayout.inLayout.frameWidth *mScale;
        inKnobLayout.outLayout.sliderHeight = inKnobLayout.inLayout.frameHeight * mScale;
        inKnobLayout.outLayout.x = inKnobLayout.inLayout.x*mScale;
        inKnobLayout.outLayout.y = inKnobLayout.inLayout.y *mScale;
	}

    void updatePath();
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Image mImage;
    juce::AudioProcessor& mProcessor;

    KnobLookAndFeel mKnobLookAndFeel; 
    DropDownLookAndFeel mDropDownLookAndFeel; 
    ToggleButtonLookAndFeel mToggleLookAndFeel;
    float mScale;

    juce::Slider mGainKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mGainKnobAttachement;
    KnobLayout mGainKnobLayout;

    TopBarComponent mTopBar;
    KnobLayout mTopBarLayout;

    ResponseCurveComponent mAnalyzer;
    KnobLayout mAnalyzerLayout;

#include "gui.hpp"
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RootViewComponent)
};