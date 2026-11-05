#include "EffectsEngine.h"
#include <cmath>
#include <algorithm>

namespace GranularLab
{
    // =========================================================================
    // StateVariableFilter
    // =========================================================================
    void StateVariableFilter::prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;
        reset();
    }

    void StateVariableFilter::reset()
    {
        s1_L = s2_L = 0.0f;
        s1_R = s2_R = 0.0f;
    }

    void StateVariableFilter::setParameters(FilterType type, float cutoffHz, float resonanceQ) noexcept
    {
        currentType = type;
        cutoffHz = std::clamp(cutoffHz, 20.0f, static_cast<float>(currentSampleRate * 0.49));
        resonanceQ = std::clamp(resonanceQ, 0.1f, 10.0f);

        // Bilinear transform pre-warping
        g = std::tan(static_cast<float>(juce::MathConstants<double>::pi) * cutoffHz / static_cast<float>(currentSampleRate));
        k = 1.0f / resonanceQ;
        a1 = 1.0f / (1.0f + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
    }

    void StateVariableFilter::processSample(float inL, float inR, float& outL, float& outR) noexcept
    {
        // Left channel SVF
        float v3_L = inL - s2_L;
        float v1_L = a1 * s1_L + a2 * v3_L;
        float v2_L = s2_L + a2 * s1_L + a3 * v3_L;
        s1_L = 2.0f * v1_L - s1_L;
        s2_L = 2.0f * v2_L - s2_L;

        // Right channel SVF
        float v3_R = inR - s2_R;
        float v1_R = a1 * s1_R + a2 * v3_R;
        float v2_R = s2_R + a2 * s1_R + a3 * v3_R;
        s1_R = 2.0f * v1_R - s1_R;
        s2_R = 2.0f * v2_R - s2_R;

        switch (currentType)
        {
            case FilterType::LowPass:
                outL = v2_L;
                outR = v2_R;
                break;
            case FilterType::BandPass:
                outL = v1_L;
                outR = v1_R;
                break;
            case FilterType::HighPass:
                outL = inL - k * v1_L - v2_L;
                outR = inR - k * v1_R - v2_R;
                break;
            default:
                outL = v2_L;
                outR = v2_R;
                break;
        }
    }

    // =========================================================================
    // StereoDelay
    // =========================================================================
    void StereoDelay::prepare(double sampleRate, double maxDelaySeconds)
    {
        currentSampleRate = sampleRate;
        bufSize = static_cast<int>(sampleRate * maxDelaySeconds);
        delayBufL.assign(bufSize, 0.0f);
        delayBufR.assign(bufSize, 0.0f);
        writeIdx = 0;
        smoothedDelaySamples = static_cast<float>(sampleRate * 0.32);
    }

    void StereoDelay::reset()
    {
        std::fill(delayBufL.begin(), delayBufL.end(), 0.0f);
        std::fill(delayBufR.begin(), delayBufR.end(), 0.0f);
        writeIdx = 0;
    }

    void StereoDelay::processSample(float inL, float inR, float& outL, float& outR,
                                   float delayTimeMs, float feedback, float mix) noexcept
    {
        if (bufSize <= 0) return;

        float targetDelaySamples = (std::clamp(delayTimeMs, 5.0f, 2000.0f) * 0.001f) * static_cast<float>(currentSampleRate);
        smoothedDelaySamples += 0.001f * (targetDelaySamples - smoothedDelaySamples); // Smooth slew

        // Offset R slightly for ping-pong stereo widening (5ms offset)
        float delaySamplesL = smoothedDelaySamples;
        float delaySamplesR = std::min(static_cast<float>(bufSize - 2), smoothedDelaySamples + 0.005f * static_cast<float>(currentSampleRate));

        auto readInterpolated = [&](const std::vector<float>& buf, float delaySamples) -> float
        {
            float readPos = static_cast<float>(writeIdx) - delaySamples;
            while (readPos < 0.0f) readPos += static_cast<float>(bufSize);
            while (readPos >= static_cast<float>(bufSize)) readPos -= static_cast<float>(bufSize);

            int i0 = static_cast<int>(readPos);
            int i1 = (i0 + 1) % bufSize;
            float frac = readPos - static_cast<float>(i0);
            return buf[i0] + frac * (buf[i1] - buf[i0]);
        };

        float delayedL = readInterpolated(delayBufL, delaySamplesL);
        float delayedR = readInterpolated(delayBufR, delaySamplesR);

        // Feedback with soft clipping
        float fbL = std::tanh(delayedL * feedback);
        float fbR = std::tanh(delayedR * feedback);

        delayBufL[writeIdx] = inL + fbL;
        delayBufR[writeIdx] = inR + fbR;

        if (++writeIdx >= bufSize)
            writeIdx = 0;

        outL = inL + (delayedL - inL) * mix;
        outR = inR + (delayedR - inR) * mix;
    }

    // =========================================================================
    // AlgorithmicReverb
    // =========================================================================
    void AlgorithmicReverb::prepare(double sampleRate)
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = 1024;
        spec.numChannels = 2;
        reverbDSP.prepare(spec);
        reset();
    }

    void AlgorithmicReverb::reset()
    {
        reverbDSP.reset();
    }

    void AlgorithmicReverb::processBlock(float* left, float* right, int numSamples,
                                        float roomSize, float damping, float mix) noexcept
    {
        reverbParams.roomSize = std::clamp(roomSize, 0.0f, 1.0f);
        reverbParams.damping = std::clamp(damping, 0.0f, 1.0f);
        reverbParams.wetLevel = std::clamp(mix, 0.0f, 1.0f);
        reverbParams.dryLevel = 1.0f - reverbParams.wetLevel * 0.5f;
        reverbParams.width = 1.0f;
        reverbParams.freezeMode = 0.0f;
        reverbDSP.setParameters(reverbParams);

        float* const channelData[2] = { left, right };
        juce::AudioBuffer<float> buffer(channelData, 2, numSamples);
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        reverbDSP.process(context);
    }

    // =========================================================================
    // SaturationProcessor
    // =========================================================================
    void SaturationProcessor::reset()
    {
        dc_x1_L = dc_y1_L = 0.0f;
        dc_x1_R = dc_y1_R = 0.0f;
    }

    void SaturationProcessor::processSample(float inL, float inR, float& outL, float& outR,
                                           float drive, float mix) noexcept
    {
        auto saturate = [](float x, float d) -> float
        {
            float driven = x * d;
            // Warm tape-style cubic saturation with soft knee
            if (driven > 1.5f) return 1.0f;
            if (driven < -1.5f) return -1.0f;
            return driven - 0.15f * driven * driven * driven;
        };

        float satL = saturate(inL, drive);
        float satR = saturate(inR, drive);

        // 1-pole DC blocker to remove DC offset: y[n] = x[n] - x[n-1] + R*y[n-1] (R = 0.995)
        constexpr float R = 0.995f;
        float cleanL = satL - dc_x1_L + R * dc_y1_L;
        dc_x1_L = satL;
        dc_y1_L = cleanL;

        float cleanR = satR - dc_x1_R + R * dc_y1_R;
        dc_x1_R = satR;
        dc_y1_R = cleanR;

        outL = inL + (cleanL - inL) * mix;
        outR = inR + (cleanR - inR) * mix;
    }

    // =========================================================================
    // EffectsEngine
    // =========================================================================
    void EffectsEngine::prepare(double sampleRate, int maxBlockSize)
    {
        juce::ignoreUnused(maxBlockSize);
        filter.prepare(sampleRate);
        delay.prepare(sampleRate);
        reverb.prepare(sampleRate);
        saturation.reset();
    }

    void EffectsEngine::reset()
    {
        filter.reset();
        delay.reset();
        reverb.reset();
        saturation.reset();
    }

    void EffectsEngine::processBlock(float* const* channels, int numSamples, const Parameters& params) noexcept
    {
        float* l = channels[0];
        float* r = channels[1];

        // 1. Saturation
        if (params.satEnabled && params.satMix > 0.001f)
        {
            for (int i = 0; i < numSamples; ++i)
            {
                saturation.processSample(l[i], r[i], l[i], r[i], params.satDrive, params.satMix);
            }
        }

        // 2. Multimode Filter
        if (params.filterEnabled)
        {
            filter.setParameters(params.filterType, params.filterCutoff, params.filterResonance);
            for (int i = 0; i < numSamples; ++i)
            {
                filter.processSample(l[i], r[i], l[i], r[i]);
            }
        }

        // 3. Stereo Delay
        if (params.delayEnabled && params.delayMix > 0.001f)
        {
            for (int i = 0; i < numSamples; ++i)
            {
                delay.processSample(l[i], r[i], l[i], r[i], params.delayTimeMs, params.delayFeedback, params.delayMix);
            }
        }

        // 4. Algorithmic Reverb
        if (params.reverbEnabled && params.reverbMix > 0.001f)
        {
            reverb.processBlock(l, r, numSamples, params.reverbSize, params.reverbDamping, params.reverbMix);
        }
    }
}
