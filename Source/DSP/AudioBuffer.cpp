#include "AudioBuffer.h"

namespace GranularLab
{
    void CircularAudioBuffer::prepare(double sampleRate, double maxSeconds)
    {
        currentSampleRate = sampleRate;
        bufferSize = static_cast<int>(std::max(1.0, std::round(sampleRate * maxSeconds)));
        
        bufferL.assign(bufferSize, 0.0f);
        bufferR.assign(bufferSize, 0.0f);
        writeIndex = 0;
        isFrozen.store(false, std::memory_order_relaxed);

        waveMinL.assign(WAVEFORM_POINTS, 0.0f);
        waveMaxL.assign(WAVEFORM_POINTS, 0.0f);
        waveMinR.assign(WAVEFORM_POINTS, 0.0f);
        waveMaxR.assign(WAVEFORM_POINTS, 0.0f);

        downsampleFactor = std::max(1, bufferSize / WAVEFORM_POINTS);
        downsampleCounter = 0;
        currentBlockMinL = 0.0f;
        currentBlockMaxL = 0.0f;
        currentBlockMinR = 0.0f;
        currentBlockMaxR = 0.0f;
    }

    void CircularAudioBuffer::reset()
    {
        std::fill(bufferL.begin(), bufferL.end(), 0.0f);
        std::fill(bufferR.begin(), bufferR.end(), 0.0f);
        writeIndex = 0;

        std::fill(waveMinL.begin(), waveMinL.end(), 0.0f);
        std::fill(waveMaxL.begin(), waveMaxL.end(), 0.0f);
        std::fill(waveMinR.begin(), waveMinR.end(), 0.0f);
        std::fill(waveMaxR.begin(), waveMaxR.end(), 0.0f);
        downsampleCounter = 0;
    }

    void CircularAudioBuffer::write(float sampleL, float sampleR, float feedbackAmount, float feedbackL, float feedbackR) noexcept
    {
        if (bufferSize <= 0) return;

        if (!isFrozen.load(std::memory_order_relaxed))
        {
            // Soft-saturation on feedback to prevent explosion
            float fbL = std::tanh(feedbackL) * feedbackAmount;
            float fbR = std::tanh(feedbackR) * feedbackAmount;

            float inL = std::clamp(sampleL + fbL, -2.0f, 2.0f);
            float inR = std::clamp(sampleR + fbR, -2.0f, 2.0f);

            bufferL[writeIndex] = inL;
            bufferR[writeIndex] = inR;

            // Waveform peak tracking
            currentBlockMinL = std::min(currentBlockMinL, inL);
            currentBlockMaxL = std::max(currentBlockMaxL, inL);
            currentBlockMinR = std::min(currentBlockMinR, inR);
            currentBlockMaxR = std::max(currentBlockMaxR, inR);

            if (++downsampleCounter >= downsampleFactor)
            {
                downsampleCounter = 0;
                int waveIdx = (writeIndex / downsampleFactor) % WAVEFORM_POINTS;
                waveMinL[waveIdx] = currentBlockMinL;
                waveMaxL[waveIdx] = currentBlockMaxL;
                waveMinR[waveIdx] = currentBlockMinR;
                waveMaxR[waveIdx] = currentBlockMaxR;

                currentBlockMinL = 0.0f;
                currentBlockMaxL = 0.0f;
                currentBlockMinR = 0.0f;
                currentBlockMaxR = 0.0f;
            }

            if (++writeIndex >= bufferSize)
                writeIndex = 0;
        }
    }

    float CircularAudioBuffer::readHermite(int channel, double position) const noexcept
    {
        if (bufferSize <= 0) return 0.0f;

        // Wrap position into [0, bufferSize)
        double intPart;
        double frac = std::modf(position, &intPart);
        int i1 = wrapIndex(static_cast<int>(intPart));
        if (frac < 0.0)
        {
            frac += 1.0;
            i1 = wrapIndex(i1 - 1);
        }

        int i0 = wrapIndex(i1 - 1);
        int i2 = wrapIndex(i1 + 1);
        int i3 = wrapIndex(i1 + 2);

        const auto& buf = (channel == 0) ? bufferL : bufferR;
        float y0 = buf[i0];
        float y1 = buf[i1];
        float y2 = buf[i2];
        float y3 = buf[i3];

        // 4-point, 3rd-order Hermite interpolation
        float c0 = y1;
        float c1 = 0.5f * (y2 - y0);
        float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
        float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

        float f = static_cast<float>(frac);
        return ((c3 * f + c2) * f + c1) * f + c0;
    }

    float CircularAudioBuffer::readLinear(int channel, double position) const noexcept
    {
        if (bufferSize <= 0) return 0.0f;

        double intPart;
        double frac = std::modf(position, &intPart);
        int i0 = wrapIndex(static_cast<int>(intPart));
        if (frac < 0.0)
        {
            frac += 1.0;
            i0 = wrapIndex(i0 - 1);
        }
        int i1 = wrapIndex(i0 + 1);

        const auto& buf = (channel == 0) ? bufferL : bufferR;
        float y0 = buf[i0];
        float y1 = buf[i1];
        return y0 + static_cast<float>(frac) * (y1 - y0);
    }

    void CircularAudioBuffer::getWaveformSnapshot(std::vector<float>& outMinL, std::vector<float>& outMaxL,
                                                  std::vector<float>& outMinR, std::vector<float>& outMaxR) const
    {
        outMinL = waveMinL;
        outMaxL = waveMaxL;
        outMinR = waveMinR;
        outMaxR = waveMaxR;
    }
}
