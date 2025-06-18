#pragma once
#include <JuceHeader.h>
#include <array>
// This should be thought again using juce buffers no ? Warning because of templating
#pragma once
#include <JuceHeader.h>
#include <array>

struct AudioBufferFifo
{
    void prepare(int numChannels, int numSamples)
    {
        for (auto& buffer : buffers)
        {
            buffer.setSize(numChannels,
                           numSamples,
                           false,
                           true,
                           true);
            buffer.clear();
        }
    }

    bool push(const juce::AudioBuffer<float>& t)
    {
        auto write = fifo.write(1);
        if (write.blockSize1 > 0)
        {
            buffers[write.startIndex1] = t;
            return true;
        }

        return false;
    }

    bool pull(juce::AudioBuffer<float>& t)
    {
        auto read = fifo.read(1);
        if (read.blockSize1 > 0)
        {
            t = buffers[read.startIndex1];
            return true;
        }

        return false;
    }

    int getNumAvailableForReading() const
    {
        return fifo.getNumReady();
    }

private:
    static constexpr int Capacity = 100;
    std::array<juce::AudioBuffer<float>, Capacity> buffers;
    juce::AbstractFifo fifo{ Capacity };
};


enum Channel
{
    Right,
    Left //effectively 1
};
struct SingleChannelSampleFifo
{
    SingleChannelSampleFifo(Channel ch) : channelToUse(ch) {
        prepared.set(false);
    }

    void update(const juce::AudioBuffer<float>& buffer)
    {
        jassert(prepared.get());
        jassert(buffer.getNumChannels() > channelToUse);
        auto* channelPtr = buffer.getReadPointer(channelToUse);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            pushNextSampleIntoFifo(channelPtr[i]);
        }
    }

    void prepare(int bufferSize)
    {
        prepared.set(false);
        size.set(bufferSize);

        bufferToFill.setSize(1, bufferSize, false, true, true);
        audioBufferFifo.prepare(1, bufferSize);
        fifoIndex = 0;
        prepared.set(true);
    }

    int getNumCompleteBuffersAvailable() const { return audioBufferFifo.getNumAvailableForReading(); }
    bool isPrepared() const { return prepared.get(); }
    int getSize() const { return size.get(); }
    bool getAudioBuffer(juce::AudioBuffer<float>& buf) { return audioBufferFifo.pull(buf); }

private:
    Channel channelToUse;
    int fifoIndex = 0;
    AudioBufferFifo audioBufferFifo;
    juce::AudioBuffer<float> bufferToFill;
    juce::Atomic<bool> prepared = false;
    juce::Atomic<int> size = 0;

    void pushNextSampleIntoFifo(float sample)
    {
        if (fifoIndex == bufferToFill.getNumSamples())
        {
            auto ok = audioBufferFifo.push(bufferToFill);
            juce::ignoreUnused(ok);
            fifoIndex = 0;
        }

        bufferToFill.setSample(0, fifoIndex, sample);
        ++fifoIndex;
    }
};
