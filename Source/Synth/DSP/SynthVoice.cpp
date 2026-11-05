#include "SynthVoice.h"
#include <algorithm>
#include <cmath>

namespace GranularSynthLab
{
    SynthVoice::SynthVoice()
    {
        seed = 1234567;
    }

    void SynthVoice::prepare(double sampleRate)
    {
        currentSampleRate = (sampleRate > 8000.0) ? sampleRate : 44100.0;
        ampAdsr.setSampleRate(currentSampleRate);
        kill();
    }

    float SynthVoice::fastRand() noexcept
    {
        seed = seed * 1664525u + 1013904223u;
        return static_cast<float>(seed & 0x00FFFFFF) / static_cast<float>(0x00FFFFFF);
    }

    void SynthVoice::kill() noexcept
    {
        isVoiceActive = false;
        currentMidiNote = -1;
        currentVelocity = 0.0f;
        noteAge = 0;
        grainSpawnAccumulator = 0.0;
        ampAdsr.reset();

        for (auto& g : grains)
            g.kill();
    }

    void SynthVoice::noteOn(int midiNote, float velocity, const VoiceParams& params)
    {
        currentMidiNote = midiNote;
        currentVelocity = std::clamp(velocity, 0.0f, 1.0f);
        noteAge = 0;

        double totalSemitones = static_cast<double>(midiNote - params.rootNote)
                              + (params.octave * 12.0)
                              + (params.fineTune / 100.0)
                              + currentPitchBend;

        notePitchRatio = std::pow(2.0, totalSemitones / 12.0);

        ampAdsr.setParameters(params.adsrParams);
        ampAdsr.noteOn();

        // Fire first grain immediately for instant attack
        grainSpawnAccumulator = 1.0;
        isVoiceActive = true;
    }

    void SynthVoice::noteOff(float /*velocity*/, bool allowTailOff)
    {
        if (allowTailOff)
        {
            ampAdsr.noteOff();
        }
        else
        {
            kill();
        }
    }

    void SynthVoice::pitchWheel(float pitchBendSemitones)
    {
        currentPitchBend = pitchBendSemitones;
    }

    void SynthVoice::spawnGrain(const SampleBuffer& buffer, const VoiceParams& params) noexcept
    {
        const int bufLen = buffer.getLengthSamples();
        if (bufLen <= 8)
            return;

        // Find inactive grain slot
        SynthGrain* targetGrain = nullptr;
        for (auto& g : grains)
        {
            if (!g.isActive())
            {
                targetGrain = &g;
                break;
            }
        }

        // If all full, steal the oldest/most progressed
        if (targetGrain == nullptr)
        {
            float maxProgress = -1.0f;
            for (auto& g : grains)
            {
                float prog = g.getLifeProgress();
                if (prog > maxProgress)
                {
                    maxProgress = prog;
                    targetGrain = &g;
                }
            }
        }

        if (targetGrain == nullptr)
            return;

        float sprayRand = (fastRand() * 2.0f - 1.0f) * params.spray * 0.5f;
        float normPos = std::clamp(params.position + sprayRand, 0.0f, 1.0f);
        double startPos = static_cast<double>(normPos) * bufLen;

        float pitchRandSemis = (fastRand() * 2.0f - 1.0f) * params.pitchRandom;
        double rate = notePitchRatio * std::pow(2.0, (pitchRandSemis + currentPitchBend) / 12.0);

        // Compensate for sample rate difference
        if (buffer.getBufferSampleRate() > 0.0)
            rate *= (buffer.getBufferSampleRate() / currentSampleRate);

        float panVal = (fastRand() * 2.0f - 1.0f) * params.panRandom;
        bool isRev = (fastRand() < params.reverseChance);

        double lengthInSamples = (params.grainSizeMs * 0.001) * currentSampleRate;

        // Scaling gain based on density
        float grainGain = 1.0f / std::sqrt(std::max(1.0f, params.densityHz * 0.15f));

        targetGrain->init(startPos, lengthInSamples, rate, panVal, params.windowShape,
                          isRev, grainGain, params.grainSizeMs, pitchRandSemis, normPos);
    }

    void SynthVoice::renderNextBlock(const SampleBuffer& buffer, float* outL, float* outR,
                                     int numSamples, const VoiceParams& params) noexcept
    {
        if (!isVoiceActive)
            return;

        noteAge += static_cast<uint32_t>(numSamples);
        const double rateIncrement = params.densityHz / currentSampleRate;

        for (int i = 0; i < numSamples; ++i)
        {
            float env = ampAdsr.getNextSample();

            if (!ampAdsr.isActive())
            {
                // Check if all grains finished
                bool anyGrainActive = false;
                for (const auto& g : grains)
                {
                    if (g.isActive())
                    {
                        anyGrainActive = true;
                        break;
                    }
                }

                if (!anyGrainActive)
                {
                    isVoiceActive = false;
                    currentMidiNote = -1;
                    break;
                }
            }
            else
            {
                grainSpawnAccumulator += rateIncrement;
                if (grainSpawnAccumulator >= 1.0)
                {
                    spawnGrain(buffer, params);
                    grainSpawnAccumulator -= 1.0;
                }
            }

            float sampleL = 0.0f;
            float sampleR = 0.0f;

            for (auto& g : grains)
            {
                if (g.isActive())
                {
                    g.processSample(buffer, sampleL, sampleR);
                }
            }

            float voiceGain = env * currentVelocity;
            outL[i] += sampleL * voiceGain;
            outR[i] += sampleR * voiceGain;
        }
    }

    size_t SynthVoice::collectVisualGrains(GranularLab::GrainVisualInfo* outArray, size_t maxCount) const noexcept
    {
        if (!isVoiceActive || outArray == nullptr || maxCount == 0)
            return 0;

        size_t written = 0;
        for (const auto& g : grains)
        {
            if (g.isActive() && written < maxCount)
            {
                outArray[written].normPos = g.getNormalizedPos();
                outArray[written].lifeProgress = g.getLifeProgress();
                outArray[written].pan = g.getPan();
                outArray[written].pitchShift = g.getPitchShift();
                outArray[written].amplitude = g.getCurrentAmplitude() * currentVelocity;
                outArray[written].sizeMs = g.getDurationMs();
                outArray[written].reverse = g.getIsReverse();
                outArray[written].active = true;
                ++written;
            }
        }
        return written;
    }
}
