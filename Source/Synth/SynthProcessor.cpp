#include "SynthProcessor.h"
#include "SynthEditor.h"
#include "Presets/SynthPresetData.h"

namespace GranularSynthLab
{
    SynthProcessor::SynthProcessor()
        : AudioProcessor(BusesProperties()
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          apvts(*this, nullptr, "Parameters", createParameterLayout())
    {
        formatManager.registerBasicFormats();
    }

    SynthProcessor::~SynthProcessor()
    {
    }

    void SynthProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        synthEngine.prepare(sampleRate, samplesPerBlock);
        modEngine.prepare(sampleRate);
        fxEngine.prepare(sampleRate, samplesPerBlock);
        visualFrameCounter = 0;
    }

    void SynthProcessor::releaseResources()
    {
        synthEngine.reset();
        fxEngine.reset();
    }

    bool SynthProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
    {
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

        return true;
    }

    void SynthProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
    {
        juce::ScopedNoDenormals noDenormals;

        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();

        // 1. Process Virtual Keyboard MIDI
        keyboardState.processNextMidiBuffer(midiMessages, 0, numSamples, true);

        // 2. Clear buffer before synthesizing
        for (int ch = 0; ch < numChannels; ++ch)
            buffer.clear(ch, 0, numSamples);

        if (numSamples == 0)
            return;

        // 3. Read parameters from APVTS
        const int rootNoteVal       = static_cast<int>(apvts.getRawParameterValue(ParamIDs::rootNote)->load());
        const int octaveVal         = static_cast<int>(apvts.getRawParameterValue(ParamIDs::octave)->load());
        const float fineTuneVal     = apvts.getRawParameterValue(ParamIDs::fineTune)->load();
        const float pitchBendRange  = apvts.getRawParameterValue(ParamIDs::pitchBendRange)->load();

        float baseSize              = apvts.getRawParameterValue(ParamIDs::grainSize)->load();
        float baseDensity           = apvts.getRawParameterValue(ParamIDs::density)->load();
        float basePos               = apvts.getRawParameterValue(ParamIDs::position)->load();
        float baseSpray             = apvts.getRawParameterValue(ParamIDs::spray)->load();
        float basePitchRand         = apvts.getRawParameterValue(ParamIDs::pitchRand)->load();
        float basePanRand           = apvts.getRawParameterValue(ParamIDs::panRand)->load();
        float baseReverse           = apvts.getRawParameterValue(ParamIDs::reverseProb)->load();
        const int windowShapeVal    = static_cast<int>(apvts.getRawParameterValue(ParamIDs::envelopeShape)->load());

        const float attackVal       = apvts.getRawParameterValue(ParamIDs::attack)->load();
        const float decayVal        = apvts.getRawParameterValue(ParamIDs::decay)->load();
        const float sustainVal      = apvts.getRawParameterValue(ParamIDs::sustain)->load();
        const float releaseVal      = apvts.getRawParameterValue(ParamIDs::release)->load();

        bool filterEnabledVal       = apvts.getRawParameterValue(ParamIDs::filterEnabled)->load() > 0.5f;
        float filterCutoffVal       = apvts.getRawParameterValue(ParamIDs::filterCutoff)->load();
        const float filterResVal    = apvts.getRawParameterValue(ParamIDs::filterResonance)->load();
        const int filterTypeVal     = static_cast<int>(apvts.getRawParameterValue(ParamIDs::filterType)->load());

        bool delayEnabledVal        = apvts.getRawParameterValue(ParamIDs::delayEnabled)->load() > 0.5f;
        const float delayTimeVal    = apvts.getRawParameterValue(ParamIDs::delayTime)->load();
        const float delayFbVal      = apvts.getRawParameterValue(ParamIDs::delayFeedback)->load();
        const float delayMixVal     = apvts.getRawParameterValue(ParamIDs::delayMix)->load();

        bool revEnabledVal          = apvts.getRawParameterValue(ParamIDs::reverbEnabled)->load() > 0.5f;
        const float revSizeVal      = apvts.getRawParameterValue(ParamIDs::reverbSize)->load();
        const float revDampVal      = apvts.getRawParameterValue(ParamIDs::reverbDamping)->load();
        const float revMixVal       = apvts.getRawParameterValue(ParamIDs::reverbMix)->load();

        bool satEnabledVal          = apvts.getRawParameterValue(ParamIDs::satEnabled)->load() > 0.5f;
        const float driveVal        = apvts.getRawParameterValue(ParamIDs::satDrive)->load();
        const float satMixVal       = apvts.getRawParameterValue(ParamIDs::satMix)->load();
        const float masterGainDb    = apvts.getRawParameterValue(ParamIDs::outputGain)->load();
        const float masterGainVal   = juce::Decibels::decibelsToGain(masterGainDb);

        const float padXVal         = apvts.getRawParameterValue(ParamIDs::xyX)->load();
        const float padYVal         = apvts.getRawParameterValue(ParamIDs::xyY)->load();
        const int padAssignXVal     = static_cast<int>(apvts.getRawParameterValue(ParamIDs::xyXTarget)->load());
        const int padAssignYVal     = static_cast<int>(apvts.getRawParameterValue(ParamIDs::xyYTarget)->load());

        // 4. Modulation LFOs
        GranularLab::ModulationEngine::LFOConfig lfo1Cfg;
        lfo1Cfg.rateHz = apvts.getRawParameterValue(ParamIDs::lfo1Rate)->load();
        lfo1Cfg.shape  = static_cast<GranularLab::LFOShape>(static_cast<int>(apvts.getRawParameterValue(ParamIDs::lfo1Shape)->load()));
        lfo1Cfg.depth  = apvts.getRawParameterValue(ParamIDs::lfo1Depth)->load();
        lfo1Cfg.target = static_cast<GranularLab::ModTarget>(static_cast<int>(apvts.getRawParameterValue(ParamIDs::lfo1Target)->load()));

        GranularLab::ModulationEngine::LFOConfig lfo2Cfg;
        lfo2Cfg.rateHz = apvts.getRawParameterValue(ParamIDs::lfo2Rate)->load();
        lfo2Cfg.shape  = static_cast<GranularLab::LFOShape>(static_cast<int>(apvts.getRawParameterValue(ParamIDs::lfo2Shape)->load()));
        lfo2Cfg.depth  = apvts.getRawParameterValue(ParamIDs::lfo2Depth)->load();
        lfo2Cfg.target = static_cast<GranularLab::ModTarget>(static_cast<int>(apvts.getRawParameterValue(ParamIDs::lfo2Target)->load()));

        auto mod = modEngine.process(lfo1Cfg, lfo2Cfg);

        // Accumulate XY Macro Multi-Target Modulations (Output Portal style)
        float xyMod[static_cast<int>(GranularLab::XYTarget::NumTargets)] = { 0.0f };

        const auto* pTargetX1 = apvts.getRawParameterValue(ParamIDs::xyXTarget1);
        const auto* pDepthX1  = apvts.getRawParameterValue(ParamIDs::xyXDepth1);
        const auto* pTargetX2 = apvts.getRawParameterValue(ParamIDs::xyXTarget2);
        const auto* pDepthX2  = apvts.getRawParameterValue(ParamIDs::xyXDepth2);
        const auto* pTargetX3 = apvts.getRawParameterValue(ParamIDs::xyXTarget3);
        const auto* pDepthX3  = apvts.getRawParameterValue(ParamIDs::xyXDepth3);
        const auto* pTargetX4 = apvts.getRawParameterValue(ParamIDs::xyXTarget4);
        const auto* pDepthX4  = apvts.getRawParameterValue(ParamIDs::xyXDepth4);

        const auto* pTargetY1 = apvts.getRawParameterValue(ParamIDs::xyYTarget1);
        const auto* pDepthY1  = apvts.getRawParameterValue(ParamIDs::xyYDepth1);
        const auto* pTargetY2 = apvts.getRawParameterValue(ParamIDs::xyYTarget2);
        const auto* pDepthY2  = apvts.getRawParameterValue(ParamIDs::xyYDepth2);
        const auto* pTargetY3 = apvts.getRawParameterValue(ParamIDs::xyYTarget3);
        const auto* pDepthY3  = apvts.getRawParameterValue(ParamIDs::xyYDepth3);
        const auto* pTargetY4 = apvts.getRawParameterValue(ParamIDs::xyYTarget4);
        const auto* pDepthY4  = apvts.getRawParameterValue(ParamIDs::xyYDepth4);

        auto addMod = [&](const std::atomic<float>* tPtr, const std::atomic<float>* dPtr, float macroVal)
        {
            if (tPtr && dPtr)
            {
                int t = static_cast<int>(tPtr->load(std::memory_order_relaxed));
                float d = dPtr->load(std::memory_order_relaxed);
                if (t > 0 && t < static_cast<int>(GranularLab::XYTarget::NumTargets))
                    xyMod[t] += macroVal * d;
            }
        };

        addMod(pTargetX1, pDepthX1, padXVal);
        addMod(pTargetX2, pDepthX2, padXVal);
        addMod(pTargetX3, pDepthX3, padXVal);
        addMod(pTargetX4, pDepthX4, padXVal);

        addMod(pTargetY1, pDepthY1, padYVal);
        addMod(pTargetY2, pDepthY2, padYVal);
        addMod(pTargetY3, pDepthY3, padYVal);
        addMod(pTargetY4, pDepthY4, padYVal);

        // Apply Modulation offsets + XY Macro offsets
        basePos         = std::clamp(basePos + mod.positionMod + xyMod[static_cast<int>(GranularLab::XYTarget::Position)], 0.0f, 1.0f);
        baseSize        = std::clamp(baseSize + mod.grainSizeMod + xyMod[static_cast<int>(GranularLab::XYTarget::GrainSize)] * 490.0f, 10.0f, 500.0f);
        baseDensity     = std::clamp(baseDensity + mod.densityMod + xyMod[static_cast<int>(GranularLab::XYTarget::Density)] * 99.0f, 1.0f, 100.0f);
        basePitchRand   = std::clamp(basePitchRand + mod.pitchMod + xyMod[static_cast<int>(GranularLab::XYTarget::Pitch)] * 24.0f, 0.0f, 24.0f);
        basePanRand     = std::clamp(basePanRand + mod.panMod + xyMod[static_cast<int>(GranularLab::XYTarget::Pan)], 0.0f, 1.0f);
        baseSpray       = std::clamp(baseSpray + xyMod[static_cast<int>(GranularLab::XYTarget::Spray)], 0.0f, 1.0f);
        baseReverse     = std::clamp(baseReverse + xyMod[static_cast<int>(GranularLab::XYTarget::ReverseProb)], 0.0f, 1.0f);

        filterCutoffVal = std::clamp(filterCutoffVal * std::pow(2.0f, (mod.cutoffMod / 2400.0f) + xyMod[static_cast<int>(GranularLab::XYTarget::Cutoff)] * 4.0f), 20.0f, 20000.0f);

        // 5. Build Voice Parameters
        VoiceParams voiceParams;
        voiceParams.sampleRate = getSampleRate();
        voiceParams.rootNote = rootNoteVal;
        voiceParams.octave = octaveVal;
        voiceParams.fineTune = fineTuneVal;
        voiceParams.pitchBendSemis = pitchBendRange;
        voiceParams.position = basePos;
        voiceParams.spray = baseSpray;
        voiceParams.grainSizeMs = baseSize;
        voiceParams.densityHz = baseDensity;
        voiceParams.pitchRandom = basePitchRand;
        voiceParams.panRandom = basePanRand;
        voiceParams.reverseChance = baseReverse;
        voiceParams.windowShape = static_cast<GranularLab::EnvelopeShape>(windowShapeVal);

        voiceParams.adsrParams.attack  = attackVal * 0.001f;
        voiceParams.adsrParams.decay   = decayVal * 0.001f;
        voiceParams.adsrParams.sustain = sustainVal;
        voiceParams.adsrParams.release = releaseVal * 0.001f;

        // 6. Process MIDI and Render Voices
        synthEngine.processMidi(midiMessages, voiceParams);

        float* outL = buffer.getWritePointer(0);
        float* outR = (numChannels > 1) ? buffer.getWritePointer(1) : outL;
        synthEngine.renderVoices(outL, outR, numSamples, voiceParams);

        // 7. Process FX Engine (Filter, Delay, Reverb, Saturation)
        GranularLab::EffectsEngine::Parameters fxParams;
        fxParams.filterType = static_cast<GranularLab::FilterType>(filterTypeVal);
        fxParams.filterCutoff = filterCutoffVal;
        fxParams.filterResonance = filterResVal;
        fxParams.filterEnabled = filterEnabledVal;

        fxParams.delayTimeMs = delayTimeVal;
        fxParams.delayFeedback = delayFbVal;
        fxParams.delayMix = delayMixVal;
        fxParams.delayEnabled = delayEnabledVal;

        fxParams.reverbSize = revSizeVal;
        fxParams.reverbDamping = revDampVal;
        fxParams.reverbMix = revMixVal;
        fxParams.reverbEnabled = revEnabledVal;

        fxParams.satDrive = driveVal;
        fxParams.satMix = satMixVal;
        fxParams.satEnabled = satEnabledVal;

        float* const outPointers[2] = { outL, outR };
        fxEngine.processBlock(outPointers, numSamples, fxParams);

        // 8. Master Gain & Soft limiter
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                float x = channelData[i] * masterGainVal;
                // Soft clipping curve
                if (x > 1.2f) x = 1.0f;
                else if (x < -1.2f) x = -1.0f;
                else x = std::tanh(x);
                channelData[i] = x;
            }
        }

        // 9. Emit Visual Snapshot (~30 fps)
        visualFrameCounter += numSamples;
        if (visualFrameCounter >= static_cast<int>(getSampleRate() / 30.0))
        {
            visualFrameCounter = 0;
            synthEngine.pushVisualSnapshot(visualQueue);
        }
    }

    void SynthProcessor::loadSampleFile(const juce::File& file)
    {
        if (synthEngine.getSampleBuffer().loadFromFile(file, formatManager))
        {
            if (auto* param = apvts.getParameter(ParamIDs::sourceMode))
                param->setValueNotifyingHost(1.0f); // 1 = Sample
        }
    }

    void SynthProcessor::setOscillatorShape(OscShape shape)
    {
        synthEngine.getSampleBuffer().generateOscillator(shape);
        if (auto* param = apvts.getParameter(ParamIDs::sourceMode))
            param->setValueNotifyingHost(0.0f); // 0 = Oscillator
        if (auto* param = apvts.getParameter(ParamIDs::oscShape))
            param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(shape)));
    }

    void SynthProcessor::loadPreset(int presetIndex)
    {
        const auto& presets = getFactorySynthPresets();
        if (presetIndex < 0 || presetIndex >= static_cast<int>(presets.size()))
            return;

        const auto& p = presets[static_cast<size_t>(presetIndex)];

        auto setP = [&](const juce::String& id, float val)
        {
            if (auto* param = apvts.getParameter(id))
                param->setValueNotifyingHost(param->convertTo0to1(val));
        };

        setP(ParamIDs::sourceMode, static_cast<float>(p.sourceMode));
        setP(ParamIDs::oscShape, static_cast<float>(p.oscShape));
        setP(ParamIDs::rootNote, static_cast<float>(p.rootNote));
        setP(ParamIDs::octave, static_cast<float>(p.octave));
        setP(ParamIDs::fineTune, p.fineTune);

        setP(ParamIDs::grainSize, p.grainSize);
        setP(ParamIDs::density, p.density);
        setP(ParamIDs::position, p.position);
        setP(ParamIDs::spray, p.spray);
        setP(ParamIDs::pitchRand, p.pitchRand);
        setP(ParamIDs::panRand, p.panRand);
        setP(ParamIDs::reverseProb, p.reverse);
        setP(ParamIDs::envelopeShape, static_cast<float>(p.windowShape));

        setP(ParamIDs::attack, p.attack);
        setP(ParamIDs::decay, p.decay);
        setP(ParamIDs::sustain, p.sustain);
        setP(ParamIDs::release, p.release);

        setP(ParamIDs::filterType, static_cast<float>(p.filterType));
        setP(ParamIDs::filterCutoff, p.filterCutoff);
        setP(ParamIDs::filterResonance, p.filterRes);

        setP(ParamIDs::delayTime, p.delayTime);
        setP(ParamIDs::delayFeedback, p.delayFeedback);
        setP(ParamIDs::delayMix, p.delayMix);

        setP(ParamIDs::reverbSize, p.reverbSize);
        setP(ParamIDs::reverbDamping, p.reverbDamp);
        setP(ParamIDs::reverbMix, p.reverbMix);

        setP(ParamIDs::satDrive, p.drive);
        setP(ParamIDs::outputGain, 0.0f);

        setP(ParamIDs::xyX, p.padX);
        setP(ParamIDs::xyY, p.padY);

        if (p.sourceMode == 0)
        {
            synthEngine.getSampleBuffer().generateOscillator(static_cast<OscShape>(p.oscShape));
        }
    }

    void SynthProcessor::getStateInformation(juce::MemoryBlock& destData)
    {
        auto state = apvts.copyState();
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, destData);
    }

    void SynthProcessor::setStateInformation(const void* data, int sizeInBytes)
    {
        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
        if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
            // Ensure oscillator buffer is initialized
            int oscShapeVal = static_cast<int>(apvts.getRawParameterValue(ParamIDs::oscShape)->load());
            int srcModeVal  = static_cast<int>(apvts.getRawParameterValue(ParamIDs::sourceMode)->load());
            if (srcModeVal == 0)
            {
                synthEngine.getSampleBuffer().generateOscillator(static_cast<OscShape>(oscShapeVal));
            }
        }
    }

    juce::AudioProcessorEditor* SynthProcessor::createEditor()
    {
        return new SynthEditor(*this);
    }
}

// Juce plugin entry point for GranularSynthLab
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GranularSynthLab::SynthProcessor();
}
