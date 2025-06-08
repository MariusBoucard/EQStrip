void EQKnobComponent::defineKnobLayout()
{

	mLCQLayout.inLayout.x = 321 - mPositionInParent.x;
	mLCQLayout.inLayout.y = 900 - mPositionInParent.y;
	mLCQLayout.inLayout.frameWidth = 193;
	mLCQLayout.inLayout.frameHeight = 155;
	mLCQLayout.inLayout.ratio = mScale;
	mLCQLayout.inLayout.textboxHeight = 0;
	mLCQLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mLCQLayout);

	mLCFreqLayout.inLayout.x = 138 - mPositionInParent.x;
	mLCFreqLayout.inLayout.y = 802 - mPositionInParent.y;
	mLCFreqLayout.inLayout.frameWidth = 193;
	mLCFreqLayout.inLayout.frameHeight = 159;
	mLCFreqLayout.inLayout.ratio = mScale;
	mLCFreqLayout.inLayout.textboxHeight = 0;
	mLCFreqLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mLCFreqLayout);

	mBell1QLayout.inLayout.x = 770 - mPositionInParent.x;
    mBell1QLayout.inLayout.y = 800 - mPositionInParent.y;
 	mBell1QLayout.inLayout.frameWidth = 188;
	mBell1QLayout.inLayout.frameHeight = 159;
	mBell1QLayout.inLayout.ratio = mScale;
	mBell1QLayout.inLayout.textboxHeight = 0;
	mBell1QLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mBell1QLayout);

    mBell1FreqLayout.inLayout.x = 457 - mPositionInParent.x;
    mBell1FreqLayout.inLayout.y = 741 - mPositionInParent.y;
    mBell1FreqLayout.inLayout.frameWidth = 188;
    mBell1FreqLayout.inLayout.frameHeight = 159;
    mBell1FreqLayout.inLayout.ratio = mScale;
    mBell1FreqLayout.inLayout.textboxHeight = 0;
    mBell1FreqLayout.inLayout.textboxPadding = 0;
    computeKnobLayout(mBell1FreqLayout);

	mBell1GainLayout.inLayout.x = 658 - mPositionInParent.x;
	mBell1GainLayout.inLayout.y = 898 - mPositionInParent.y;
	mBell1GainLayout.inLayout.frameWidth = 182;
	mBell1GainLayout.inLayout.frameHeight = 157;
	mBell1GainLayout.inLayout.ratio = mScale;
	mBell1GainLayout.inLayout.textboxHeight = 0;
	mBell1GainLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mBell1GainLayout);

	mBell2FreqLayout.inLayout.x = 933 - mPositionInParent.x;
	mBell2FreqLayout.inLayout.y = 805 - mPositionInParent.y;
	mBell2FreqLayout.inLayout.frameWidth = 178;
	mBell2FreqLayout.inLayout.frameHeight = 167;
	mBell2FreqLayout.inLayout.ratio = mScale;
	mBell2FreqLayout.inLayout.textboxHeight = 0;
	mBell2FreqLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mBell2FreqLayout);

	mBell2GainLayout.inLayout.x = 1111 - mPositionInParent.x;
	mBell2GainLayout.inLayout.y = 895 - mPositionInParent.y;
	mBell2GainLayout.inLayout.frameWidth = 175;
	mBell2GainLayout.inLayout.frameHeight = 161;
	mBell2GainLayout.inLayout.ratio = mScale;
	mBell2GainLayout.inLayout.textboxHeight = 0;
	mBell2GainLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mBell2GainLayout);

	mBell2QLayout.inLayout.x = 1260 - mPositionInParent.x;
	mBell2QLayout.inLayout.y = 713 - mPositionInParent.y;
	mBell2QLayout.inLayout.frameWidth = 175;
	mBell2QLayout.inLayout.frameHeight = 169;
	mBell2QLayout.inLayout.ratio = mScale;
	mBell2QLayout.inLayout.textboxHeight = 0;
	mBell2QLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mBell2QLayout);

	mHSGainLayout.inLayout.x = 1434 - mPositionInParent.x;
	mHSGainLayout.inLayout.y = 706 - mPositionInParent.y;
	mHSGainLayout.inLayout.frameWidth = 158;
	mHSGainLayout.inLayout.frameHeight = 165;
	mHSGainLayout.inLayout.ratio = mScale;
	mHSGainLayout.inLayout.textboxHeight = 0;
	mHSGainLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mHSGainLayout);

	mHSFreqLayout.inLayout.x = 1430 - mPositionInParent.x;
	mHSFreqLayout.inLayout.y = 889 - mPositionInParent.y;
	mHSFreqLayout.inLayout.frameWidth = 167;
	mHSFreqLayout.inLayout.frameHeight = 166;
	mHSFreqLayout.inLayout.ratio = mScale;
	mHSFreqLayout.inLayout.textboxHeight = 0;
	mHSFreqLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mHSFreqLayout);

	mHSQLayout.inLayout.x = 1627 - mPositionInParent.x;
	mHSQLayout.inLayout.y = 890 - mPositionInParent.y;
	mHSQLayout.inLayout.frameWidth = 176;
	mHSQLayout.inLayout.frameHeight = 162;
	mHSQLayout.inLayout.ratio = mScale;
	mHSQLayout.inLayout.textboxHeight = 0;
	mHSQLayout.inLayout.textboxPadding = 0;
	computeKnobLayout(mHSQLayout);





}


void EQKnobComponent::setSliderAttachement(AudioProcessor& inProcessor)
{
	SkeletonAudioProcessor* processor = dynamic_cast<SkeletonAudioProcessor*>(&inProcessor);
	mBell1FreqAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "Bell1_Freq", mBell1Freq);
	mLcQAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "HPF_Q", mLCQ);
	mLcFreqAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "HPF_Freq", mLCFreq);
	mBell1QAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "Bell1_Q", mBell1Q);
	 mBell1GainAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "Bell1_Gain", mBell1Gain);
	mBell2QAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "Bell2_Q", mBell2Q);
	mBell2FreqAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "Bell2_Freq", mBell2Freq);
	mBell2GainAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "Bell2_Gain", mBell2Gain);
	mHSGainAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "HS_Gain", mHSGain);
	mHSFreqAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "HS_Freq", mHSFreq);
	mHSQAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "HS_Q", mHSQ);

}

void EQKnobComponent::configureNodes(juce::AudioProcessor& inProcessor)
{
    auto bounds = getLocalBounds();

    setSliderAttachement(inProcessor);
	configureKnob(mLCQ, mLCQLayout, BinaryData::LcQ_png, BinaryData::LcQ_pngSize);
	configureKnob(mLCFreq, mLCFreqLayout, BinaryData::LcFreq_png, BinaryData::LcFreq_pngSize);

	configureKnob(mBell1Gain, mBell1GainLayout, BinaryData::Bell1Gain_png, BinaryData::Bell1Gain_pngSize);
	configureKnob(mBell2Gain, mBell2GainLayout, BinaryData::Bell2Gain_png, BinaryData::Bell2Gain_pngSize);
	configureKnob(mBell1Q, mBell1QLayout, BinaryData::Bell2Q_png, BinaryData::Bell2Q_pngSize);
	configureKnob(mBell2Q, mBell2QLayout, BinaryData::Bell2Q_png, BinaryData::Bell2Q_pngSize);
	configureKnob(mBell2Freq, mBell2FreqLayout, BinaryData::Bell2Freq_png, BinaryData::Bell2Freq_pngSize);
	configureKnob(mBell1Freq, mBell1FreqLayout, BinaryData::Bell1Freq_png, BinaryData::Bell1Freq_pngSize);

	configureKnob(mHSGain, mHSGainLayout, BinaryData::HSGain_png, BinaryData::HSGain_pngSize);
	configureKnob(mHSFreq, mHSFreqLayout, BinaryData::HSFreq_png, BinaryData::HSFreq_pngSize);
	configureKnob(mHSQ, mHSQLayout, BinaryData::HSq_png, BinaryData::HSq_pngSize);


}

void EQKnobComponent::configureKnob(juce::Slider& inKnob, KnobLayout& inLayout, const void* inImage, int inImageSize)
{
	auto bounds = getLocalBounds();

	inKnob.setBounds(inLayout.outLayout.x,inLayout.outLayout.y,inLayout.outLayout.sliderWidth,inLayout.outLayout.sliderHeight);
	inKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	inKnob.setLookAndFeel(new KnobLookAndFeel());
	auto look = dynamic_cast<KnobLookAndFeel*>(&inKnob.getLookAndFeel());
	look->setImage(inImage, inImageSize);
	inKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	inKnob.setPopupDisplayEnabled(true,true, this);
	addAndMakeVisible(inKnob);
}
