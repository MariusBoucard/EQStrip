#pragma once 

class SettingsModalLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawComponentBackground(juce::Graphics& g, juce::Component& component)
    {
        auto bounds = component.getLocalBounds().toFloat();

        g.setColour(juce::Colours::darkgrey.withAlpha(0.8f));
        g.fillRoundedRectangle(bounds, 15.0f);

        g.setColour(juce::Colours::limegreen.withAlpha(0.8f));
        g.drawRoundedRectangle(bounds, 15.0f, 4.0f);
    }
};

class BeautifulLabelLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        auto bounds = label.getLocalBounds().toFloat();

        g.setColour(juce::Colours::black.withAlpha(0.1f));
        g.fillRoundedRectangle(bounds, 10.0f);

        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(20.0f, juce::Font::bold));
        g.drawText(label.getText(), bounds, juce::Justification::centred, true);
    }
};

class ModalButtonDefaultLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // Custom button drawing
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();

        // Background color with transparency
        juce::Colour baseColour = juce::Colours::black.withAlpha(0.7f);
        if (isMouseOverButton)
            baseColour = baseColour.brighter(0.2f); // Lighter on hover
        if (isButtonDown)
            baseColour = baseColour.darker(0.2f); // Darker when pressed

        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, 10.0f); // Rounded corners

        // Border color
        juce::Colour borderColour = isMouseOverButton ? juce::Colours::white : juce::Colours::limegreen;
        g.setColour(borderColour);
        g.drawRoundedRectangle(bounds, 10.0f, 2.0f); // Rounded border
    }

    // Custom button text
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        auto font = juce::Font(18.0f, juce::Font::bold); // Bigger font
        g.setFont(font);

        // Text color
        juce::Colour textColour = isMouseOverButton ? juce::Colours::white : juce::Colours::lightgrey;
        g.setColour(textColour);

        auto bounds = button.getLocalBounds();
        g.drawText(button.getButtonText(), bounds, juce::Justification::centred, true);
    }

    // Custom label drawing
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        g.fillAll(label.findColour(juce::Label::backgroundColourId)); // Background color (transparent by default)
        g.setColour(juce::Colours::white); // Text color
        g.setFont(juce::Font(16.0f, juce::Font::bold)); // Bigger font
        g.drawText(label.getText(), label.getLocalBounds(), juce::Justification::centredLeft, true);
    }
};


class TopBarButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // Custom button background drawing
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();

        // Transparent background by default
        juce::Colour baseColour = juce::Colours::transparentBlack;

        // Dark background on hover
        if (isMouseOverButton)
            baseColour = juce::Colours::black.withAlpha(0.5f);

        g.setColour(baseColour);
        g.fillRect(bounds); // Fill the button background
    }

    // Custom button text drawing
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        auto font = juce::Font(16.0f, juce::Font::bold); // Font size and style
        g.setFont(font);

        // Text color: light grey by default, white on hover
        juce::Colour textColour = isMouseOverButton ? juce::Colours::white : juce::Colours::lightgrey;
        g.setColour(textColour);

        auto bounds = button.getLocalBounds();
        g.drawText(button.getButtonText(), bounds, juce::Justification::centred, true); // Centered text
    }
};