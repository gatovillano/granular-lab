#pragma once

#include "SampleBuffer.h"
#include "SynthGrain.h"
#include "Common/LockFreeQueue.h"
#include <array>
#include <juce_audio_basics/juce_audio_basics.h>

namespace GranularSynthLab
{
    struct VoiceParams
    {
        double sampleRate { 44100.0 };
        int rootNote { 60 };
        int octave { 0 };
        float fineTune { 0.0f };
        float pitchBendSemis { 0.0f };

        float position { 0.25f };
        float spray { 0.15f };
        float grainSizeMs { 80.0f };
        float densityHz { 24.0f };
        float pitchRandom { 0.0f };
        float panRandom { 0.4f };
        float reverseChance { 0.0f };
        GranularLab::EnvelopeShape windowShape { GranularLab::EnvelopeShape::Hann };

        juce::ADSR::Parameters adsrParams;
    };

    class SynthVoice
    {
    public:
        SynthVoice();

        void prepare(double sampleRate);
        void noteOn(int midiNote, float velocity, const VoiceParams& params);
        void noteOff(float velocity, bool allowTailOff);
        void pitchWheel(float pitchBendSemitones);

        void renderNextBlock(const SampleBuffer& buffer, float* outL, float* outR,
                             int numSamples, const VoiceParams& params) noexcept;

        bool isActive() const noexcept { return isVoiceActive; }
        int getMidiNote() const noexcept { return currentMidiNote; }
        uint32_t getAge() const noexcept { return noteAge; }
        void kill() noexcept;

        // Fill visual info array for active grains in this voice
        size_t collectVisualGrains(GranularLab::GrainVisualInfo* outArray, size_t maxCount) const noexcept;

    private:
        static constexpr size_t MAX_GRAINS_PER_VOICE = 16;
        std::array<SynthGrain, MAX_GRAINS_PER_VOICE> grains;

        juce::ADSR ampAdsr;
        bool isVoiceActive { false };
        int currentMidiNote { -1 };
        float currentVelocity { 0.0f };
        float currentPitchBend { 0.0f };
        double notePitchRatio { 1.0 };
        double grainSpawnAccumulator { 0.0 };
        double currentSampleRate { 44100.0 };
        uint32_t noteAge { 0 };
        uint32_t seed { 54321 };

        float fastRand() noexcept;
        void spawnGrain(const SampleBuffer& buffer, const VoiceParams& params) noexcept;
    };
}
