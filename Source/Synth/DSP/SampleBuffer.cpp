#include "SampleBuffer.h"
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

namespace GranularSynthLab
{
    SampleBuffer::SampleBuffer()
    {
        // Initialize default waveform (Supersaw)
        auto initialData = std::make_shared<SampleData>();
        initialData->lengthSamples = 44100 * 2;
        initialData->sampleRate = 44100.0;
        initialData->left.resize(initialData->lengthSamples, 0.0f);
        initialData->right.resize(initialData->lengthSamples, 0.0f);
        initialData->name = "Supersaw";
        initialData->isOscillator = true;
        initialData->shape = OscShape::Supersaw;

        computePeaks(*initialData);
        currentData = initialData;
        activeData.store(initialData);

        generateOscillator(OscShape::Supersaw);
    }

    void SampleBuffer::prepare(double sampleRate)
    {
        currentHostSampleRate = (sampleRate > 8000.0) ? sampleRate : 44100.0;
        if (currentMode.load() == SoundSourceMode::Oscillator)
        {
            generateOscillator(currentOscShape.load());
        }
    }

    void SampleBuffer::computePeaks(SampleData& data)
    {
        constexpr int NUM_PEAKS = 512;
        data.minPeaksL.assign(NUM_PEAKS, 0.0f);
        data.maxPeaksL.assign(NUM_PEAKS, 0.0f);
        data.minPeaksR.assign(NUM_PEAKS, 0.0f);
        data.maxPeaksR.assign(NUM_PEAKS, 0.0f);

        if (data.lengthSamples <= 0 || data.left.empty())
            return;

        int step = std::max(1, data.lengthSamples / NUM_PEAKS);

        for (int p = 0; p < NUM_PEAKS; ++p)
        {
            int startIdx = p * step;
            int endIdx = std::min(data.lengthSamples, startIdx + step);

            float minL = 0.0f, maxL = 0.0f;
            float minR = 0.0f, maxR = 0.0f;

            for (int i = startIdx; i < endIdx; ++i)
            {
                float sL = data.left[static_cast<size_t>(i)];
                float sR = data.right[static_cast<size_t>(i)];
                minL = std::min(minL, sL);
                maxL = std::max(maxL, sL);
                minR = std::min(minR, sR);
                maxR = std::max(maxR, sR);
            }

            data.minPeaksL[static_cast<size_t>(p)] = minL;
            data.maxPeaksL[static_cast<size_t>(p)] = maxL;
            data.minPeaksR[static_cast<size_t>(p)] = minR;
            data.maxPeaksR[static_cast<size_t>(p)] = maxR;
        }
    }

    void SampleBuffer::generateOscillator(OscShape shape)
    {
        currentOscShape.store(shape);
        currentMode.store(SoundSourceMode::Oscillator);

        auto newData = std::make_shared<SampleData>();
        newData->sampleRate = currentHostSampleRate;
        newData->isOscillator = true;
        newData->shape = shape;

        // Generate 2 seconds of high quality looped sound at C3 (130.81278 Hz)
        const double baseFreq = 130.81278; // C3
        const int numSamples = static_cast<int>(currentHostSampleRate * 2.0);
        newData->lengthSamples = numSamples;
        newData->left.resize(static_cast<size_t>(numSamples), 0.0f);
        newData->right.resize(static_cast<size_t>(numSamples), 0.0f);

        const double twoPi = juce::MathConstants<double>::twoPi;

        switch (shape)
        {
            case OscShape::Supersaw:
            {
                newData->name = "Supersaw Stack";
                const double detunes[7] = { 0.982, 0.991, 0.996, 1.0, 1.004, 1.009, 1.018 };
                const float pansL[7]   = { 0.9f,  0.3f,  0.7f,  0.5f, 0.3f,  0.7f,  0.1f };
                const float pansR[7]   = { 0.1f,  0.7f,  0.3f,  0.5f, 0.7f,  0.3f,  0.9f };

                for (size_t s = 0; s < 7; ++s)
                {
                    double freq = baseFreq * detunes[s];
                    double phase = (s * 0.142);
                    for (int i = 0; i < numSamples; ++i)
                    {
                        double saw = 2.0 * phase - 1.0;
                        newData->left[static_cast<size_t>(i)] += static_cast<float>(saw * pansL[s] * 0.22);
                        newData->right[static_cast<size_t>(i)] += static_cast<float>(saw * pansR[s] * 0.22);

                        phase += freq / currentHostSampleRate;
                        if (phase >= 1.0) phase -= 1.0;
                    }
                }
                break;
            }

            case OscShape::Sine:
            {
                newData->name = "Warm Sine";
                for (int i = 0; i < numSamples; ++i)
                {
                    double t = static_cast<double>(i) / currentHostSampleRate;
                    double s = 0.85 * std::sin(twoPi * baseFreq * t) + 0.15 * std::sin(twoPi * baseFreq * 2.0 * t);
                    newData->left[static_cast<size_t>(i)] = static_cast<float>(s);
                    newData->right[static_cast<size_t>(i)] = static_cast<float>(s);
                }
                break;
            }

            case OscShape::Triangle:
            {
                newData->name = "Analog Triangle";
                double phase = 0.0;
                for (int i = 0; i < numSamples; ++i)
                {
                    double tri = 2.0 * std::abs(2.0 * phase - 1.0) - 1.0;
                    // soft saturation
                    float val = std::tanh(static_cast<float>(tri * 1.2));
                    newData->left[static_cast<size_t>(i)] = val;
                    newData->right[static_cast<size_t>(i)] = val;

                    phase += baseFreq / currentHostSampleRate;
                    if (phase >= 1.0) phase -= 1.0;
                }
                break;
            }

            case OscShape::Square:
            {
                newData->name = "PWM Square Wave";
                double phase = 0.0;
                for (int i = 0; i < numSamples; ++i)
                {
                    double t = static_cast<double>(i) / currentHostSampleRate;
                    double pwm = 0.5 + 0.2 * std::sin(twoPi * 0.7 * t);
                    double sqrL = (phase < pwm) ? 0.7 : -0.7;
                    double sqrR = (phase < (1.0 - pwm)) ? 0.7 : -0.7;

                    newData->left[static_cast<size_t>(i)] = static_cast<float>(sqrL);
                    newData->right[static_cast<size_t>(i)] = static_cast<float>(sqrR);

                    phase += baseFreq / currentHostSampleRate;
                    if (phase >= 1.0) phase -= 1.0;
                }
                break;
            }

            case OscShape::Noise:
            {
                newData->name = "Pink Noise Cloud";
                juce::Random rng(12345);
                float b0L = 0.0f, b1L = 0.0f, b2L = 0.0f;
                float b0R = 0.0f, b1R = 0.0f, b2R = 0.0f;

                for (int i = 0; i < numSamples; ++i)
                {
                    float whiteL = rng.nextFloat() * 2.0f - 1.0f;
                    float whiteR = rng.nextFloat() * 2.0f - 1.0f;

                    b0L = 0.99886f * b0L + whiteL * 0.0555179f;
                    b1L = 0.99332f * b1L + whiteL * 0.0750759f;
                    b2L = 0.96900f * b2L + whiteL * 0.1538520f;
                    float pinkL = (b0L + b1L + b2L + whiteL * 0.5362f) * 0.15f;

                    b0R = 0.99886f * b0R + whiteR * 0.0555179f;
                    b1R = 0.99332f * b1R + whiteR * 0.0750759f;
                    b2R = 0.96900f * b2R + whiteR * 0.1538520f;
                    float pinkR = (b0R + b1R + b2R + whiteR * 0.5362f) * 0.15f;

                    newData->left[static_cast<size_t>(i)] = pinkL;
                    newData->right[static_cast<size_t>(i)] = pinkR;
                }
                break;
            }

            case OscShape::FMBell:
            {
                newData->name = "FM Crystal Bell";
                const double modRatio = 3.5;
                const double modIndex = 2.4;
                for (int i = 0; i < numSamples; ++i)
                {
                    double t = static_cast<double>(i) / currentHostSampleRate;
                    double modPhase = twoPi * baseFreq * modRatio * t;
                    double mod = modIndex * std::sin(modPhase);
                    double s = 0.8 * std::sin(twoPi * baseFreq * t + mod);

                    newData->left[static_cast<size_t>(i)] = static_cast<float>(s);
                    newData->right[static_cast<size_t>(i)] = static_cast<float>(s);
                }
                break;
            }

            case OscShape::VocalFormant:
            {
                newData->name = "Vocal Choir Formant";
                // Synthesize formant spectrum for vowel 'Aah' (C3 fundamental + 600Hz / 1050Hz / 2400Hz resonances)
                for (int i = 0; i < numSamples; ++i)
                {
                    double t = static_cast<double>(i) / currentHostSampleRate;
                    double sum = 0.0;
                    for (int h = 1; h <= 24; ++h)
                    {
                        double freqH = baseFreq * h;
                        double f1Dist = (freqH - 650.0) / 90.0;
                        double f2Dist = (freqH - 1100.0) / 110.0;
                        double f3Dist = (freqH - 2500.0) / 150.0;
                        double amp = std::exp(-0.5 * f1Dist * f1Dist) * 1.0 +
                                     std::exp(-0.5 * f2Dist * f2Dist) * 0.6 +
                                     std::exp(-0.5 * f3Dist * f3Dist) * 0.3;
                        sum += amp * std::sin(twoPi * freqH * t + (h * 0.5));
                    }
                    float val = std::tanh(static_cast<float>(sum * 0.25));
                    newData->left[static_cast<size_t>(i)] = val;
                    newData->right[static_cast<size_t>(i)] = val;
                }
                break;
            }

            default:
                break;
        }

        computePeaks(*newData);
        currentData = newData;
        activeData.store(newData);
    }

    bool SampleBuffer::loadFromFile(const juce::File& file, juce::AudioFormatManager& formatManager)
    {
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
        if (reader == nullptr)
            return false;

        const int maxSamples = static_cast<int>(reader->sampleRate * 60.0); // max 60 seconds
        const int numSamplesToRead = std::min(static_cast<int>(reader->lengthInSamples), maxSamples);
        if (numSamplesToRead <= 0)
            return false;

        auto newData = std::make_shared<SampleData>();
        newData->sampleRate = reader->sampleRate;
        newData->lengthSamples = numSamplesToRead;
        newData->name = file.getFileName().toStdString();
        newData->isOscillator = false;
        newData->left.resize(static_cast<size_t>(numSamplesToRead), 0.0f);
        newData->right.resize(static_cast<size_t>(numSamplesToRead), 0.0f);

        juce::AudioBuffer<float> tempBuffer(static_cast<int>(reader->numChannels), numSamplesToRead);
        reader->read(&tempBuffer, 0, numSamplesToRead, 0, true, true);

        // Copy left channel
        std::copy_n(tempBuffer.getReadPointer(0), numSamplesToRead, newData->left.data());

        // Copy right or duplicate left if mono
        if (reader->numChannels > 1)
        {
            std::copy_n(tempBuffer.getReadPointer(1), numSamplesToRead, newData->right.data());
        }
        else
        {
            std::copy_n(tempBuffer.getReadPointer(0), numSamplesToRead, newData->right.data());
        }

        // Normalize peak to ~0.9
        float peak = 0.0001f;
        for (int i = 0; i < numSamplesToRead; ++i)
        {
            peak = std::max(peak, std::abs(newData->left[static_cast<size_t>(i)]));
            peak = std::max(peak, std::abs(newData->right[static_cast<size_t>(i)]));
        }
        if (peak > 0.0001f)
        {
            float normScale = 0.9f / peak;
            for (int i = 0; i < numSamplesToRead; ++i)
            {
                newData->left[static_cast<size_t>(i)] *= normScale;
                newData->right[static_cast<size_t>(i)] *= normScale;
            }
        }

        computePeaks(*newData);
        currentMode.store(SoundSourceMode::Sample);
        currentData = newData;
        activeData.store(newData);
        return true;
    }

    float SampleBuffer::readHermite(int channel, double position) const noexcept
    {
        auto dataPtr = activeData.load(std::memory_order_relaxed);
        if (!dataPtr || dataPtr->lengthSamples <= 4)
            return 0.0f;

        const auto& buf = (channel == 0) ? dataPtr->left : dataPtr->right;
        const int len = dataPtr->lengthSamples;

        // Wrap position within bounds seamlessly
        double wrapped = std::fmod(position, static_cast<double>(len));
        if (wrapped < 0.0) wrapped += len;

        int i1 = static_cast<int>(wrapped);
        double frac = wrapped - i1;

        int i0 = (i1 - 1 + len) % len;
        int i2 = (i1 + 1) % len;
        int i3 = (i1 + 2) % len;

        float p0 = buf[static_cast<size_t>(i0)];
        float p1 = buf[static_cast<size_t>(i1)];
        float p2 = buf[static_cast<size_t>(i2)];
        float p3 = buf[static_cast<size_t>(i3)];

        // Hermite 4-point cubic spline interpolation
        float c0 = p1;
        float c1 = 0.5f * (p2 - p0);
        float c2 = p0 - 2.5f * p1 + 2.0f * p2 - 0.5f * p3;
        float c3 = 0.5f * (p3 - p0) + 1.5f * (p1 - p2);

        float f = static_cast<float>(frac);
        return ((c3 * f + c2) * f + c1) * f + c0;
    }

    void SampleBuffer::getWaveformSnapshot(std::vector<float>& outMinL, std::vector<float>& outMaxL,
                                          std::vector<float>& outMinR, std::vector<float>& outMaxR) const
    {
        auto dataPtr = activeData.load(std::memory_order_relaxed);
        if (!dataPtr || dataPtr->minPeaksL.empty())
        {
            outMinL.assign(512, 0.0f);
            outMaxL.assign(512, 0.0f);
            outMinR.assign(512, 0.0f);
            outMaxR.assign(512, 0.0f);
            return;
        }

        outMinL = dataPtr->minPeaksL;
        outMaxL = dataPtr->maxPeaksL;
        outMinR = dataPtr->minPeaksR;
        outMaxR = dataPtr->maxPeaksR;
    }

    int SampleBuffer::getLengthSamples() const noexcept
    {
        auto dataPtr = activeData.load(std::memory_order_relaxed);
        return dataPtr ? dataPtr->lengthSamples : 0;
    }

    double SampleBuffer::getBufferSampleRate() const noexcept
    {
        auto dataPtr = activeData.load(std::memory_order_relaxed);
        return dataPtr ? dataPtr->sampleRate : 44100.0;
    }

    std::string SampleBuffer::getSourceTitle() const
    {
        auto dataPtr = activeData.load(std::memory_order_relaxed);
        return dataPtr ? dataPtr->name : "None";
    }

    bool SampleBuffer::isSampleLoaded() const noexcept
    {
        auto dataPtr = activeData.load(std::memory_order_relaxed);
        return dataPtr && !dataPtr->isOscillator;
    }
}
