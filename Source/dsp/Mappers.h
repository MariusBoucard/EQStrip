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
	static void setGain(double& inGainSetup, double gain)
	{
		inGainSetup = gain;
	}
	static void setSampleRate(double sampleRate)
	{
		getMapperInstance().mSampleRate = sampleRate;
		// TODO Set reload all
	}
	static void setHpf(juce::ReferenceCountedObjectPtr<juce::dsp::IIR::Coefficients<float>>& inSetup,double inFreq, double inQ)
	{
		auto sampleRate = getMapperInstance().mSampleRate;
		auto order = std::max(inQ,0.01); // Clip Q to 1.0
		auto newCoeffs = dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, inFreq, order);

		inSetup = newCoeffs;
	}
	static void setBell(juce::ReferenceCountedObjectPtr<juce::dsp::IIR::Coefficients<float>>& inSetup, double inFreq, double inQ, double inGain) {
		auto sampleRate = getMapperInstance().mSampleRate;
		auto order = std::max(inQ, 0.001); // Clip Q to 1.0
		auto gain = std::pow(10.0, inGain / 20.0);
		auto newCoeffs = dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, inFreq, order, gain);
		inSetup = newCoeffs;
	}
	static void setHighShelf(juce::ReferenceCountedObjectPtr<juce::dsp::IIR::Coefficients<float>>& inSetup, double inFreq, double inQ, double inGain)
	{
		auto sampleRate = getMapperInstance().mSampleRate;
		auto order = std::max(inQ, 0.001); // Clip Q to 1.0
		//auto gain = inGain ;
		auto gain = std::pow(10.0, inGain / 20.0);

		auto newCoeffs = dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, inFreq, order,gain);
		inSetup = newCoeffs;
	}

private:
	double mSampleRate;
	// juce::AudioProcessorValueTreeState& mParameters; // TODO pass it parameters
};
