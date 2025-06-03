#pragma once

#include <JuceHeader.h>
#include <thread>
#include <juce_dsp/juce_dsp.h>

class Mappers
{
private:
	Mappers()
		: mSampleRate(44100.0) {} 
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
	static void setHpf(     juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>>& inSetup,double inFreq, double inQ)
	{
		// TODO SampleRate access
		auto order = 2;// std::max(inQ,1);
		auto newCoeffs = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(inFreq, 44100, order);

		inSetup = newCoeffs;
		//  inSetup.clear();
		//  for (int i = 0; i < newCoeffs.size(); i++)
		//  {
		//  	inSetup.add(newCoeffs);
		// }
	}
	static void setBell(double inBellSetup, double bell)// TODO
	{
		inBellSetup = bell;
	}


	// static void setToneStackMid(recursive_linear_filter::BiquadParams& inMidSetup, const double val) {
	// }

private:
	double mSampleRate;
};
