#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace GranularSynthLab
{
    class SynthProcessor;

    class SynthADSRPanel : public juce::Component
    {
    public:
        explicit SynthADSRPanel(SynthProcessor& processor);
        ~SynthADSRPanel() override = default;

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        SynthProcessor& processor;

        juce::Slider attackSlider;
        juce::Slider decaySlider;
        juce::Slider sustainSlider;
        juce::Slider releaseSlider;

        juce::Label attackLabel;
        juce::Label decayLabel;
        juce::Label sustainLabel;
        juce::Label releaseLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttach;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthADSRPanel)
    };
}
