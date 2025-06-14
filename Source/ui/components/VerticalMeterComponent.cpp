#include "VerticalMeterComponent.h"

namespace GUI
{
    void VerticalMeterComponent::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colours::white.withBrightness(0.4f));
        g.fillRoundedRectangle(bounds, 5.0f);
        g.setColour(juce::Colours::white);

        // Map the level from minLevel to max level
        const auto top = jmap(mLevel, mMinLevel, mMaxLevel, 0.0f, static_cast<float>(getHeight()));
        g.fillRoundedRectangle(bounds.removeFromTop(top),5.f);

    }

    void VerticalMeterComponent::setLevel(float inLevel)
    {
        mLevel = inLevel;
    }
}