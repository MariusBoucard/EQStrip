void RootViewComponent::defineKnobLayout()
{
    mInputGainKnobLayout.inLayout.x = 15;
    mInputGainKnobLayout.inLayout.y = 495;
    mInputGainKnobLayout.inLayout.frameWidth = 160;
    mInputGainKnobLayout.inLayout.frameHeight = 192;
    mInputGainKnobLayout.inLayout.ratio = mScale;
    mInputGainKnobLayout.inLayout.textboxHeight = 0;
    mInputGainKnobLayout.inLayout.textboxPadding = 0;
    computeKnobLayout(mInputGainKnobLayout);

	mOutputGainKnobLayout.inLayout.x = 1725;
	mOutputGainKnobLayout.inLayout.y = 507;
	mOutputGainKnobLayout.inLayout.frameWidth = 186;
	mOutputGainKnobLayout.inLayout.frameHeight = 164;
	mOutputGainKnobLayout.inLayout.ratio = mScale;
	mOutputGainKnobLayout.inLayout.textboxHeight = 0;
	mOutputGainKnobLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mOutputGainKnobLayout);

	mTopBarLayout.inLayout.x = 0;
	mTopBarLayout.inLayout.y = 0;
	mTopBarLayout.inLayout.frameWidth = 1920; // Relative to parent
	mTopBarLayout.inLayout.frameHeight = 60;
	mTopBarLayout.inLayout.ratio = mScale;
	mTopBarLayout.inLayout.textboxHeight = 0;
	mTopBarLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mTopBarLayout);

	mAnalyzerLayout.inLayout.x = 180;
	mAnalyzerLayout.inLayout.y = 170;
	mAnalyzerLayout.inLayout.frameWidth = 1560;
	mAnalyzerLayout.inLayout.frameHeight = 330;
	mAnalyzerLayout.inLayout.ratio = mScale;
	mAnalyzerLayout.inLayout.textboxHeight = 0;
	mAnalyzerLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mAnalyzerLayout);

	mEQStripLayout.inLayout.x = 180;
	mEQStripLayout.inLayout.y = 670;
	mEQStripLayout.inLayout.frameWidth = 1650;
	mEQStripLayout.inLayout.frameHeight = 380;
	mEQStripLayout.inLayout.ratio = mScale;
	mEQStripLayout.inLayout.textboxHeight = 0;
	mEQStripLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mEQStripLayout);

	mInputMeterLeftLayout.inLayout.x = 75;
	mInputMeterLeftLayout.inLayout.y = 250;
	mInputMeterLeftLayout.inLayout.frameWidth = 20;
	mInputMeterLeftLayout.inLayout.frameHeight = 230;
	mInputMeterLeftLayout.inLayout.ratio = mScale;
	mInputMeterLeftLayout.inLayout.textboxHeight = 0;
	mInputMeterLeftLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mInputMeterLeftLayout);

	mInputMeterRightLayout.inLayout.x = 100;
	mInputMeterRightLayout.inLayout.y = 250;
	mInputMeterRightLayout.inLayout.frameWidth = 20;
	mInputMeterRightLayout.inLayout.frameHeight = 230;
	mInputMeterRightLayout.inLayout.ratio = mScale;
	mInputMeterRightLayout.inLayout.textboxHeight = 0;
	mInputMeterRightLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mInputMeterRightLayout);

	mOutputMeterLeftLayout.inLayout.x = 1820;
	mOutputMeterLeftLayout.inLayout.y = 250;
	mOutputMeterLeftLayout.inLayout.frameWidth = 20;
	mOutputMeterLeftLayout.inLayout.frameHeight = 230;
	mOutputMeterLeftLayout.inLayout.ratio = mScale;
	mOutputMeterLeftLayout.inLayout.textboxHeight = 0;
	mOutputMeterLeftLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mOutputMeterLeftLayout);

	mOutputMeterRightLayout.inLayout.x = 1845;
	mOutputMeterRightLayout.inLayout.y = 250;
	mOutputMeterRightLayout.inLayout.frameWidth = 20;
	mOutputMeterRightLayout.inLayout.frameHeight = 230;
	mOutputMeterRightLayout.inLayout.ratio = mScale;
	mOutputMeterRightLayout.inLayout.textboxHeight = 0;
	mOutputMeterRightLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mOutputMeterRightLayout);

}


void RootViewComponent::configureNodes(juce::AudioProcessor& inProcessor)
{
    auto bounds = getLocalBounds();

    setSliderAttachement(inProcessor);

	configureKnob(mInputGainKnob, mInputGainKnobLayout,BinaryData::GainKnob_png, BinaryData::GainKnob_pngSize);
	configureKnob(mOutputGainKnob, mOutputGainKnobLayout,BinaryData::outputGain_png, BinaryData::outputGain_pngSize);

	mTopBar.setBounds(mTopBarLayout.outLayout.x,mTopBarLayout.outLayout.y,mTopBarLayout.outLayout.sliderWidth,mTopBarLayout.outLayout.sliderHeight);
	mAnalyzer.setBounds(mAnalyzerLayout.outLayout.x,mAnalyzerLayout.outLayout.y,mAnalyzerLayout.outLayout.sliderWidth,mAnalyzerLayout.outLayout.sliderHeight);
	addAndMakeVisible(mInputMeterLeft);
	addAndMakeVisible(mInputMeterRight);
	addAndMakeVisible(mOutputMeterLeft);
	addAndMakeVisible(mOutputMeterRight);
	addAndMakeVisible(mTopBar);
	addAndMakeVisible(mInputGainKnob);
	addAndMakeVisible(mOutputGainKnob);
	addAndMakeVisible(mAnalyzer);

	mEQStrip.setBounds(mEQStripLayout.outLayout.x,mEQStripLayout.outLayout.y,mEQStripLayout.outLayout.sliderWidth,mEQStripLayout.outLayout.sliderHeight);
	addAndMakeVisible(mEQStrip);

}
