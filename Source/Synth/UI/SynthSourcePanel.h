#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../DSP/SampleBuffer.h"

namespace GranularSynthLab
{
    class SynthProcessor;

    class SynthSourcePanel : public juce::Component
    {
    public:
        explicit SynthSourcePanel(SynthProcessor& processor);
        ~SynthSourcePanel() override = default;

        void paint(juce::Graphics& g) override;
        void resized() override;
        void updateState();

    private:
        SynthProcessor& processor;

        juce::TextButton oscModeBtn { "OSCILLATOR" };
        juce::TextButton sampleModeBtn { "SAMPLE" };

        // Osc Shape buttons
        juce::TextButton sawBtn   { "SAW" };
        juce::TextButton sineBtn  { "SINE" };
        juce::TextButton triBtn   { "TRI" };
        juce::TextButton sqrBtn   { "SQR" };
        juce::TextButton noiseBtn { "NOISE" };
        juce::TextButton fmBtn    { "FM BELL" };
        juce::TextButton vocalBtn { "VOCAL" };

        // Sample loader button
        juce::TextButton loadFileBtn { "CARGAR AUDIO..." };
        std::unique_ptr<juce::FileChooser> fileChooser;

        // Root note & Octave
        juce::ComboBox rootNoteBox;
        juce::ComboBox octaveBox;

        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> rootNoteAttach;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> octaveAttach;

        void setOscShape(OscShape shape);
        void openFileBrowser();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthSourcePanel)
    };
}
