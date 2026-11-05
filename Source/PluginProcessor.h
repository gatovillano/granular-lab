#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Common/Parameters.h"
#include "DSP/GranularEngine.h"
#include "DSP/ModulationEngine.h"
#include "DSP/EffectsEngine.h"

#if HAS_CLAP_JUCE_EXTENSIONS
#include "clap-juce-extensions/clap-juce-extensions.h"
#endif

namespace GranularLab
{
    class GranularLabAudioProcessor : public juce::AudioProcessor
#if HAS_CLAP_JUCE_EXTENSIONS
        , public clap_juce_extensions::clap_extensions
#endif
    {
    public:
        GranularLabAudioProcessor();
        ~GranularLabAudioProcessor() override;

        void prepareToPlay(double sampleRate, int samplesPerBlock) override;
        void releaseResources() override;
        void reset() override;

        bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

        void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override;

        const juce::String getName() const override;

        bool acceptsMidi() const override;
        bool producesMidi() const override;
        bool isMidiEffect() const override;
        double getTailLengthSeconds() const override;

        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram(int index) override;
        const juce::String getProgramName(int index) override;
        void changeProgramName(int index, const juce::String& newName) override;

        void getStateInformation(juce::MemoryBlock& destData) override;
        void setStateInformation(const void* data, int sizeInBytes) override;

        juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
        GranularEngine& getGranularEngine() { return granularEngine; }

    private:
        juce::AudioProcessorValueTreeState apvts;

        GranularEngine granularEngine;
        ModulationEngine modulationEngine;
        EffectsEngine effectsEngine;

        // Atomic parameter pointers for lock-free audio thread reads
        std::atomic<float>* pGrainSize { nullptr };
        std::atomic<float>* pDensity { nullptr };
        std::atomic<float>* pPosition { nullptr };
        std::atomic<float>* pSpray { nullptr };
        std::atomic<float>* pPitch { nullptr };
        std::atomic<float>* pPitchRand { nullptr };
        std::atomic<float>* pPan { nullptr };
        std::atomic<float>* pPanRand { nullptr };
        std::atomic<float>* pReverseProb { nullptr };
        std::atomic<float>* pFreeze { nullptr };
        std::atomic<float>* pFeedback { nullptr };
        std::atomic<float>* pEnvelopeShape { nullptr };
        std::atomic<float>* pMix { nullptr };
        std::atomic<float>* pOutputGain { nullptr };

        std::atomic<float>* pFilterEnabled { nullptr };
        std::atomic<float>* pFilterType { nullptr };
        std::atomic<float>* pFilterCutoff { nullptr };
        std::atomic<float>* pFilterResonance { nullptr };

        std::atomic<float>* pDelayEnabled { nullptr };
        std::atomic<float>* pDelayTime { nullptr };
        std::atomic<float>* pDelayFeedback { nullptr };
        std::atomic<float>* pDelayMix { nullptr };

        std::atomic<float>* pReverbEnabled { nullptr };
        std::atomic<float>* pReverbSize { nullptr };
        std::atomic<float>* pReverbDamping { nullptr };
        std::atomic<float>* pReverbMix { nullptr };

        std::atomic<float>* pSatEnabled { nullptr };
        std::atomic<float>* pSatDrive { nullptr };
        std::atomic<float>* pSatMix { nullptr };

        std::atomic<float>* pLfo1Rate { nullptr };
        std::atomic<float>* pLfo1Depth { nullptr };
        std::atomic<float>* pLfo1Target { nullptr };
        std::atomic<float>* pLfo1Shape { nullptr };

        std::atomic<float>* pLfo2Rate { nullptr };
        std::atomic<float>* pLfo2Depth { nullptr };
        std::atomic<float>* pLfo2Target { nullptr };
        std::atomic<float>* pLfo2Shape { nullptr };

        // XY Macro Multi-Target Pointers
        std::atomic<float>* pX { nullptr };
        std::atomic<float>* pY { nullptr };
        std::array<std::atomic<float>*, 4> pXTargets { nullptr, nullptr, nullptr, nullptr };
        std::array<std::atomic<float>*, 4> pXDepths { nullptr, nullptr, nullptr, nullptr };
        std::array<std::atomic<float>*, 4> pYTargets { nullptr, nullptr, nullptr, nullptr };
        std::array<std::atomic<float>*, 4> pYDepths { nullptr, nullptr, nullptr, nullptr };

        juce::AudioBuffer<float> wetBuffer;

        void cacheParameterPointers();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularLabAudioProcessor)
    };
}
