#pragma once

#include "AudioBuffer.h"
#include "Grain.h"
#include "../Common/Parameters.h"
#include "../Common/LockFreeQueue.h"
#include <array>
#include <random>

namespace GranularLab
{
    class GranularEngine
    {
    public:
        static constexpr size_t MAX_GRAINS = 128;

        GranularEngine();
        ~GranularEngine() = default;

        void prepare(double sampleRate, int maxBlockSize);
        void reset();

        struct Parameters
        {
            float grainSizeMs { 80.0f };
            float densityHz { 25.0f };
            float position { 0.5f };
            float spray { 0.15f };
            float pitchSemitones { 0.0f };
            float pitchRandSemitones { 0.0f };
            float pan { 0.0f };
            float panRand { 0.4f };
            float reverseProb { 0.0f };
            bool freeze { false };
            float feedback { 0.0f };
            EnvelopeShape envelope { EnvelopeShape::Hann };
        };

        void processBlock(const float* const* inputChannels,
                          float* const* outputChannels,
                          int numSamples,
                          const Parameters& params) noexcept;

        CircularAudioBuffer& getAudioBuffer() noexcept { return circularBuffer; }
        const CircularAudioBuffer& getAudioBuffer() const noexcept { return circularBuffer; }

        bool popVisualSnapshot(VisualSnapshot& snapshot) noexcept
        {
            return visualQueue.latest(snapshot);
        }

    private:
        CircularAudioBuffer circularBuffer;
        std::array<Grain, MAX_GRAINS> grains;
        SPSCQueue<VisualSnapshot, 8> visualQueue;

        double currentSampleRate { 44100.0 };
        double spawnAccumulator { 0.0 };
        int visualCounter { 0 };

        // Fast simple PRNG for audio thread
        uint32_t rngState { 133742 };
        inline float randomFloat() noexcept // returns 0.0f to 1.0f
        {
            rngState ^= rngState << 13;
            rngState ^= rngState >> 17;
            rngState ^= rngState << 5;
            return static_cast<float>(rngState & 0x7FFFFFFF) / 2147483648.0f;
        }

        inline float randomBipolar() noexcept // returns -1.0f to 1.0f
        {
            return randomFloat() * 2.0f - 1.0f;
        }

        void spawnGrain(const Parameters& params) noexcept;
        void emitVisualSnapshot() noexcept;
    };
}
