#pragma once

#include <vector>
#include <atomic>
#include <cmath>
#include <algorithm>
#include <juce_core/juce_core.h>

namespace GranularLab
{
    class CircularAudioBuffer
    {
    public:
        CircularAudioBuffer() = default;
        ~CircularAudioBuffer() = default;

        void prepare(double sampleRate, double maxSeconds = 10.0);
        void reset();

        void write(float sampleL, float sampleR, float feedbackAmount = 0.0f, float feedbackL = 0.0f, float feedbackR = 0.0f) noexcept;

        float readHermite(int channel, double position) const noexcept;
        float readLinear(int channel, double position) const noexcept;

        void setFrozen(bool frozen) noexcept { isFrozen.store(frozen, std::memory_order_relaxed); }
        bool getFrozen() const noexcept { return isFrozen.load(std::memory_order_relaxed); }

        int getBufferSize() const noexcept { return bufferSize; }
        int getWriteIndex() const noexcept { return writeIndex; }
        double getSampleRate() const noexcept { return currentSampleRate; }

        float getNormalizedWritePos() const noexcept
        {
            if (bufferSize > 0)
                return static_cast<float>(writeIndex) / static_cast<float>(bufferSize);
            return 0.0f;
        }

        // GUI Waveform visualization interface
        static constexpr int WAVEFORM_POINTS = 512;
        void getWaveformSnapshot(std::vector<float>& outMinL, std::vector<float>& outMaxL,
                                std::vector<float>& outMinR, std::vector<float>& outMaxR) const;

    private:
        std::vector<float> bufferL;
        std::vector<float> bufferR;
        int bufferSize { 0 };
        int writeIndex { 0 };
        double currentSampleRate { 44100.0 };
        std::atomic<bool> isFrozen { false };

        // Waveform downsampling
        mutable std::vector<float> waveMinL;
        mutable std::vector<float> waveMaxL;
        mutable std::vector<float> waveMinR;
        mutable std::vector<float> waveMaxR;
        int downsampleCounter { 0 };
        int downsampleFactor { 256 };
        float currentBlockMinL { 0.0f };
        float currentBlockMaxL { 0.0f };
        float currentBlockMinR { 0.0f };
        float currentBlockMaxR { 0.0f };

        inline int wrapIndex(int idx) const noexcept
        {
            while (idx < 0) idx += bufferSize;
            while (idx >= bufferSize) idx -= bufferSize;
            return idx;
        }
    };
}
