#include "AnalyzerComponent.h"
#include "../../dsp/Processor.h"

ResponseCurveComponent::ResponseCurveComponent(SkeletonAudioProcessor &p,
                                              juce::AudioProcessorValueTreeState& inParams,
                                              ParameterSetup& inParameterSetup)
: mProcessor(p),
  mParameters(inParams),
  mParameterSetup(inParameterSetup),
  mLeftPathProducer(mProcessor.getAudioBufferFifo())
{
  const auto &params = mProcessor.getParameters();
  for (auto param : params)
  {
    param->addListener(this);
  }

  updateChain();
  startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
  const auto &params = mProcessor.getParameters();
  for (auto param : params)
  {
    param->removeListener(this);
  }
  stopTimer();
}
void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
  parametersChanged.set(true);
}

//===========================Lets move from timer calleback in here
void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate) {
  // FFT START HERE SEEMS HARDDDD
  juce::AudioBuffer<float> tempIncomingBuffer(1, 2048);
  bool available = false;
  // Pull from fifo and ask for rendering // Merging buffer could be here to increase fft precision
  if (mAudioBufferFifo->getNumAvailableForReading() > 2048 / 441) // To change TODO using bufferSize
  {
    available = true;
    int writePosition = 0;
    juce::AudioBuffer<float> pulledBuffer;
    for (int i = 0; i < 2048 / 441; ++i) {
      int numSamplesToCopy = juce::jmin(
        pulledBuffer.getNumSamples(),
        tempIncomingBuffer.getNumSamples() - writePosition
      );
      mAudioBufferFifo->pull(pulledBuffer);
      // Copy samples from pulledBuffer to tempIncomingBuffer
      for (int ch = 0; ch < tempIncomingBuffer.getNumChannels(); ++ch) {
        tempIncomingBuffer.copyFrom(ch, writePosition, pulledBuffer, ch, 0, numSamplesToCopy);
      }

      writePosition += numSamplesToCopy;
    }
  }

  if (available) {
    leftChannelFFTDataGenerator.produceFFTDataForRendering(tempIncomingBuffer, -48.f);
  }


  const auto fftSize = leftChannelFFTDataGenerator.getFFTSize();

  // 48000/2048 = 23 hz : this is the binwidth //Carefull this could mess up TODO
  const auto binWidth = Mappers::getSamplerate() / (double) fftSize;

  auto fftData = leftChannelFFTDataGenerator.getFFTData();
  if (fftData.data()) {
    mPathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, -48.f);
  }


  mCurrentPath = mPathProducer.getPath();
  while (mPathProducer.getNumPathsAvailable()) {
    // mPathProducer.getPath();
  }
}

void ResponseCurveComponent::timerCallback()
{
  if(shouldShowFFTAnalisis){
      auto fftBounds = getAnalysisArea().toFloat();
      auto sampleRate = mProcessor.getSampleRate();
      mLeftPathProducer.process(fftBounds, sampleRate);
  }


  if (parametersChanged.compareAndSetBool(false, true))
  {
    // update the monochain
    updateChain();
  }
  repaint();
}

void ResponseCurveComponent::updateChain()
{
  auto sampleRate = mProcessor.getSampleRate();

  auto setup = mParameterSetup.getAudioThreadParams();
  auto bell1 = setup->mBell1Coeffs;
  auto bell2 = setup->mBell2Coeffs;
  auto lowCut = setup->mHighPassCoeffs;
  auto highShelf = setup->mHighShelfCoeffs ;

  monoChain.get<ChainPosition::Bell1>().coefficients = bell1;
  monoChain.get<ChainPosition::Bell2>().coefficients = bell2;
  monoChain.get<ChainPosition::LowCut>().coefficients = lowCut;
  monoChain.get<ChainPosition::HighShelf>().coefficients = highShelf;
}


void ResponseCurveComponent::paint(juce::Graphics &g)
{
  using namespace juce;
  updateChain();

  // g.drawImage(background, getLocalBounds().toFloat());
  auto responseArea = getAnalysisArea();
  auto w = responseArea.getWidth();
  auto sampleRate = 44100.0;
  std::vector<double> mags(w);
  for (int i = 0; i < w; ++i)
  {
    double mag = 1.f;
    auto freq = juce::mapToLog10(double(i) / double(w), 20.0, 20000.0);

    if (!monoChain.isBypassed<ChainPosition::Bell1>())
      mag *= monoChain.get<ChainPosition::Bell1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    if (!monoChain.isBypassed<ChainPosition::Bell2>())
      mag *= monoChain.get<ChainPosition::Bell2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    if (!monoChain.isBypassed<ChainPosition::LowCut>())
      mag *= monoChain.get<ChainPosition::LowCut>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
    if (!monoChain.isBypassed<ChainPosition::HighShelf>())
      mag *= monoChain.get<ChainPosition::HighShelf>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

    mags[i] = juce::Decibels::gainToDecibels(mag);
  }

  juce::Path responseCurve;
  double outputMin = responseArea.getBottom();
  double outputMax = responseArea.getY();

  auto map = [outputMin, outputMax](double input)
  {
    return juce::jmap(input, -24.0, 24.0, outputMin, outputMax);
  };

  responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));

  for (int i = 1; i < mags.size(); ++i)
  {
    responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
  }

  g.setColour(juce::Colours::red);
  g.strokePath(responseCurve, juce::PathStrokeType(2.f));

  g.setColour(juce::Colours::blue);
  juce::Path mAnalyzerPath;
  mAnalyzerPath = mLeftPathProducer.getPath();
  juce::Rectangle<float> pathBounds = mAnalyzerPath.getBounds();

  g.strokePath(mAnalyzerPath, juce::PathStrokeType(2.f));
  //g.strokePath(mLeftPathProducer.getPath(), juce::PathStrokeType(2.f));
}

void ResponseCurveComponent::resized()
{
  using namespace juce;
  background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);
  Graphics g(background);
  Array<float> freqs{
      20, /*30, 40,*/ 50, 100,
      200, /*300, 400,*/ 500, 1000,
      2000, /*3000, 4000,*/ 5000, 10000, 20000

  };
  auto renderArea = getAnalysisArea();
  auto left = renderArea.getX();
  auto right = renderArea.getRight();
  auto top = renderArea.getY();
  auto bottom = renderArea.getBottom();
  auto width = renderArea.getWidth();

  Array<float> xs;
  for (auto f : freqs)
  {
    auto normX = mapFromLog10(f, 20.f, 20000.f);
    xs.add(left + width * normX);
  }

  g.setColour(Colours::dimgrey);
  for (auto x : xs)
  {
    // auto normX = mapFromLog10(f,20.f,20000.f);
    g.drawVerticalLine(x, top, bottom);
  }

  Array<float> gain{
      -24, -12, 0, 12, 24};
  for (auto db : gain)
  {
    auto y = jmap(db, -24.f, 24.f, float(bottom), float(top));
    g.setColour(db == 0.f ? Colours::purple : Colours::darkgrey);
    g.drawHorizontalLine(y, left, right);
  }
  //  g.drawRect(getAnalysisArea());

  g.setColour(Colours::lightgrey);
  const int fontHeight = 10;
  g.setFont(fontHeight);

  for (int i = 0; i < freqs.size(); i++)
  {
    auto f = freqs[i];
    auto x = xs[i];
    bool addK = false;
    String str;
    if (f > 999.f)
    {
      addK = true;
      f /= 1000.f;
    }

    str << f;
    if (addK)
    {
      str << "k";
    }
    str << "Hz";
    auto textWidth = g.getCurrentFont().getStringWidth(str);

    Rectangle<int> r;
    r.setSize(textWidth, fontHeight);
    r.setCentre(x, 0);
    r.setY(1);
    g.drawFittedText(str, r, juce::Justification::centred, 1);
  }

  for (auto db : gain)
  {
    auto y = jmap(db, -24.f, 24.f, float(bottom), float(top));
    String str;
    if (db > 0)
    {
      str << "+";
    }
    str << db;
    auto textWidth = g.getCurrentFont().getStringWidth(str);
    Rectangle<int> r;
    r.setSize(textWidth, fontHeight);
    r.setX(getWidth() - textWidth);
    r.setCentre(r.getCentreX(), y);

    // g.setColour(db == 0.f ? Colours::purple : Colours::lightgrey);
    // g.drawFittedText(str, r, juce::Justification::centred, 1);

    str.clear();
    str << (db - 24.f);
    textWidth = g.getCurrentFont().getStringWidth(str);
    r.setX(1);
    r.setSize(textWidth, fontHeight);
    g.setColour(Colours::lightgrey);
    g.drawFittedText(str, r, juce::Justification::centred, 1);
  }
}

juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
{
  auto bounds = getLocalBounds();
  // One instance of live constant per line attention
  //  bounds.reduce(10,
  //  8);
  bounds.removeFromTop(12);
  bounds.removeFromBottom(2);
  bounds.removeFromLeft(20);
  bounds.removeFromRight(20);

  return bounds;
}
juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
{
  auto bounds = getRenderArea();
  return bounds;
}