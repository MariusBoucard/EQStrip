#include "TopBarComponent.h"
#include "../../dsp/processor.h"
#include "../gui.h"

void TopBarComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
    juce::Rectangle<float> bounds(0, 0, getWidth(), getHeight());
    g.fillRoundedRectangle(bounds, 5.0f);

    g.setColour(juce::Colours::limegreen);
    g.fillRect(bounds.removeFromBottom(1.0f));
}

