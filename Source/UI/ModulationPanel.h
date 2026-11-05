#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "CustomLookAndFeel.h"
#include "../Common/Parameters.h"

namespace GranularLab
{
    class ModulationPanel : public juce::Component
    {
    public:
        ModulationPanel(juce::AudioProcessorValueTreeState& apvts);
        ~ModulationPanel() override = default;

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        juce::AudioProcessorValueTreeState& apvts;

        // LFO 1
        juce::Slider lfo1RateSlider;
        juce::Slider lfo1DepthSlider;
        juce::ComboBox lfo1ShapeCombo;
        juce::ComboBox lfo1TargetCombo;
        juce::Label lfo1RateLabel { {}, "Rate" };
        juce::Label lfo1DepthLabel { {}, "Depth" };

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfo1RateAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfo1DepthAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfo1ShapeAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfo1TargetAttach;

        // LFO 2
        juce::Slider lfo2RateSlider;
        juce::Slider lfo2DepthSlider;
        juce::ComboBox lfo2ShapeCombo;
        juce::ComboBox lfo2TargetCombo;
        juce::Label lfo2RateLabel { {}, "Rate" };
        juce::Label lfo2DepthLabel { {}, "Depth" };

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfo2RateAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfo2DepthAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfo2ShapeAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfo2TargetAttach;

        void setupLFOControls(int lfoNum);
    };
}
