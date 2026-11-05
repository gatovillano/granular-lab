#pragma once

#include "SampleBuffer.h"
#include "SynthVoice.h"
#include "Common/LockFreeQueue.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <vector>

namespace GranularSynthLab
{
    class SynthEngine
    {
    public:
        static constexpr size_t MAX_VOICES = 16;

        SynthEngine();
        ~SynthEngine() = default;

        void prepare(double sampleRate, int maxBlockSize);
        void reset();

        void processMidi(const juce::MidiBuffer& midiMessages, const VoiceParams& params);
        void renderVoices(float* outL, float* outR, int numSamples, const VoiceParams& params) noexcept;

        SampleBuffer& getSampleBuffer() noexcept { return sampleBuffer; }
        const SampleBuffer& getSampleBuffer() const noexcept { return sampleBuffer; }

        int getActiveVoiceCount() const noexcept;
        void pushVisualSnapshot(GranularLab::SPSCQueue<GranularLab::VisualSnapshot, 8>& visualQueue);

    private:
        std::array<SynthVoice, MAX_VOICES> voices;
        SampleBuffer sampleBuffer;

        double currentSampleRate { 44100.0 };
        float currentPitchBendSemis { 0.0f };
        bool sustainPedal { false };
        std::vector<int> sustainedNotes;

        // Temporary voice buffer
        std::vector<float> voiceBufferL;
        std::vector<float> voiceBufferR;

        SynthVoice* findFreeVoice();
        SynthVoice* findVoicePlayingNote(int midiNote);
    };
}
