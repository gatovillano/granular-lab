#pragma once

#include "../Common/Parameters.h"
#include <juce_core/juce_core.h>

namespace GranularLab
{
    class LFO
    {
    public:
        LFO() = default;

        void prepare(double sampleRate);
        void reset();

        float getNextSample(float rateHz, LFOShape shape) noexcept;

    private:
        double currentSampleRate { 44100.0 };
        double phase { 0.0 };
        float currentSH { 0.0f };
        float nextSH { 0.0f };
        float shPhase { 0.0f };
        uint32_t rngState { 987654321 };

        inline float getRand() noexcept
        {
            rngState ^= rngState << 13;
            rngState ^= rngState >> 17;
            rngState ^= rngState << 5;
            return (static_cast<float>(rngState & 0x7FFFFFFF) / 1073741824.0f) - 1.0f; // -1 to 1
        }
    };

    class ModulationEngine
    {
    public:
        ModulationEngine() = default;

        void prepare(double sampleRate);
        void reset();

        struct ModulationOutput
        {
            float positionMod { 0.0f };
            float grainSizeMod { 0.0f };
            float densityMod { 0.0f };
            float pitchMod { 0.0f };
            float panMod { 0.0f };
            float cutoffMod { 0.0f };
        };

        struct LFOConfig
        {
            float rateHz { 1.0f };
            float depth { 0.0f };
            ModTarget target { ModTarget::None };
            LFOShape shape { LFOShape::Sine };
        };

        ModulationOutput process(const LFOConfig& lfo1Cfg, const LFOConfig& lfo2Cfg) noexcept;

    private:
        LFO lfo1;
        LFO lfo2;
    };
}
