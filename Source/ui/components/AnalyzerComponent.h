
#pragma once

#include <JuceHeader.h>
#include "../../dsp/ParameterSetup.h"
#include "../../dsp/OutputData.h"
class SkeletonAudioProcessor;

// #include "PluginProcessor.h"
enum FFTOrder
{
    order32 = 0,
    order64 = 1,
    order2048 = 11,
    order4096 = 12,
    order8192 = 13
};

using Filter = juce::dsp::IIR::Filter<float>;
using MonoChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

enum ChainPosition
{
    LowCut,
    Bell1,
    Bell2,
    HighShelf
};

template<typename BlockType>
struct FFTDataGenerator
{
    /**
     produces the FFT data from an audio buffer.
     */
    void produceFFTDataForRendering(const juce::AudioBuffer<float>& audioData, const float negativeInfinity)
    {
        const auto fftSize = getFFTSize();

       fftData.assign(512, 0); // This line is good, assuming fftData is sized correctly
       auto* readIndex = audioData.getReadPointer(0);
       std::copy(readIndex, readIndex + fftSize, fftData.begin()); // This copies to fftData

        // first apply a windowing function to our data
        window->multiplyWithWindowingTable (fftData.data(), fftSize);       // [1]

        // then render our FFT data..
        forwardFFT->performFrequencyOnlyForwardTransform (fftData.data());  // [2]

        int numBins = (int)fftSize / 2;

        // //normalize the fft values.
        for( int i = 0; i < numBins; ++i )
        {
            auto v = fftData[i];
            fftData[i] /= (float) numBins;
            if( !std::isinf(v) && !std::isnan(v) )
            {
                v /= float(numBins);
            }
            else
            {
                v = 0.f;
            }
            fftData[i] = v;
        }

        //convert them to decibels
        for( int i = 0; i < numBins; ++i )
        {
            fftData[i] = juce::Decibels::gainToDecibels(fftData[i], negativeInfinity);
        }

        mCurrentFFT = fftData;
      //fftDataFifo.push(fftData); // Push the std::vector<float> to the FIFO
    }

    void changeOrder(FFTOrder newOrder)
    {
        //when you change order, recreate the window, forwardFFT, fifo, fftData
        //also reset the fifoIndex
        //things that need recreating should be created on the heap via std::make_unique<>

        order = newOrder;
        auto fftSize = getFFTSize();

        forwardFFT = std::make_unique<juce::dsp::FFT>(order);
        window = std::make_unique<juce::dsp::WindowingFunction<float>>(fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris);

        fftData.clear();
        fftData.resize(fftSize * 2, 0); // fftData is now std::vector<float>, so resize works

       // fftDataFifo.prepare(fftData.size()); // Prepare the FIFO to hold vectors of this size
    }
    //==============================================================================
    int getFFTSize() const { return 1 << 3; }
     int getNumAvailableFFTDataBlocks() const { return fftDataFifo.getNumAvailableForReading(); }
    std::vector<float> getFFTData() const { return fftData; }
    //==============================================================================
    bool getFFTData(BlockType& dataToFill) { return false; // fftDataFifo.pull(dataToFill);
        }
private:
    FFTOrder order;
    std::vector<float> fftData; // Changed from BlockType to std::vector<float>
    std::unique_ptr<juce::dsp::FFT> forwardFFT;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;

    // Assuming AudioBufferFifo is templated to store std::vector<float>
    // or that it can handle a BlockType that is compatible with std::vector<float>
    std::vector<float> mCurrentFFT;
    AudioBufferFifo fftDataFifo; // The FIFO now stores std::vector<float>
};

template<typename PathType>
struct AnalyzerPathGenerator
{
    void generatePath(const std::vector<float>& renderData,
                      juce::Rectangle<float> fftBounds,
                      int fftSize,
                      float binWidth,
                      float negativeInfinity)
    {
        auto top = fftBounds.getY();
        auto bottom = fftBounds.getHeight();
        auto width = fftBounds.getWidth();

        int numBins = (int)fftSize / 2;

        PathType p;
        p.preallocateSpace(3 * (int)fftBounds.getWidth());

        auto map = [bottom, top, negativeInfinity](float v)
        {
            return juce::jmap(v,
                              negativeInfinity, 0.f,
                              float(bottom+10),   top);
        };

        auto y = map(renderData[0]);

        if( std::isnan(y) || std::isinf(y) )
            y = bottom;
        
        p.startNewSubPath(0, y);

        const int pathResolution = 2;

        for( int binNum = 1; binNum < numBins; binNum += pathResolution )
        {
            y = map(renderData[binNum]);

            if( !std::isnan(y) && !std::isinf(y) )
            {
                auto binFreq = binNum * binWidth;
                auto normalizedBinX = juce::mapFromLog10(binFreq, 20.f, 20000.f);
                int binX = std::floor(normalizedBinX * width);
                p.lineTo(binX, y);
            }
        }
        mPath = p;

       // pathFifo.push(p);
    }
    

    int getNumPathsAvailable() const
    {
        return pathFifo.getNumAvailableForReading();
    }


    PathType getPath() const
    {
        return mPath;
    }

    bool getPath(PathType& path)
    {
        return false;//pathFifo.pull(path);
    }
private:
    AudioBufferFifo pathFifo;
    PathType mPath;
};


struct PathProducer {
    PathProducer(SingleChannelSampleFifo &scsf) : leftChannelFifo(&scsf) {
        leftChannelFFTDataGenerator.changeOrder(FFTOrder::order32); // Will change to fifo to make biger ff
        monoBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize());
    }

    void process(juce::Rectangle<float> fftBounds, double sampleRate);

    juce::Path getPath() const { return pathProducer.getPath(); }

private:
    SingleChannelSampleFifo *leftChannelFifo;
    juce::AudioBuffer<float> monoBuffer;
    FFTDataGenerator<std::vector<float> > leftChannelFFTDataGenerator;
    AnalyzerPathGenerator<juce::Path> pathProducer;
    juce::Path leftChannelFFTPath;
};

struct ResponseCurveComponent : juce::Component,
                                juce::AudioProcessorParameter::Listener,
                                juce::Timer
{
  ResponseCurveComponent(SkeletonAudioProcessor &processor, juce::AudioProcessorValueTreeState& inParameters, ParameterSetup& inParameterSetup);
  ~ResponseCurveComponent();
  void parameterValueChanged(int parameterIndex, float newValue) override;

  void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}
  void timerCallback() override;
  void paint(juce::Graphics &g) override;
  void updateChain();
  void resized() override;

  void toggleAnalysisEnablement(bool enabled)
  {
    shouldShowFFTAnalisis = enabled;
  }
private:
  juce::Atomic<bool> parametersChanged{false};
  MonoChain monoChain;
  ParameterSetup& mParameterSetup;
  SkeletonAudioProcessor &mProcessor;
  juce::AudioProcessorValueTreeState& mParameters;

  juce::Image background;

  juce::Rectangle<int> getRenderArea();
  juce::Rectangle<int> getAnalysisArea();

  PathProducer mLeftPathProducer;
  bool shouldShowFFTAnalisis = true ;
 };
