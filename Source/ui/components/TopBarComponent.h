#pragma once
#include <JuceHeader.h>
#include "../../looknfeel/TopBarLookAndFeel.h"

class TopBarComponent : public juce::Component
{
public:
    TopBarComponent(juce::AudioProcessor& processor)
        : audioProcessor(processor)
    {
        mAboutButton.setLookAndFeel(&mButtonLookNFeel);
        addAndMakeVisible(mAboutButton);
        mAboutButton.setButtonText("About");
        mAboutButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        mAboutButton.onClick = [this]() { showAboutModal(); };

    }

    ~TopBarComponent() override
	{
		mAboutButton.setLookAndFeel(nullptr);

		setLookAndFeel(nullptr); 
	}

    void paint(juce::Graphics& g) override;

    void resized() override
    {
        auto buttonSize = getHeight();
        mAboutButton.setBounds(0, 0, buttonSize*2, buttonSize);
       
    }

private:
    TopBarButtonLookAndFeel mButtonLookNFeel;

    juce::AudioProcessor& audioProcessor;
    juce::TextButton mAboutButton{ "About" };
    juce::Label mAmpNameLabel{ "RADIATOR" };
    BeautifulLabelLookAndFeel mLabelLookAndFeel; 

    void showAboutModal()
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon,
            "About",
            "Ballzzy's DSP | NAM - version 0.1",
            "Lets get back to it");
    }


};