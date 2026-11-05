#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/CustomLookAndFeel.h"
#include "UI/PresetBar.h"
#include "UI/XYPad.h"
#include "UI/WaveformView.h"
#include "UI/ParticleView.h"
#include "UI/EffectsPanel.h"
#include "UI/ModulationPanel.h"

namespace GranularLab
{
    class GranularLabAudioProcessorEditor : public juce::AudioProcessorEditor
    {
    public:
        explicit GranularLabAudioProcessorEditor(GranularLabAudioProcessor&);
        ~GranularLabAudioProcessorEditor() override;

        void paint(juce::Graphics&) override;
        void resized() override;

    private:
        GranularLabAudioProcessor& processorRef;
        CustomLookAndFeel customLookAndFeel;

        // Header
        PresetBar presetBar;

        // Visual / Interactive Core
        XYPad xyPad;
        WaveformView waveformView;
        ParticleView particleView;

        // Main Granular Knobs Strip
        struct KnobWithLabel
        {
            juce::Slider slider;
            juce::Label label;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
        };

        KnobWithLabel knobGrainSize;
        KnobWithLabel knobDensity;
        KnobWithLabel knobPosition;
        KnobWithLabel knobSpray;
        KnobWithLabel knobPitch;
        KnobWithLabel knobPitchRand;
        KnobWithLabel knobPan;
        KnobWithLabel knobPanRand;
        KnobWithLabel knobReverse;
        KnobWithLabel knobFeedback;
        KnobWithLabel knobMix;
        KnobWithLabel knobOutput;

        juce::ToggleButton btnFreeze { "FREEZE" };
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> freezeAttach;

        juce::ComboBox comboWindow;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> windowAttach;

        // Bottom Deck (Effects & Modulation)
        juce::TextButton tabEffectsBtn { "EFFECTS (FILTER / DELAY / REVERB / DRIVE)" };
        juce::TextButton tabModBtn { "MODULATION (LFO 1 / LFO 2)" };
        bool showingEffectsTab { true };

        EffectsPanel effectsPanel;
        ModulationPanel modulationPanel;

        void setupKnob(KnobWithLabel& k, const juce::String& paramId, const juce::String& labelText);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularLabAudioProcessorEditor)
    };
}
