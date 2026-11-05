#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace GranularLab
{
    GranularLabAudioProcessor::GranularLabAudioProcessor()
        : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          apvts(*this, nullptr, "Parameters", createParameterLayout())
    {
        cacheParameterPointers();
    }

    GranularLabAudioProcessor::~GranularLabAudioProcessor() = default;

    void GranularLabAudioProcessor::cacheParameterPointers()
    {
        pGrainSize      = apvts.getRawParameterValue(ParamIDs::grainSize);
        pDensity        = apvts.getRawParameterValue(ParamIDs::density);
        pPosition       = apvts.getRawParameterValue(ParamIDs::position);
        pSpray          = apvts.getRawParameterValue(ParamIDs::spray);
        pPitch          = apvts.getRawParameterValue(ParamIDs::pitch);
        pPitchRand      = apvts.getRawParameterValue(ParamIDs::pitchRand);
        pPan            = apvts.getRawParameterValue(ParamIDs::pan);
        pPanRand        = apvts.getRawParameterValue(ParamIDs::panRand);
        pReverseProb    = apvts.getRawParameterValue(ParamIDs::reverseProb);
        pFreeze         = apvts.getRawParameterValue(ParamIDs::freeze);
        pFeedback       = apvts.getRawParameterValue(ParamIDs::feedback);
        pEnvelopeShape  = apvts.getRawParameterValue(ParamIDs::envelopeShape);
        pMix            = apvts.getRawParameterValue(ParamIDs::mix);
        pOutputGain     = apvts.getRawParameterValue(ParamIDs::outputGain);

        pFilterEnabled  = apvts.getRawParameterValue(ParamIDs::filterEnabled);
        pFilterType     = apvts.getRawParameterValue(ParamIDs::filterType);
        pFilterCutoff   = apvts.getRawParameterValue(ParamIDs::filterCutoff);
        pFilterResonance= apvts.getRawParameterValue(ParamIDs::filterResonance);

        pDelayEnabled   = apvts.getRawParameterValue(ParamIDs::delayEnabled);
        pDelayTime      = apvts.getRawParameterValue(ParamIDs::delayTime);
        pDelayFeedback  = apvts.getRawParameterValue(ParamIDs::delayFeedback);
        pDelayMix       = apvts.getRawParameterValue(ParamIDs::delayMix);

        pReverbEnabled  = apvts.getRawParameterValue(ParamIDs::reverbEnabled);
        pReverbSize     = apvts.getRawParameterValue(ParamIDs::reverbSize);
        pReverbDamping  = apvts.getRawParameterValue(ParamIDs::reverbDamping);
        pReverbMix      = apvts.getRawParameterValue(ParamIDs::reverbMix);

        pSatEnabled     = apvts.getRawParameterValue(ParamIDs::satEnabled);
        pSatDrive       = apvts.getRawParameterValue(ParamIDs::satDrive);
        pSatMix         = apvts.getRawParameterValue(ParamIDs::satMix);

        pLfo1Rate       = apvts.getRawParameterValue(ParamIDs::lfo1Rate);
        pLfo1Depth      = apvts.getRawParameterValue(ParamIDs::lfo1Depth);
        pLfo1Target     = apvts.getRawParameterValue(ParamIDs::lfo1Target);
        pLfo1Shape      = apvts.getRawParameterValue(ParamIDs::lfo1Shape);

        pLfo2Rate       = apvts.getRawParameterValue(ParamIDs::lfo2Rate);
        pLfo2Depth      = apvts.getRawParameterValue(ParamIDs::lfo2Depth);
        pLfo2Target     = apvts.getRawParameterValue(ParamIDs::lfo2Target);
        pLfo2Shape      = apvts.getRawParameterValue(ParamIDs::lfo2Shape);
    }

    const juce::String GranularLabAudioProcessor::getName() const
    {
        return JucePlugin_Name;
    }

    bool GranularLabAudioProcessor::acceptsMidi() const { return false; }
    bool GranularLabAudioProcessor::producesMidi() const { return false; }
    bool GranularLabAudioProcessor::isMidiEffect() const { return false; }
    double GranularLabAudioProcessor::getTailLengthSeconds() const { return 2.0; }

    int GranularLabAudioProcessor::getNumPrograms() { return 1; }
    int GranularLabAudioProcessor::getCurrentProgram() { return 0; }
    void GranularLabAudioProcessor::setCurrentProgram(int index) { juce::ignoreUnused(index); }
    const juce::String GranularLabAudioProcessor::getProgramName(int index)
    {
        juce::ignoreUnused(index);
        return {};
    }
    void GranularLabAudioProcessor::changeProgramName(int index, const juce::String& newName)
    {
        juce::ignoreUnused(index, newName);
    }

    void GranularLabAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        wetBuffer.setSize(2, samplesPerBlock);
        granularEngine.prepare(sampleRate, samplesPerBlock);
        modulationEngine.prepare(sampleRate);
        effectsEngine.prepare(sampleRate, samplesPerBlock);
    }

    void GranularLabAudioProcessor::releaseResources()
    {
        granularEngine.reset();
        modulationEngine.reset();
        effectsEngine.reset();
    }

    void GranularLabAudioProcessor::reset()
    {
        granularEngine.reset();
        modulationEngine.reset();
        effectsEngine.reset();
    }

    bool GranularLabAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
    {
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

        if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo()
            && layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono())
            return false;

        return true;
    }

    void GranularLabAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
    {
        juce::ignoreUnused(midiMessages);
        juce::ScopedNoDenormals noDenormals;

        const int totalNumInputChannels = getTotalNumInputChannels();
        const int totalNumOutputChannels = getTotalNumOutputChannels();
        const int numSamples = buffer.getNumSamples();

        if (numSamples == 0) return;

        // Clear excess output channels
        for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear(i, 0, numSamples);

        // Preallocate / ensure wet buffer size
        if (wetBuffer.getNumSamples() < numSamples)
            wetBuffer.setSize(2, numSamples, false, false, true);

        // Prepare modulation
        ModulationEngine::LFOConfig lfo1Cfg;
        lfo1Cfg.rateHz = pLfo1Rate ? pLfo1Rate->load() : 1.0f;
        lfo1Cfg.depth = pLfo1Depth ? pLfo1Depth->load() : 0.0f;
        lfo1Cfg.target = static_cast<ModTarget>(pLfo1Target ? static_cast<int>(pLfo1Target->load()) : 0);
        lfo1Cfg.shape = static_cast<LFOShape>(pLfo1Shape ? static_cast<int>(pLfo1Shape->load()) : 0);

        ModulationEngine::LFOConfig lfo2Cfg;
        lfo2Cfg.rateHz = pLfo2Rate ? pLfo2Rate->load() : 1.0f;
        lfo2Cfg.depth = pLfo2Depth ? pLfo2Depth->load() : 0.0f;
        lfo2Cfg.target = static_cast<ModTarget>(pLfo2Target ? static_cast<int>(pLfo2Target->load()) : 0);
        lfo2Cfg.shape = static_cast<LFOShape>(pLfo2Shape ? static_cast<int>(pLfo2Shape->load()) : 0);

        auto mod = modulationEngine.process(lfo1Cfg, lfo2Cfg);

        // Prepare Granular Engine Parameters with modulation applied
        GranularEngine::Parameters gParams;
        gParams.grainSizeMs = std::clamp((pGrainSize ? pGrainSize->load() : 80.0f) + mod.grainSizeMod, 10.0f, 500.0f);
        gParams.densityHz = std::clamp((pDensity ? pDensity->load() : 25.0f) + mod.densityMod, 1.0f, 100.0f);
        gParams.position = std::clamp((pPosition ? pPosition->load() : 0.5f) + mod.positionMod, 0.0f, 1.0f);
        gParams.spray = pSpray ? pSpray->load() : 0.15f;
        gParams.pitchSemitones = std::clamp((pPitch ? pPitch->load() : 0.0f) + mod.pitchMod, -24.0f, 24.0f);
        gParams.pitchRandSemitones = pPitchRand ? pPitchRand->load() : 0.0f;
        gParams.pan = std::clamp((pPan ? pPan->load() : 0.0f) + mod.panMod, -1.0f, 1.0f);
        gParams.panRand = pPanRand ? pPanRand->load() : 0.4f;
        gParams.reverseProb = pReverseProb ? pReverseProb->load() : 0.0f;
        gParams.freeze = pFreeze ? (pFreeze->load() > 0.5f) : false;
        gParams.feedback = pFeedback ? pFeedback->load() : 0.0f;
        gParams.envelope = static_cast<EnvelopeShape>(pEnvelopeShape ? static_cast<int>(pEnvelopeShape->load()) : 0);

        // Channel setup
        const float* inL = buffer.getReadPointer(0);
        const float* inR = (totalNumInputChannels > 1) ? buffer.getReadPointer(1) : inL;
        const float* const inputPointers[2] = { inL, inR };

        float* wetL = wetBuffer.getWritePointer(0);
        float* wetR = wetBuffer.getWritePointer(1);
        float* const wetPointers[2] = { wetL, wetR };

        // Process granular engine
        granularEngine.processBlock(inputPointers, wetPointers, numSamples, gParams);

        // Process Effects Engine on the wet path
        EffectsEngine::Parameters fxParams;
        fxParams.filterEnabled = pFilterEnabled ? (pFilterEnabled->load() > 0.5f) : true;
        fxParams.filterType = static_cast<FilterType>(pFilterType ? static_cast<int>(pFilterType->load()) : 0);
        fxParams.filterCutoff = std::clamp((pFilterCutoff ? pFilterCutoff->load() : 12000.0f) + mod.cutoffMod, 20.0f, 20000.0f);
        fxParams.filterResonance = pFilterResonance ? pFilterResonance->load() : 0.707f;

        fxParams.delayEnabled = pDelayEnabled ? (pDelayEnabled->load() > 0.5f) : false;
        fxParams.delayTimeMs = pDelayTime ? pDelayTime->load() : 320.0f;
        fxParams.delayFeedback = pDelayFeedback ? pDelayFeedback->load() : 0.35f;
        fxParams.delayMix = pDelayMix ? pDelayMix->load() : 0.3f;

        fxParams.reverbEnabled = pReverbEnabled ? (pReverbEnabled->load() > 0.5f) : false;
        fxParams.reverbSize = pReverbSize ? pReverbSize->load() : 0.65f;
        fxParams.reverbDamping = pReverbDamping ? pReverbDamping->load() : 0.4f;
        fxParams.reverbMix = pReverbMix ? pReverbMix->load() : 0.35f;

        fxParams.satEnabled = pSatEnabled ? (pSatEnabled->load() > 0.5f) : false;
        fxParams.satDrive = pSatDrive ? pSatDrive->load() : 1.5f;
        fxParams.satMix = pSatMix ? pSatMix->load() : 0.5f;

        effectsEngine.processBlock(wetPointers, numSamples, fxParams);

        // Dry/Wet Mix and Master Gain
        float mix = pMix ? pMix->load() : 0.5f;
        float gainDb = pOutputGain ? pOutputGain->load() : 0.0f;
        float gainLinear = juce::Decibels::decibelsToGain(gainDb);

        float* outL = buffer.getWritePointer(0);
        float* outR = (totalNumOutputChannels > 1) ? buffer.getWritePointer(1) : outL;

        for (int i = 0; i < numSamples; ++i)
        {
            float drySampleL = inL[i];
            float drySampleR = inR[i];
            float wetSampleL = wetL[i];
            float wetSampleR = wetR[i];

            float mixedL = (drySampleL * (1.0f - mix) + wetSampleL * mix) * gainLinear;
            float mixedR = (drySampleR * (1.0f - mix) + wetSampleR * mix) * gainLinear;

            // Protection against NaN/Inf and soft limiting
            if (!std::isfinite(mixedL)) mixedL = 0.0f;
            if (!std::isfinite(mixedR)) mixedR = 0.0f;

            // Soft saturation ceiling at +/- 1.0 to protect ears & monitors
            outL[i] = std::tanh(mixedL);
            if (totalNumOutputChannels > 1)
                outR[i] = std::tanh(mixedR);
        }
    }

    bool GranularLabAudioProcessor::hasEditor() const { return true; }

    juce::AudioProcessorEditor* GranularLabAudioProcessor::createEditor()
    {
        return new GranularLabAudioProcessorEditor(*this);
    }

    void GranularLabAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
    {
        auto state = apvts.copyState();
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, destData);
    }

    void GranularLabAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
    {
        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
        if (xmlState.get() != nullptr && xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

// Juce entry point
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GranularLab::GranularLabAudioProcessor();
}
