#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Presets/PresetData.h"
#include "CustomLookAndFeel.h"

namespace GranularLab
{
    class PresetBar : public juce::Component
    {
    public:
        PresetBar(juce::AudioProcessorValueTreeState& apvts);
        ~PresetBar() override = default;

        void paint(juce::Graphics& g) override;
        void resized() override;

        void loadPreset(int index);

    private:
        juce::AudioProcessorValueTreeState& apvts;
        std::vector<Preset> presets;

        juce::ComboBox presetCombo;
        juce::TextButton btnPrev { "<" };
        juce::TextButton btnNext { ">" };
        juce::TextButton btnInit { "INIT" };

        void loadPresetInternal(const Preset& p);
    };
}
