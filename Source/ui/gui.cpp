#include "gui.h"

#include "../dsp/Processor.h"
#include <JuceHeader.h>
int RootViewComponent::ROOT_WIDTH = 900;
int RootViewComponent::ROOT_HEIGHT = 550;

RootViewComponent::RootViewComponent(juce::AudioProcessor& processor)
    : AudioProcessorEditor(processor)
    , mProcessor(processor)
{
    auto imageData = BinaryData::plate_png;
    auto imageDataSize = BinaryData::plate_pngSize;

    mImage = juce::ImageFileFormat::loadFrom(imageData, imageDataSize);

    if (mImage.isNull())
        DBG("Failed to load image from resources");

    setSize(mImage.getBounds().getWidth(), mImage.getBounds().getHeight());
    defineKnobLayout();
    configureNodes(processor);
}

RootViewComponent::~RootViewComponent()
{
	

}

void RootViewComponent::setSliderAttachement(AudioProcessor& inProcessoe)
{
    SkeletonAudioProcessor* ampAudioProcessor = dynamic_cast<SkeletonAudioProcessor*>(&inProcessoe);

}

void RootViewComponent::updatePath()
{
}

void RootViewComponent::paint(juce::Graphics& g)
{
    // g.fillAll(juce::Colours::black);
    //
    // if (!mImage.isNull())
    // {
    //     auto bounds = getLocalBounds().toFloat();
    //     auto imageBounds = mImage.getBounds().toFloat();
    //     auto scale = juce::jmin(bounds.getWidth() / imageBounds.getWidth(),
    //         bounds.getHeight() / imageBounds.getHeight());
    //     auto scaledImageBounds = imageBounds.withSize(imageBounds.getWidth() * scale,
    //         imageBounds.getHeight() * scale);
    //     g.drawImage(mImage, scaledImageBounds);
    // }
    // else
    // {
    //     g.setColour(juce::Colours::white);
    //     g.drawText("Image not found", getLocalBounds(), juce::Justification::centred);
    // }
}

void RootViewComponent::resized()
{
    auto bounds = getLocalBounds();
}
