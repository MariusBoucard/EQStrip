#include "gui.h"

#include "../dsp/Processor.h"
#include <JuceHeader.h>

RootViewComponent::RootViewComponent(juce::AudioProcessor& processor)
    : AudioProcessorEditor(processor)
    , mProcessor(processor)
    , mTopBar(processor)
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
    mScale = 0.5f;

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
	

}

void RootViewComponent::setSliderAttachement(AudioProcessor& inProcessor)
{
    SkeletonAudioProcessor* processor = dynamic_cast<SkeletonAudioProcessor*>(&inProcessor);
    mGainKnobAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor->getCustomParameterTree(), "gain", mGainKnob);


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
    mGainKnobLayout.inLayout.ratio = mScale;
    computeKnobLayout(mGainKnobLayout);
    mGainKnob.setBounds(mGainKnobLayout.outLayout.x,mGainKnobLayout.outLayout.y,mGainKnobLayout.outLayout.sliderWidth,mGainKnobLayout.outLayout.sliderHeight);

    mTopBarLayout.inLayout.ratio = mScale;
    computeKnobLayout(mTopBarLayout);
    mTopBar.setBounds(mTopBarLayout.outLayout.x,mTopBarLayout.outLayout.y,mTopBarLayout.outLayout.sliderWidth,mTopBarLayout.outLayout.sliderHeight);
}
