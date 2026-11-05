#pragma once

#include "SampleBuffer.h"
#include "Common/Parameters.h"
#include <cmath>

namespace GranularSynthLab
{
    class SynthGrain
    {
    public:
        SynthGrain() = default;

        void init(double startPos, double lengthInSamples, double rate,
                  float panVal, GranularLab::EnvelopeShape envShape, bool reverse,
                  float gain, float durationMs, float pitchSemitones, float normPos) noexcept;

        void processSample(const SampleBuffer& buffer, float& outL, float& outR) noexcept;

        bool isActive() const noexcept { return active; }
        float getLifeProgress() const noexcept
        {
            if (durationSamples > 0.0)
                return static_cast<float>(elapsedSamples / durationSamples);
            return 1.0f;
        }

        float getPan() const noexcept { return pan; }
        float getPitchShift() const noexcept { return pitch; }
        float getDurationMs() const noexcept { return sizeMs; }
        float getNormalizedPos() const noexcept { return startNormPos; }
        bool getIsReverse() const noexcept { return isReverse; }
        float getCurrentAmplitude() const noexcept { return currentAmp; }

        void kill() noexcept { active = false; }

    private:
        bool active { false };
        double currentPosition { 0.0 };
        double durationSamples { 1.0 };
        double elapsedSamples { 0.0 };
        double playbackRate { 1.0 };
        float panL { 0.707f };
        float panR { 0.707f };
        float pan { 0.0f };
        GranularLab::EnvelopeShape shape { GranularLab::EnvelopeShape::Hann };
        bool isReverse { false };
        float grainGain { 1.0f };
        float sizeMs { 50.0f };
        float pitch { 0.0f };
        float startNormPos { 0.0f };
        float currentAmp { 0.0f };

        float computeWindow(float progress) const noexcept;
    };
}
