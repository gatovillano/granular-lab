#pragma once

#include "../Common/Parameters.h"
#include <juce_dsp/juce_dsp.h>
#include <vector>

namespace GranularLab
{
    // Real-time zero-delay feedback State Variable Filter
    class StateVariableFilter
    {
    public:
        void prepare(double sampleRate);
        void reset();
        void setParameters(FilterType type, float cutoffHz, float resonanceQ) noexcept;
        void processSample(float inL, float inR, float& outL, float& outR) noexcept;

    private:
        double currentSampleRate { 44100.0 };
        FilterType currentType { FilterType::LowPass };
        float g { 0.0f };
        float k { 0.0f };
        float a1 { 0.0f }, a2 { 0.0f }, a3 { 0.0f };

        // Integrator states for stereo
        float s1_L { 0.0f }, s2_L { 0.0f };
        float s1_R { 0.0f }, s2_R { 0.0f };
    };

    // Stereo Delay with smooth time interpolation & soft feedback
    class StereoDelay
    {
    public:
        void prepare(double sampleRate, double maxDelaySeconds = 2.5);
        void reset();
        void processSample(float inL, float inR, float& outL, float& outR,
                           float delayTimeMs, float feedback, float mix) noexcept;

    private:
        double currentSampleRate { 44100.0 };
        std::vector<float> delayBufL;
        std::vector<float> delayBufR;
        int bufSize { 0 };
        int writeIdx { 0 };
        float smoothedDelaySamples { 4410.0f };
    };

    // Algorithmic Stereo Reverb (Freeverb enhanced)
    class AlgorithmicReverb
    {
    public:
        void prepare(double sampleRate);
        void reset();
        void processBlock(float* left, float* right, int numSamples,
                          float roomSize, float damping, float mix) noexcept;

    private:
        juce::dsp::Reverb reverbDSP;
        juce::dsp::Reverb::Parameters reverbParams;
    };

    // Analog Style Soft Saturation
    class SaturationProcessor
    {
    public:
        void reset();
        void processSample(float inL, float inR, float& outL, float& outR,
                           float drive, float mix) noexcept;

    private:
        // DC blocker states
        float dc_x1_L { 0.0f }, dc_y1_L { 0.0f };
        float dc_x1_R { 0.0f }, dc_y1_R { 0.0f };
    };

    class EffectsEngine
    {
    public:
        EffectsEngine() = default;

        void prepare(double sampleRate, int maxBlockSize);
        void reset();

        struct Parameters
        {
            // Filter
            bool filterEnabled { true };
            FilterType filterType { FilterType::LowPass };
            float filterCutoff { 12000.0f };
            float filterResonance { 0.707f };

            // Delay
            bool delayEnabled { false };
            float delayTimeMs { 320.0f };
            float delayFeedback { 0.35f };
            float delayMix { 0.3f };

            // Reverb
            bool reverbEnabled { false };
            float reverbSize { 0.65f };
            float reverbDamping { 0.4f };
            float reverbMix { 0.35f };

            // Saturation
            bool satEnabled { false };
            float satDrive { 1.5f };
            float satMix { 0.5f };
        };

        void processBlock(float* const* channels, int numSamples, const Parameters& params) noexcept;

    private:
        StateVariableFilter filter;
        StereoDelay delay;
        AlgorithmicReverb reverb;
        SaturationProcessor saturation;
    };
}
