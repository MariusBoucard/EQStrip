void RootViewComponent::defineKnobLayout()
{
    mGainKnobLayout.inLayout.x = 15;
    mGainKnobLayout.inLayout.y = 495;
    mGainKnobLayout.inLayout.frameWidth = 160;
    mGainKnobLayout.inLayout.frameHeight = 192;
    mGainKnobLayout.inLayout.ratio = mScale;
    mGainKnobLayout.inLayout.textboxHeight = 0;
    mGainKnobLayout.inLayout.textboxPadding = 0;
    computeKnobLayout(mGainKnobLayout);

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
	mEQStripLayout.inLayout.frameWidth = 1580;
	mEQStripLayout.inLayout.frameHeight = 380;
	mEQStripLayout.inLayout.ratio = mScale;
	mEQStripLayout.inLayout.textboxHeight = 0;
	mEQStripLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mEQStripLayout);
}


void RootViewComponent::configureNodes(juce::AudioProcessor& inProcessor)
{
    auto bounds = getLocalBounds();

    setSliderAttachement(inProcessor);

	mGainKnob.setBounds(mGainKnobLayout.outLayout.x,
         		mGainKnobLayout.outLayout.y,
         		mGainKnobLayout.outLayout.sliderWidth,
         		mGainKnobLayout.outLayout.sliderHeight);
	mGainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	mGainKnob.setLookAndFeel(new KnobLookAndFeel());

	auto look = dynamic_cast<KnobLookAndFeel*>(&mGainKnob.getLookAndFeel());
	look->setImage(BinaryData::GainKnob_png, BinaryData::GainKnob_pngSize);
	mGainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

	mTopBar.setBounds(mTopBarLayout.outLayout.x,mTopBarLayout.outLayout.y,mTopBarLayout.outLayout.sliderWidth,mTopBarLayout.outLayout.sliderHeight);
	mAnalyzer.setBounds(mAnalyzerLayout.outLayout.x,mAnalyzerLayout.outLayout.y,mAnalyzerLayout.outLayout.sliderWidth,mAnalyzerLayout.outLayout.sliderHeight);
	addAndMakeVisible(mTopBar);
	addAndMakeVisible(mGainKnob);
	addAndMakeVisible(mAnalyzer);

	mEQStrip.setBounds(mEQStripLayout.outLayout.x,mEQStripLayout.outLayout.y,mEQStripLayout.outLayout.sliderWidth,mEQStripLayout.outLayout.sliderHeight);
	addAndMakeVisible(mEQStrip);
	mEQStrip.setScale(mScale);
}
