#pragma once

#include <JuceHeader.h>
#include <thread>
#include <juce_dsp/juce_dsp.h>

class Mappers
{
private:
	Mappers()
		: mSampleRate(44100.0)
		{}
	Mappers(const Mappers&) = delete; 
	Mappers& operator=(const Mappers&) = delete;

public:
	static Mappers& getMapperInstance()
	{
		static Mappers instance;
		return instance;
	}
	static void init(juce::AudioProcessorValueTreeState& parameters)
	{
		getMapperInstance().mParameters = &parameters;
	}
	static juce::AudioProcessorValueTreeState& getParameters()
	{
		jassert(getMapperInstance().mParameters != nullptr && "Mappers not initialized with parameters!");
		return *getMapperInstance().mParameters;
	}
	static void setSampleRate(double sampleRate)
	{
		getMapperInstance().mSampleRate = sampleRate;
	}
	static void setHpf(juce::ReferenceCountedObjectPtr<juce::dsp::IIR::Coefficients<float>>& inSetup,double inFreq, double inQ)
	{
		auto sampleRate = getMapperInstance().mSampleRate;
		auto order = std::max(inQ,0.02);
		auto newCoeffs = dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, inFreq, order);

		inSetup = newCoeffs;
	}
	static void setBell(juce::ReferenceCountedObjectPtr<juce::dsp::IIR::Coefficients<float>>& inSetup, double inFreq, double inQ, double inGain) {
		auto sampleRate = getMapperInstance().mSampleRate;
		auto order = std::max(inQ, 0.02);
		auto gain = std::pow(10.0, inGain / 20.0);
		auto newCoeffs = dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, inFreq, order, gain);
		inSetup = newCoeffs;
	}
	static void setHighShelf(juce::ReferenceCountedObjectPtr<juce::dsp::IIR::Coefficients<float>>& inSetup, double inFreq, double inQ, double inGain)
	{
		auto sampleRate = getMapperInstance().mSampleRate;
		auto order = std::max(inQ, 0.02);
		auto gain = std::pow(10.0, inGain / 20.0);

		auto newCoeffs = dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, inFreq, order,gain);
		inSetup = newCoeffs;
	}

private:
	double mSampleRate;
	juce::AudioProcessorValueTreeState* mParameters; // Store as pointer to avoid object slicing
	// juce::AudioProcessorValueTreeState& mParameters; // TODO pass it parameters
};
