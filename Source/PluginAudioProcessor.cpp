#include "PluginAudioProcessor.h"
#include "dsp/ParameterSetup.h"
#include <filesystem>
#include <iostream>


PluginAudioProcessor::PluginAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", AudioChannelSet::stereo())
          .withOutput("Output", AudioChannelSet::stereo()))
      , mParameters(*this, nullptr, "PARAMETERS", createParameterLayout())
      , mParameterSetup(mParameters)
      , mSkeletonProcessor(mParameters, mParameterSetup)
{
    Mappers::init(mParameters);
}

PluginAudioProcessor::~PluginAudioProcessor()
{
}

void PluginAudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &a)
{
    mSkeletonProcessor.processBlock(buffer, a);
}
