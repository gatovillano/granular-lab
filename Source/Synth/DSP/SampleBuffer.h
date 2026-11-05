#pragma once

#include <vector>
#include <string>
#include <atomic>
#include <memory>
#include <cmath>
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>

namespace GranularSynthLab
{
    enum class OscShape
    {
        Supersaw = 0,
        Sine,
        Triangle,
        Square,
        Noise,
        FMBell,
        VocalFormant,
        NumShapes
    };

    enum class SoundSourceMode
    {
        Oscillator = 0,
        Sample
    };

    struct SampleData
    {
        std::vector<float> left;
        std::vector<float> right;
        int lengthSamples { 0 };
        double sampleRate { 44100.0 };
        std::string name { "Supersaw" };
        bool isOscillator { true };
        OscShape shape { OscShape::Supersaw };

        // 512-point min/max display peaks
        std::vector<float> minPeaksL;
        std::vector<float> maxPeaksL;
        std::vector<float> minPeaksR;
        std::vector<float> maxPeaksR;
    };

    class SampleBuffer
    {
    public:
        SampleBuffer();
        ~SampleBuffer() = default;

        void prepare(double sampleRate);

        // Load an external audio file (called from UI / background thread)
        bool loadFromFile(const juce::File& file, juce::AudioFormatManager& formatManager);

        // Generate built-in synthesis waveforms (called on init or when shape changes)
        void generateOscillator(OscShape shape);

        // Real-time audio thread Hermite interpolation read
        float readHermite(int channel, double position) const noexcept;

        // Peak downsampling for Waveform UI
        void getWaveformSnapshot(std::vector<float>& outMinL, std::vector<float>& outMaxL,
                                 std::vector<float>& outMinR, std::vector<float>& outMaxR) const;

        int getLengthSamples() const noexcept;
        double getBufferSampleRate() const noexcept;
        std::string getSourceTitle() const;
        bool isSampleLoaded() const noexcept;
        SoundSourceMode getSourceMode() const noexcept { return currentMode.load(std::memory_order_relaxed); }
        void setSourceMode(SoundSourceMode mode) noexcept { currentMode.store(mode, std::memory_order_relaxed); }
        OscShape getCurrentOscShape() const noexcept { return currentOscShape.load(std::memory_order_relaxed); }

    private:
        double currentHostSampleRate { 44100.0 };
        std::atomic<SoundSourceMode> currentMode { SoundSourceMode::Oscillator };
        std::atomic<OscShape> currentOscShape { OscShape::Supersaw };

        // Double buffer for thread-safe audio swaps without blocking or allocating on audio thread
        std::shared_ptr<SampleData> currentData;
        std::atomic<std::shared_ptr<SampleData>> activeData;

        void computePeaks(SampleData& data);
    };
}
