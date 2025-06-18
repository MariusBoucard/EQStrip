
#pragma once

#include <JuceHeader.h>
#include "../../dsp/Mappers.h"// TODO Fix that shit
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
    void produceFFTDataForRendering(const juce::AudioBuffer<float>& inBuffer, const float negativeInfinity)
    {
        const auto fftSize = getFFTSize();

       mFftData.assign(fftSize, 0);// TODO Check
       auto* readIndex = inBuffer.getReadPointer(0); // left channel TODO
       std::copy(readIndex, readIndex + fftSize, mFftData.begin()); // This copies to fftData

        // first apply a windowing function to our data
        mWindow->multiplyWithWindowingTable (mFftData.data(), fftSize);       // [1]

        // then render our FFT data..
        forwardFFT->performFrequencyOnlyForwardTransform (mFftData.data());  // [2]

        int numBins = (int)fftSize / 2;

        // // //normalize the fft values.
        // for( int i = 0; i < numBins; ++i )
        // {
        //     auto fftValueBin = mFftData[i];
        //     if( !std::isinf(fftValueBin) && !std::isnan(fftValueBin) )
        //     {
        //         fftValueBin /= float(numBins);
        //     }
        //     else
        //     {
        //         fftValueBin = 0.f;
        //     }
        //     mFftData[i] = fftValueBin;
        // }

        //convert them to decibels
        auto offset = 512;
        for( int i = 0 + offset; i < numBins+offset; ++i )
        {
            mFftData[i-offset] = juce::Decibels::gainToDecibels(mFftData[i], negativeInfinity);
        }

        int numSamples = (int) mFftData.size();

        juce::AudioBuffer<float> buffer(1, numSamples);
        buffer.copyFrom(0, 0, mFftData.data(), numSamples);
        mFftDataFifo.push(buffer); // Need to template or create a vectorBufferFifo that can handle std::vector<float>
    }

    void changeOrder(FFTOrder newOrder)
    {
        //when you change order, recreate the window, forwardFFT, fifo, fftData
        //also reset the fifoIndex
        //things that need recreating should be created on the heap via std::make_unique<>

        order = newOrder;
        auto fftSize = getFFTSize();

        forwardFFT = std::make_unique<juce::dsp::FFT>(order);
        mWindow = std::make_unique<juce::dsp::WindowingFunction<float>>(fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris);

        mFftData.clear();
        mFftData.resize(fftSize , 0); // fftData is now std::vector<float>, so resize works

        mFftDataFifo.prepare(1, mFftData.size()); // Prepare the FIFO to hold vectors of this size
    }
    //==============================================================================
    int getFFTSize() const { return 2048; }
    int getNumAvailableFFTDataBlocks() const { return mFftDataFifo.getNumAvailableForReading(); }
    std::vector<float> getFFTData() {
        if (getNumAvailableFFTDataBlocks() > 0) {
            juce::AudioBuffer<float> buffer;
            buffer.setSize(1, mFftData.size());
            mFftDataFifo.pull(buffer);

            std::vector<float> data;
            data.resize(mFftData.size());
            std::copy(buffer.getReadPointer(0), buffer.getReadPointer(0) + mFftData.size(), data.begin());
            return data;
        }
        return std::vector<float>{};
    }
    //==============================================================================
    bool getFFTData(BlockType& dataToFill) { return false; // fftDataFifo.pull(dataToFill);
        }
private:
    FFTOrder order;
    std::vector<float> mFftData; // Changed from BlockType to std::vector<float>
    std::unique_ptr<juce::dsp::FFT> forwardFFT;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> mWindow;

    // Assuming AudioBufferFifo is templated to store std::vector<float>
    // or that it can handle a BlockType that is compatible with std::vector<float>
    std::vector<float> mCurrentFFT;
    AudioBufferFifo mFftDataFifo; // The FIFO now stores std::vector<float>
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
        auto top = 0;
        auto bottom = fftBounds.getHeight();
        auto width = fftBounds.getWidth();

        int numBins = (int)fftSize / 2; // Why

        PathType p;
        p.preallocateSpace(3 * (int)fftBounds.getWidth()); // Why

        auto map = [bottom, top, negativeInfinity](float v)
        {
            return juce::jmap<float>(v,
                              negativeInfinity,
                              0.f,
                              float(bottom),
                              top);
        };

        auto y = map(renderData[0]);

        if( std::isnan(y) || std::isinf(y) )
            y = bottom;
        
        p.startNewSubPath(0, y);

        const int pathResolution = 2;
       // p.lineTo(300, 0);
        for( int binNum = 1; binNum < numBins; binNum += pathResolution )
        {
            y = map(renderData[binNum]);

            if( !std::isnan(y) && !std::isinf(y) )
            {
                auto binFreq = binNum * binWidth;
                auto normalizedBinX = juce::mapFromLog10(binFreq, 20.f, 20000.f);
                int binX = std::floor(normalizedBinX * width); // This could go to far
                p.lineTo(binX, y);
            }
        }
        mCurrentPath = p;
      //  mPathFifo.push(p);
    }
    

    int getNumPathsAvailable() const
    {
        return mPathFifo.size();
    }

    PathType getPath()
    {
        return mCurrentPath;
    }

private:
    std::queue<juce::Path> mPathFifo;
    PathType mPath;
    juce::Path mCurrentPath;
};


struct PathProducer {
    // It should have a fifo of path that can never be nul, always keeping an element
    PathProducer(AudioBufferFifo &scsf)
     : mAudioBufferFifo(&scsf)
    {
        leftChannelFFTDataGenerator.changeOrder(FFTOrder::order32); // Will change to fifo to make biger ff
        mFFTBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize());
    }
    // We should have the ability to merge buffers

    void process(Rectangle<float> fftBounds, double sampleRate);

    Path getPath() {
    // if (mPathProducer.getNumPathsAvailable() > 0)
    // {
    return mPathProducer.getPath();
    // }
    // else
    // { return Path();}
    }

private:
    AudioBufferFifo *mAudioBufferFifo;
    juce::AudioBuffer<float> mFFTBuffer;
    FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;
    AnalyzerPathGenerator<juce::Path> mPathProducer;
    juce::Path mCurrentPath;
    std::queue<juce::Path> mPathFifo;
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
