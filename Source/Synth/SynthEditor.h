#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "SynthProcessor.h"
#include "UI/SynthPresetBar.h"
#include "UI/SynthSourcePanel.h"
#include "UI/SynthWaveformView.h"
#include "UI/SynthParticleView.h"
#include "UI/SynthADSRPanel.h"
#include "UI/XYPad.h"
#include "UI/EffectsPanel.h"
#include "UI/ModulationPanel.h"
#include "UI/CustomLookAndFeel.h"

namespace GranularSynthLab
{
    class SynthEditor : public juce::AudioProcessorEditor,
                        public juce::Timer
    {
    public:
        explicit SynthEditor(SynthProcessor& processor);
        ~SynthEditor() override;

        void paint(juce::Graphics& g) override;
        void resized() override;
        void timerCallback() override;

    private:
        SynthProcessor& processor;
        GranularLab::CustomLookAndFeel customLookAndFeel;

        // Header
        SynthPresetBar presetBar;

        // Audio Source
        SynthSourcePanel sourcePanel;

        // Waveform & Particles
        SynthWaveformView waveformView;
        SynthParticleView particleView;

        // XY Pad & ADSR
        GranularLab::XYPad xyPad;
        SynthADSRPanel adsrPanel;

        // Granular knobs
        juce::Slider sizeSlider;
        juce::Slider densitySlider;
        juce::Slider spraySlider;
        juce::Slider pitchRandSlider;
        juce::Slider panRandSlider;
        juce::Slider reverseSlider;
        juce::ComboBox windowShapeCombo;

        juce::Label sizeLabel      { {}, "SIZE" };
        juce::Label densityLabel   { {}, "DENSITY" };
        juce::Label sprayLabel     { {}, "SPRAY" };
        juce::Label pitchRandLabel { {}, "PITCH RND" };
        juce::Label panRandLabel   { {}, "PAN SPREAD" };
        juce::Label reverseLabel   { {}, "REVERSE" };
        juce::Label windowLabel    { {}, "WINDOW" };

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sizeAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> densityAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sprayAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchRandAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panRandAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverseAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> windowShapeAttach;

        // Effects & Modulation Tabs
        juce::TextButton btnTabFX { "EFFECTS" };
        juce::TextButton btnTabMod { "MODULATION" };
        GranularLab::EffectsPanel effectsPanel;
        GranularLab::ModulationPanel modulationPanel;

        // Virtual Keyboard
        juce::MidiKeyboardComponent keyboardComponent;

        void setupKnob(juce::Slider& slider, juce::Label& label, const juce::String& suffix);
        void switchTab(bool showFX);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthEditor)
    };
}
