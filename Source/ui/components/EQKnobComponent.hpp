void EQKnobComponent::defineKnobLayout()
{
    mBell1FreqLayout.inLayout.x = 15;
    mBell1FreqLayout.inLayout.y = 15;
    mBell1FreqLayout.inLayout.frameWidth = 160;
    mBell1FreqLayout.inLayout.frameHeight = 192;
    mBell1FreqLayout.inLayout.ratio = mScale;
    mBell1FreqLayout.inLayout.textboxHeight = 0;
    mBell1FreqLayout.inLayout.textboxPadding = 0;
    computeKnobLayout(mBell1FreqLayout);
}

void EQKnobComponent::setSliderAttachement(AudioProcessor& inProcessor)
{
	SkeletonAudioProcessor* processor = dynamic_cast<SkeletonAudioProcessor*>(&inProcessor);
	mBell1FreqAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "Bell1_Freq", mBell1Freq);


}
void EQKnobComponent::configureNodes(juce::AudioProcessor& inProcessor)
{
    auto bounds = getLocalBounds();

    setSliderAttachement(inProcessor);

	mBell1Freq.setBounds(mBell1FreqLayout.outLayout.x,
         		mBell1FreqLayout.outLayout.y,
         		mBell1FreqLayout.outLayout.sliderWidth,
         		mBell1FreqLayout.outLayout.sliderHeight);
	mBell1Freq.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	mBell1Freq.setLookAndFeel(new KnobLookAndFeel());
	auto look = dynamic_cast<KnobLookAndFeel*>(&mBell1Freq.getLookAndFeel());
	look->setImage(BinaryData::GainKnob_png, BinaryData::GainKnob_pngSize);
	mBell1Freq.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

	addAndMakeVisible(mBell1Freq);
}
