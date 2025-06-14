#include "gui.h"

#include "../dsp/Processor.h"
#include <JuceHeader.h>

RootViewComponent::RootViewComponent(SkeletonAudioProcessor& processor)
    : AudioProcessorEditor(processor)
    , mProcessor(processor)
    , mTopBar(processor)
    , mEQStrip(processor,180,670)
    , mScale(0.5f)
    , mAnalyzer(mProcessor,dynamic_cast<SkeletonAudioProcessor&>(processor).getCustomParameterTree(),dynamic_cast<SkeletonAudioProcessor&>(processor).getParameterSetup())
{
    auto imageData = BinaryData::plate_png;
    auto imageDataSize = BinaryData::plate_pngSize;
    setResizable(true, true);
    setResizeLimits(400, 300, 1920, 1080);
    mImage = juce::ImageFileFormat::loadFrom(imageData, imageDataSize);

    float aspectRatio = (float) mImage.getWidth() / (float) mImage.getHeight();

    int minHeight = 300;
    int minWidth  = static_cast<int>(minHeight * aspectRatio);

    int maxHeight = 900;
    int maxWidth  = static_cast<int>(maxHeight * aspectRatio);

    setResizable(true, true);
    setResizeLimits(minWidth, minHeight, maxWidth, maxHeight);
    getConstrainer()->setFixedAspectRatio(aspectRatio);
    if (mImage.isNull())
        DBG("Failed to load image from resources");

    setSize(mImage.getBounds().getWidth()*mScale, mImage.getBounds().getHeight()*mScale);
    defineKnobLayout();
    configureNodes(processor);

}

RootViewComponent::~RootViewComponent()
{
    mInputGainKnobAttachement.reset();
    mOutputGainKnobAttachement.reset();

    for (auto* child : getChildren())
    {
        if (auto* slider = dynamic_cast<juce::Slider*>(child))
            slider->setLookAndFeel(nullptr);
    }

    mKnobLookAndFeels.clear();

    removeAllChildren();
}

void RootViewComponent::setSliderAttachement(AudioProcessor& inProcessor)
{
    SkeletonAudioProcessor* processor = dynamic_cast<SkeletonAudioProcessor*>(&inProcessor);
    mInputGainKnobAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "inputGain", mInputGainKnob);
    mOutputGainKnobAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "outputGain", mOutputGainKnob);

}

void RootViewComponent::updatePath()
{
}

void RootViewComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    if (!mImage.isNull())
    {
        auto bounds = getLocalBounds().toFloat();
        auto imageBounds = mImage.getBounds().toFloat();

        // Compute the scale to fit the image in the current component size
        mScale = juce::jmin(bounds.getWidth() / imageBounds.getWidth(),
                                bounds.getHeight() / imageBounds.getHeight());

        auto scaledImageBounds = imageBounds.withSize(
            imageBounds.getWidth() * mScale,
            imageBounds.getHeight() * mScale);

        g.drawImage(mImage, scaledImageBounds);
    }
    else
    {
        g.setColour(juce::Colours::white);
        g.drawText("Image not found", getLocalBounds(), juce::Justification::centred);
    }
}

void RootViewComponent::resized()
{
    auto bounds = getLocalBounds().toFloat();
    auto imageBounds = mImage.getBounds().toFloat();
    mScale = juce::jmin(bounds.getWidth() / imageBounds.getWidth(),
                            bounds.getHeight() / imageBounds.getHeight());
    mInputGainKnobLayout.inLayout.ratio = mScale;
    computeKnobLayout(mInputGainKnobLayout);
    mInputGainKnob.setBounds(mInputGainKnobLayout.outLayout.x,mInputGainKnobLayout.outLayout.y,mInputGainKnobLayout.outLayout.sliderWidth,mInputGainKnobLayout.outLayout.sliderHeight);

    mOutputGainKnobLayout.inLayout.ratio = mScale;
    computeKnobLayout(mOutputGainKnobLayout);
    mOutputGainKnob.setBounds(mOutputGainKnobLayout.outLayout.x,mOutputGainKnobLayout.outLayout.y,mOutputGainKnobLayout.outLayout.sliderWidth,mOutputGainKnobLayout.outLayout.sliderHeight);
    mTopBarLayout.inLayout.ratio = mScale;
    computeKnobLayout(mTopBarLayout);
    mTopBar.setBounds(mTopBarLayout.outLayout.x,mTopBarLayout.outLayout.y,mTopBarLayout.outLayout.sliderWidth,mTopBarLayout.outLayout.sliderHeight);

    mAnalyzerLayout.inLayout.ratio = mScale;
    computeKnobLayout(mAnalyzerLayout);
    mAnalyzer.setBounds(mAnalyzerLayout.outLayout.x,mAnalyzerLayout.outLayout.y,mAnalyzerLayout.outLayout.sliderWidth,mAnalyzerLayout.outLayout.sliderHeight);

    mEQStripLayout.inLayout.ratio = mScale;
    computeKnobLayout(mEQStripLayout);
    mEQStrip.setBounds(mEQStripLayout.outLayout.x,mEQStripLayout.outLayout.y,mEQStripLayout.outLayout.sliderWidth,mEQStripLayout.outLayout.sliderHeight);
    mEQStrip.setScale(mScale);
    mEQStrip.resized();
}
