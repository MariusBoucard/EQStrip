struct ParameterSetup;

class ParamListener : public juce::AudioProcessorValueTreeState::Listener
{
public:
    ParamListener(juce::AudioProcessor& inParentProcessor, ParameterSetup& inParameterSetup)
        :
         mProcessor(inParentProcessor)
        , mParameterSetup(inParameterSetup)
    {}

    void parameterChanged(const juce::String& parameterID, float newValue) override
    {

    }
private:
    juce::AudioProcessor& mProcessor;
    ParameterSetup& mParameterSetup;
};
