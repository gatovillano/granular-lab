#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../DSP/AudioBuffer.h"
#include "CustomLookAndFeel.h"

namespace GranularLab
{
    class WaveformView : public juce::Component, public juce::Timer
    {
    public:
        WaveformView(const CircularAudioBuffer& buf, juce::AudioProcessorValueTreeState& apvts);
        ~WaveformView() override;

        void paint(juce::Graphics& g) override;
        void timerCallback() override;

    private:
        const CircularAudioBuffer& buffer;
        juce::AudioProcessorValueTreeState& apvts;

        std::vector<float> minL, maxL, minR, maxR;
        float currentWritePos { 0.0f };
        bool isFrozen { false };
    };
}
