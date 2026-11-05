#include "ModulationEngine.h"

namespace GranularLab
{
    void LFO::prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;
        phase = 0.0;
        currentSH = getRand();
        nextSH = getRand();
        shPhase = 0.0f;
    }

    void LFO::reset()
    {
        phase = 0.0;
        shPhase = 0.0f;
    }

    float LFO::getNextSample(float rateHz, LFOShape shape) noexcept
    {
        rateHz = std::clamp(rateHz, 0.01f, 50.0f);
        double phaseIncrement = (rateHz * juce::MathConstants<double>::twoPi) / currentSampleRate;

        float output = 0.0f;
        switch (shape)
        {
            case LFOShape::Sine:
                output = static_cast<float>(std::sin(phase));
                break;

            case LFOShape::Triangle:
            {
                // Triangle normalized to [-1, 1]
                double norm = phase / juce::MathConstants<double>::twoPi;
                output = static_cast<float>(2.0 * std::abs(2.0 * (norm - std::floor(norm + 0.5))) - 1.0);
                break;
            }

            case LFOShape::Saw:
            {
                // Saw rising from -1 to 1
                double norm = phase / juce::MathConstants<double>::twoPi;
                output = static_cast<float>(2.0 * norm - 1.0);
                break;
            }

            case LFOShape::SampleAndHold:
            {
                // Smoothly interpolated Random S&H (cosine easing between steps)
                float t = 0.5f * (1.0f - std::cos(static_cast<float>(phase)));
                output = currentSH + t * (nextSH - currentSH);
                break;
            }

            default:
                output = static_cast<float>(std::sin(phase));
                break;
        }

        phase += phaseIncrement;
        if (phase >= juce::MathConstants<double>::twoPi)
        {
            phase -= juce::MathConstants<double>::twoPi;
            if (shape == LFOShape::SampleAndHold)
            {
                currentSH = nextSH;
                nextSH = getRand();
            }
        }

        return output;
    }

    void ModulationEngine::prepare(double sampleRate)
    {
        lfo1.prepare(sampleRate);
        lfo2.prepare(sampleRate);
    }

    void ModulationEngine::reset()
    {
        lfo1.reset();
        lfo2.reset();
    }

    ModulationEngine::ModulationOutput ModulationEngine::process(const LFOConfig& lfo1Cfg, const LFOConfig& lfo2Cfg) noexcept
    {
        ModulationOutput out;

        float val1 = lfo1.getNextSample(lfo1Cfg.rateHz, lfo1Cfg.shape) * lfo1Cfg.depth;
        float val2 = lfo2.getNextSample(lfo2Cfg.rateHz, lfo2Cfg.shape) * lfo2Cfg.depth;

        auto applyMod = [&](ModTarget target, float val)
        {
            switch (target)
            {
                case ModTarget::Position:  out.positionMod  += val * 0.4f; break; // +/- 40% offset
                case ModTarget::GrainSize: out.grainSizeMod += val * 150.0f; break; // +/- 150ms
                case ModTarget::Density:   out.densityMod   += val * 40.0f; break; // +/- 40Hz
                case ModTarget::Pitch:     out.pitchMod     += val * 12.0f; break; // +/- 12 semitones
                case ModTarget::Pan:       out.panMod       += val * 0.7f; break; // +/- 0.7 pan
                case ModTarget::Cutoff:    out.cutoffMod    += val * 4000.0f; break; // +/- 4kHz
                default: break;
            }
        };

        applyMod(lfo1Cfg.target, val1);
        applyMod(lfo2Cfg.target, val2);

        return out;
    }
}
