#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include "SynthParameters.h"
#include "DSP/SynthEngine.h"
#include "DSP/ModulationEngine.h"
#include "DSP/EffectsEngine.h"
#include "Common/LockFreeQueue.h"

namespace GranularSynthLab
{
    class SynthProcessor : public juce::AudioProcessor
    {
    public:
        SynthProcessor();
        ~SynthProcessor() override;

        void prepareToPlay(double sampleRate, int samplesPerBlock) override;
        void releaseResources() override;
        bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
        void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override { return true; }

        const juce::String getName() const override { return "Granular Synth Lab"; }
        bool acceptsMidi() const override { return true; }
        bool producesMidi() const override { return false; }
        bool isMidiEffect() const override { return false; }
        double getTailLengthSeconds() const override { return 3.0; }

        int getNumPrograms() override { return 1; }
        int getCurrentProgram() override { return 0; }
        void setCurrentProgram(int) override {}
        const juce::String getProgramName(int) override { return {}; }
        void changeProgramName(int, const juce::String&) override {}

        void getStateInformation(juce::MemoryBlock& destData) override;
        void setStateInformation(const void* data, int sizeInBytes) override;

        juce::AudioProcessorValueTreeState& getAPVTS() noexcept { return apvts; }
        SynthEngine& getSynthEngine() noexcept { return synthEngine; }
        juce::MidiKeyboardState& getKeyboardState() noexcept { return keyboardState; }
        juce::AudioFormatManager& getFormatManager() noexcept { return formatManager; }
        GranularLab::SPSCQueue<GranularLab::VisualSnapshot, 8>& getVisualQueue() noexcept { return visualQueue; }

        void loadSampleFile(const juce::File& file);
        void setOscillatorShape(OscShape shape);
        void loadPreset(int presetIndex);

    private:
        juce::AudioProcessorValueTreeState apvts;
        SynthEngine synthEngine;
        GranularLab::ModulationEngine modEngine;
        GranularLab::EffectsEngine fxEngine;

        juce::MidiKeyboardState keyboardState;
        juce::AudioFormatManager formatManager;
        GranularLab::SPSCQueue<GranularLab::VisualSnapshot, 8> visualQueue;

        int visualFrameCounter { 0 };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthProcessor)
    };
}
