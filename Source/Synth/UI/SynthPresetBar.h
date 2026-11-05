#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace GranularSynthLab
{
    class SynthProcessor;

    class SynthPresetBar : public juce::Component
    {
    public:
        explicit SynthPresetBar(SynthProcessor& processor);
        ~SynthPresetBar() override = default;

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        SynthProcessor& processor;

        juce::ComboBox presetCombo;
        juce::TextButton prevBtn { "<" };
        juce::TextButton nextBtn { ">" };
        juce::TextButton initBtn { "INIT" };

        void loadSelectedPreset();
        void selectPrevPreset();
        void selectNextPreset();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthPresetBar)
    };
}
