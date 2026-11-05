#include "SynthGrain.h"
#include <juce_core/juce_core.h>

namespace GranularSynthLab
{
    void SynthGrain::init(double startPos, double lengthInSamples, double rate,
                          float panVal, GranularLab::EnvelopeShape envShape, bool reverse,
                          float gain, float durationMs, float pitchSemitones, float normPos) noexcept
    {
        currentPosition = startPos;
        durationSamples = std::max(4.0, lengthInSamples);
        elapsedSamples = 0.0;
        playbackRate = std::max(0.001, rate);
        pan = std::clamp(panVal, -1.0f, 1.0f);

        // Equal-power panning
        float angle = (pan + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
        panL = std::cos(angle);
        panR = std::sin(angle);

        shape = envShape;
        isReverse = reverse;
        grainGain = gain;
        sizeMs = durationMs;
        pitch = pitchSemitones;
        startNormPos = normPos;
        currentAmp = 0.0f;
        active = true;
    }

    float SynthGrain::computeWindow(float progress) const noexcept
    {
        progress = std::clamp(progress, 0.0f, 1.0f);

        switch (shape)
        {
            case GranularLab::EnvelopeShape::Hann:
                return 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * progress));

            case GranularLab::EnvelopeShape::Gaussian:
            {
                float x = (progress - 0.5f) * 4.0f;
                return std::exp(-0.5f * x * x);
            }

            case GranularLab::EnvelopeShape::Tukey:
            {
                constexpr float alpha = 0.5f;
                if (progress < 0.5f * alpha)
                    return 0.5f * (1.0f + std::cos(juce::MathConstants<float>::pi * (2.0f * progress / alpha - 1.0f)));
                else if (progress > 1.0f - 0.5f * alpha)
                    return 0.5f * (1.0f + std::cos(juce::MathConstants<float>::pi * (2.0f * (1.0f - progress) / alpha - 1.0f)));
                return 1.0f;
            }

            case GranularLab::EnvelopeShape::Triangle:
                return 1.0f - std::abs(2.0f * progress - 1.0f);

            default:
                return 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * progress));
        }
    }

    void SynthGrain::processSample(const SampleBuffer& buffer, float& outL, float& outR) noexcept
    {
        if (!active) return;

        float progress = static_cast<float>(elapsedSamples / durationSamples);
        float window = computeWindow(progress);
        currentAmp = window * grainGain;

        float sL = buffer.readHermite(0, currentPosition);
        float sR = buffer.readHermite(1, currentPosition);

        outL += sL * currentAmp * panL;
        outR += sR * currentAmp * panR;

        if (isReverse)
            currentPosition -= playbackRate;
        else
            currentPosition += playbackRate;

        elapsedSamples += 1.0;
        if (elapsedSamples >= durationSamples)
        {
            active = false;
        }
    }
}
