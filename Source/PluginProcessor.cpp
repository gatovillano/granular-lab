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

        // XY Macro Multi-Target Pointers
        pX = apvts.getRawParameterValue(ParamIDs::xyX);
        pY = apvts.getRawParameterValue(ParamIDs::xyY);

        pXTargets[0] = apvts.getRawParameterValue(ParamIDs::xyXTarget1);
        pXDepths[0]  = apvts.getRawParameterValue(ParamIDs::xyXDepth1);
        pXTargets[1] = apvts.getRawParameterValue(ParamIDs::xyXTarget2);
        pXDepths[1]  = apvts.getRawParameterValue(ParamIDs::xyXDepth2);
        pXTargets[2] = apvts.getRawParameterValue(ParamIDs::xyXTarget3);
        pXDepths[2]  = apvts.getRawParameterValue(ParamIDs::xyXDepth3);
        pXTargets[3] = apvts.getRawParameterValue(ParamIDs::xyXTarget4);
        pXDepths[3]  = apvts.getRawParameterValue(ParamIDs::xyXDepth4);

        pYTargets[0] = apvts.getRawParameterValue(ParamIDs::xyYTarget1);
        pYDepths[0]  = apvts.getRawParameterValue(ParamIDs::xyYDepth1);
        pYTargets[1] = apvts.getRawParameterValue(ParamIDs::xyYTarget2);
        pYDepths[1]  = apvts.getRawParameterValue(ParamIDs::xyYDepth2);
        pYTargets[2] = apvts.getRawParameterValue(ParamIDs::xyYTarget3);
        pYDepths[2]  = apvts.getRawParameterValue(ParamIDs::xyYDepth3);
        pYTargets[3] = apvts.getRawParameterValue(ParamIDs::xyYTarget4);
        pYDepths[3]  = apvts.getRawParameterValue(ParamIDs::xyYDepth4);
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

        // 2. Accumulate XY Macro Multi-Target Modulations (Output Portal style)
        float macroX = pX ? pX->load(std::memory_order_relaxed) : 0.5f;
        float macroY = pY ? pY->load(std::memory_order_relaxed) : 0.5f;

        float xyMod[static_cast<int>(XYTarget::NumTargets)] = { 0.0f };

        for (int s = 0; s < 4; ++s)
        {
            if (pXTargets[s] && pXDepths[s])
            {
                int targetX = static_cast<int>(pXTargets[s]->load(std::memory_order_relaxed));
                float depthX = pXDepths[s]->load(std::memory_order_relaxed);
                if (targetX > 0 && targetX < static_cast<int>(XYTarget::NumTargets))
                    xyMod[targetX] += macroX * depthX;
            }

            if (pYTargets[s] && pYDepths[s])
            {
                int targetY = static_cast<int>(pYTargets[s]->load(std::memory_order_relaxed));
                float depthY = pYDepths[s]->load(std::memory_order_relaxed);
                if (targetY > 0 && targetY < static_cast<int>(XYTarget::NumTargets))
                    xyMod[targetY] += macroY * depthY;
            }
        }

        // Prepare Granular Engine Parameters with modulation applied
        float baseGrainSize = pGrainSize ? pGrainSize->load() : 80.0f;
        float baseDensity   = pDensity ? pDensity->load() : 25.0f;
        float basePos       = pPosition ? pPosition->load() : 0.5f;
        float baseSpray     = pSpray ? pSpray->load() : 0.15f;
        float basePitch     = pPitch ? pPitch->load() : 0.0f;
        float basePitchRand = pPitchRand ? pPitchRand->load() : 0.0f;
        float basePan       = pPan ? pPan->load() : 0.0f;
        float basePanRand   = pPanRand ? pPanRand->load() : 0.4f;
        float baseReverse   = pReverseProb ? pReverseProb->load() : 0.0f;

        GranularEngine::Parameters gParams;
        gParams.grainSizeMs = std::clamp(baseGrainSize + mod.grainSizeMod + xyMod[static_cast<int>(XYTarget::GrainSize)] * 490.0f, 10.0f, 500.0f);
        gParams.densityHz = std::clamp(baseDensity + mod.densityMod + xyMod[static_cast<int>(XYTarget::Density)] * 99.0f, 1.0f, 100.0f);
        gParams.position = std::clamp(basePos + mod.positionMod + xyMod[static_cast<int>(XYTarget::Position)], 0.0f, 1.0f);
        gParams.spray = std::clamp(baseSpray + xyMod[static_cast<int>(XYTarget::Spray)], 0.0f, 1.0f);
        gParams.pitchSemitones = std::clamp(basePitch + mod.pitchMod + xyMod[static_cast<int>(XYTarget::Pitch)] * 24.0f, -24.0f, 24.0f);
        gParams.pitchRandSemitones = basePitchRand;
        gParams.pan = std::clamp(basePan + mod.panMod + xyMod[static_cast<int>(XYTarget::Pan)], -1.0f, 1.0f);
        gParams.panRand = basePanRand;
        gParams.reverseProb = std::clamp(baseReverse + xyMod[static_cast<int>(XYTarget::ReverseProb)], 0.0f, 1.0f);
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
        float baseFilterCutoff = pFilterCutoff ? pFilterCutoff->load() : 12000.0f;
        float baseFilterReso   = pFilterResonance ? pFilterResonance->load() : 0.707f;
        float baseDelayTime    = pDelayTime ? pDelayTime->load() : 320.0f;
        float baseDelayFb      = pDelayFeedback ? pDelayFeedback->load() : 0.35f;
        float baseDelayMix     = pDelayMix ? pDelayMix->load() : 0.3f;
        float baseReverbSize   = pReverbSize ? pReverbSize->load() : 0.65f;
        float baseReverbMix    = pReverbMix ? pReverbMix->load() : 0.35f;
        float baseSatDrive     = pSatDrive ? pSatDrive->load() : 1.5f;

        EffectsEngine::Parameters fxParams;
        fxParams.filterEnabled = pFilterEnabled ? (pFilterEnabled->load() > 0.5f) : true;
        fxParams.filterType = static_cast<FilterType>(pFilterType ? static_cast<int>(pFilterType->load()) : 0);
        fxParams.filterCutoff = std::clamp(baseFilterCutoff * std::pow(2.0f, (mod.cutoffMod / 2400.0f) + xyMod[static_cast<int>(XYTarget::Cutoff)] * 4.0f), 20.0f, 20000.0f);
        fxParams.filterResonance = std::clamp(baseFilterReso + xyMod[static_cast<int>(XYTarget::Resonance)] * 5.0f, 0.1f, 10.0f);

        fxParams.delayEnabled = pDelayEnabled ? (pDelayEnabled->load() > 0.5f) : false;
        fxParams.delayTimeMs = std::clamp(baseDelayTime + xyMod[static_cast<int>(XYTarget::DelayTime)] * 1000.0f, 10.0f, 2000.0f);
        fxParams.delayFeedback = std::clamp(baseDelayFb + xyMod[static_cast<int>(XYTarget::DelayFeedback)] * 0.5f, 0.0f, 0.95f);
        fxParams.delayMix = std::clamp(baseDelayMix + xyMod[static_cast<int>(XYTarget::DelayMix)], 0.0f, 1.0f);

        fxParams.reverbEnabled = pReverbEnabled ? (pReverbEnabled->load() > 0.5f) : false;
        fxParams.reverbSize = std::clamp(baseReverbSize + xyMod[static_cast<int>(XYTarget::ReverbSize)] * 0.5f, 0.0f, 1.0f);
        fxParams.reverbDamping = pReverbDamping ? pReverbDamping->load() : 0.4f;
        fxParams.reverbMix = std::clamp(baseReverbMix + xyMod[static_cast<int>(XYTarget::ReverbMix)], 0.0f, 1.0f);

        fxParams.satEnabled = pSatEnabled ? (pSatEnabled->load() > 0.5f) : false;
        fxParams.satDrive = std::clamp(baseSatDrive + xyMod[static_cast<int>(XYTarget::SatDrive)] * 5.0f, 1.0f, 10.0f);
        fxParams.satMix = pSatMix ? pSatMix->load() : 0.5f;

        effectsEngine.processBlock(wetPointers, numSamples, fxParams);

        // Dry/Wet Mix and Master Gain
        float baseMix = pMix ? pMix->load() : 0.5f;
        float mix = std::clamp(baseMix + xyMod[static_cast<int>(XYTarget::Mix)], 0.0f, 1.0f);
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
