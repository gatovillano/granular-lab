#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "CustomLookAndFeel.h"
#include "../Common/Parameters.h"

namespace GranularLab
{
    class EffectsPanel : public juce::Component
    {
    public:
        EffectsPanel(juce::AudioProcessorValueTreeState& apvts);
        ~EffectsPanel() override = default;

        void paint(juce::Graphics& g) override;
        void resized() override;
        void updateMacroModulations(const float* xyMod);

    private:
        juce::AudioProcessorValueTreeState& apvts;

        // Filter Section
        juce::ToggleButton filterToggle { "FILTER" };
        juce::ComboBox filterTypeCombo;
        juce::Slider filterCutoffSlider;
        juce::Slider filterResoSlider;
        juce::Label filterCutoffLabel { {}, "Cutoff" };
        juce::Label filterResoLabel { {}, "Reso" };

        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> filterToggleAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResoAttach;

        // Delay Section
        juce::ToggleButton delayToggle { "DELAY" };
        juce::Slider delayTimeSlider;
        juce::Slider delayFeedbackSlider;
        juce::Slider delayMixSlider;
        juce::Label delayTimeLabel { {}, "Time" };
        juce::Label delayFeedbackLabel { {}, "Feedbk" };
        juce::Label delayMixLabel { {}, "Mix" };

        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> delayToggleAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayFeedbackAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayMixAttach;

        // Reverb Section
        juce::ToggleButton reverbToggle { "REVERB" };
        juce::Slider reverbSizeSlider;
        juce::Slider reverbDampingSlider;
        juce::Slider reverbMixSlider;
        juce::Label reverbSizeLabel { {}, "Size" };
        juce::Label reverbDampingLabel { {}, "Damp" };
        juce::Label reverbMixLabel { {}, "Mix" };

        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reverbToggleAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbSizeAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbDampingAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbMixAttach;

        // Saturation Section
        juce::ToggleButton satToggle { "DRIVE" };
        juce::Slider satDriveSlider;
        juce::Slider satMixSlider;
        juce::Label satDriveLabel { {}, "Drive" };
        juce::Label satMixLabel { {}, "Mix" };

        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> satToggleAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> satDriveAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> satMixAttach;

        void setupRotary(juce::Slider& slider, juce::Label& label);
    };
}
