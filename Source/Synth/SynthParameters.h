#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Common/Parameters.h"

namespace GranularSynthLab
{
    namespace ParamIDs
    {
        // Synth specific parameters
        inline constexpr auto sourceMode     = "source_mode";
        inline constexpr auto oscShape       = "osc_shape";
        inline constexpr auto rootNote       = "root_note";
        inline constexpr auto octave         = "octave";
        inline constexpr auto fineTune       = "fine_tune";
        inline constexpr auto pitchBendRange = "pitch_bend_range";

        inline constexpr auto attack         = "amp_attack";
        inline constexpr auto decay          = "amp_decay";
        inline constexpr auto sustain        = "amp_sustain";
        inline constexpr auto release        = "amp_release";

        // Shared parameters with GranularLab
        inline constexpr auto grainSize       = GranularLab::ParamIDs::grainSize;
        inline constexpr auto density         = GranularLab::ParamIDs::density;
        inline constexpr auto position        = GranularLab::ParamIDs::position;
        inline constexpr auto spray           = GranularLab::ParamIDs::spray;
        inline constexpr auto pitch           = GranularLab::ParamIDs::pitch;
        inline constexpr auto pitchRand       = GranularLab::ParamIDs::pitchRand;
        inline constexpr auto pan             = GranularLab::ParamIDs::pan;
        inline constexpr auto panRand         = GranularLab::ParamIDs::panRand;
        inline constexpr auto reverseProb     = GranularLab::ParamIDs::reverseProb;
        inline constexpr auto envelopeShape   = GranularLab::ParamIDs::envelopeShape;

        inline constexpr auto filterEnabled   = GranularLab::ParamIDs::filterEnabled;
        inline constexpr auto filterType      = GranularLab::ParamIDs::filterType;
        inline constexpr auto filterCutoff    = GranularLab::ParamIDs::filterCutoff;
        inline constexpr auto filterResonance = GranularLab::ParamIDs::filterResonance;

        inline constexpr auto delayEnabled    = GranularLab::ParamIDs::delayEnabled;
        inline constexpr auto delayTime       = GranularLab::ParamIDs::delayTime;
        inline constexpr auto delayFeedback   = GranularLab::ParamIDs::delayFeedback;
        inline constexpr auto delayMix        = GranularLab::ParamIDs::delayMix;

        inline constexpr auto reverbEnabled   = GranularLab::ParamIDs::reverbEnabled;
        inline constexpr auto reverbSize      = GranularLab::ParamIDs::reverbSize;
        inline constexpr auto reverbDamping   = GranularLab::ParamIDs::reverbDamping;
        inline constexpr auto reverbMix       = GranularLab::ParamIDs::reverbMix;

        inline constexpr auto satEnabled      = GranularLab::ParamIDs::satEnabled;
        inline constexpr auto satDrive        = GranularLab::ParamIDs::satDrive;
        inline constexpr auto satMix          = GranularLab::ParamIDs::satMix;

        inline constexpr auto lfo1Rate        = GranularLab::ParamIDs::lfo1Rate;
        inline constexpr auto lfo1Depth       = GranularLab::ParamIDs::lfo1Depth;
        inline constexpr auto lfo1Target      = GranularLab::ParamIDs::lfo1Target;
        inline constexpr auto lfo1Shape       = GranularLab::ParamIDs::lfo1Shape;

        inline constexpr auto lfo2Rate        = GranularLab::ParamIDs::lfo2Rate;
        inline constexpr auto lfo2Depth       = GranularLab::ParamIDs::lfo2Depth;
        inline constexpr auto lfo2Target      = GranularLab::ParamIDs::lfo2Target;
        inline constexpr auto lfo2Shape       = GranularLab::ParamIDs::lfo2Shape;

        inline constexpr auto xyX             = GranularLab::ParamIDs::xyX;
        inline constexpr auto xyY             = GranularLab::ParamIDs::xyY;
        inline constexpr auto xyXTarget       = GranularLab::ParamIDs::xyXTarget;
        inline constexpr auto xyYTarget       = GranularLab::ParamIDs::xyYTarget;

        inline constexpr auto xyXTarget1      = GranularLab::ParamIDs::xyXTarget1;
        inline constexpr auto xyXDepth1       = GranularLab::ParamIDs::xyXDepth1;
        inline constexpr auto xyXTarget2      = GranularLab::ParamIDs::xyXTarget2;
        inline constexpr auto xyXDepth2       = GranularLab::ParamIDs::xyXDepth2;
        inline constexpr auto xyXTarget3      = GranularLab::ParamIDs::xyXTarget3;
        inline constexpr auto xyXDepth3       = GranularLab::ParamIDs::xyXDepth3;
        inline constexpr auto xyXTarget4      = GranularLab::ParamIDs::xyXTarget4;
        inline constexpr auto xyXDepth4       = GranularLab::ParamIDs::xyXDepth4;

        inline constexpr auto xyYTarget1      = GranularLab::ParamIDs::xyYTarget1;
        inline constexpr auto xyYDepth1       = GranularLab::ParamIDs::xyYDepth1;
        inline constexpr auto xyYTarget2      = GranularLab::ParamIDs::xyYTarget2;
        inline constexpr auto xyYDepth2       = GranularLab::ParamIDs::xyYDepth2;
        inline constexpr auto xyYTarget3      = GranularLab::ParamIDs::xyYTarget3;
        inline constexpr auto xyYDepth3       = GranularLab::ParamIDs::xyYDepth3;
        inline constexpr auto xyYTarget4      = GranularLab::ParamIDs::xyYTarget4;
        inline constexpr auto xyYDepth4       = GranularLab::ParamIDs::xyYDepth4;

        inline constexpr auto outputGain      = GranularLab::ParamIDs::outputGain;
    }

    inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        // Source & Tuning
        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::sourceMode, "Source Mode", juce::StringArray { "Oscillator", "Sample" }, 0));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::oscShape, "Osc Shape",
            juce::StringArray { "Supersaw", "Sine", "Triangle", "Square", "Pink Noise", "FM Crystal", "Vocal Choir" }, 0));

        params.push_back(std::make_unique<juce::AudioParameterInt>(
            ParamIDs::rootNote, "Root Note", 36, 84, 60)); // C3 = 60

        params.push_back(std::make_unique<juce::AudioParameterInt>(
            ParamIDs::octave, "Octave", -2, 2, 0));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::fineTune, "Fine Tune", -100.0f, 100.0f, 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterInt>(
            ParamIDs::pitchBendRange, "Pitch Bend Range", 1, 24, 2));

        // ADSR Amplitude Envelope
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::attack, "Attack",
            juce::NormalisableRange<float>(1.0f, 5000.0f, 0.1f, 0.35f), 15.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::decay, "Decay",
            juce::NormalisableRange<float>(10.0f, 5000.0f, 0.1f, 0.35f), 350.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::sustain, "Sustain",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::release, "Release",
            juce::NormalisableRange<float>(10.0f, 8000.0f, 0.1f, 0.35f), 600.0f));

        // Granular Engine
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::grainSize, "Grain Size",
            juce::NormalisableRange<float>(10.0f, 500.0f, 0.1f, 0.4f), 80.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::density, "Density",
            juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f, 0.5f), 24.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::position, "Position",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.25f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::spray, "Spray",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.15f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::pitch, "Pitch Shift",
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::pitchRand, "Pitch Random",
            juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::pan, "Pan",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::panRand, "Pan Spread",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.4f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::reverseProb, "Reverse Chance",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::envelopeShape, "Window Shape",
            juce::StringArray { "Hann", "Gaussian", "Tukey", "Triangle" }, 0));

        // Filter
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            ParamIDs::filterEnabled, "Filter Enable", true));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::filterType, "Filter Type",
            juce::StringArray { "LowPass", "HighPass", "BandPass" }, 0));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::filterCutoff, "Filter Cutoff",
            juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.25f), 12000.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::filterResonance, "Filter Res",
            juce::NormalisableRange<float>(0.1f, 10.0f, 0.05f, 0.5f), 0.707f));

        // Delay
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            ParamIDs::delayEnabled, "Delay Enable", true));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::delayTime, "Delay Time",
            juce::NormalisableRange<float>(10.0f, 2000.0f, 1.0f, 0.35f), 250.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::delayFeedback, "Delay Feedback",
            juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f), 0.35f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::delayMix, "Delay Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.2f));

        // Reverb
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            ParamIDs::reverbEnabled, "Reverb Enable", true));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::reverbSize, "Reverb Size",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.6f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::reverbDamping, "Reverb Damp",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.4f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::reverbMix, "Reverb Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));

        // Saturation
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            ParamIDs::satEnabled, "Sat Enable", true));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::satDrive, "Drive",
            juce::NormalisableRange<float>(1.0f, 10.0f, 0.05f), 1.5f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::satMix, "Sat Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));

        // Output Gain
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::outputGain, "Output Gain",
            juce::NormalisableRange<float>(-36.0f, 12.0f, 0.1f), 0.0f));

        // XY Pad Multi-Target
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::xyX, "XY Pad X", 0.0f, 1.0f, 0.5f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::xyY, "XY Pad Y", 0.0f, 1.0f, 0.5f));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::xyXTarget, "XY X Target",
            GranularLab::getXYTargetNames(), 1));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::xyYTarget, "XY Y Target",
            GranularLab::getXYTargetNames(), 2));

        // Macro X Slots (1 to 4)
        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::xyXTarget1, "Macro X Target 1", GranularLab::getXYTargetNames(), 1));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::xyXDepth1, "Macro X Depth 1",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 1.0f));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::xyXTarget2, "Macro X Target 2", GranularLab::getXYTargetNames(), 0));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::xyXDepth2, "Macro X Depth 2",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::xyXTarget3, "Macro X Target 3", GranularLab::getXYTargetNames(), 0));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::xyXDepth3, "Macro X Depth 3",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::xyXTarget4, "Macro X Target 4", GranularLab::getXYTargetNames(), 0));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::xyXDepth4, "Macro X Depth 4",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));

        // Macro Y Slots (1 to 4)
        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::xyYTarget1, "Macro Y Target 1", GranularLab::getXYTargetNames(), 2));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::xyYDepth1, "Macro Y Depth 1",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 1.0f));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::xyYTarget2, "Macro Y Target 2", GranularLab::getXYTargetNames(), 0));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::xyYDepth2, "Macro Y Depth 2",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::xyYTarget3, "Macro Y Target 3", GranularLab::getXYTargetNames(), 0));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::xyYDepth3, "Macro Y Depth 3",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::xyYTarget4, "Macro Y Target 4", GranularLab::getXYTargetNames(), 0));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::xyYDepth4, "Macro Y Depth 4",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));

        // Modulation LFO 1 & 2
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::lfo1Rate, "LFO 1 Rate",
            juce::NormalisableRange<float>(0.05f, 20.0f, 0.01f, 0.35f), 1.0f));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::lfo1Shape, "LFO 1 Shape",
            juce::StringArray { "Sine", "Triangle", "Saw", "S&H" }, 0));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::lfo1Depth, "LFO 1 Depth", 0.0f, 1.0f, 0.2f));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::lfo1Target, "LFO 1 Target",
            juce::StringArray { "None", "Position", "Size", "Density", "Pitch", "Pan", "Cutoff" }, 1));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::lfo2Rate, "LFO 2 Rate",
            juce::NormalisableRange<float>(0.05f, 20.0f, 0.01f, 0.35f), 0.25f));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::lfo2Shape, "LFO 2 Shape",
            juce::StringArray { "Sine", "Triangle", "Saw", "S&H" }, 1));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            ParamIDs::lfo2Depth, "LFO 2 Depth", 0.0f, 1.0f, 0.15f));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            ParamIDs::lfo2Target, "LFO 2 Target",
            juce::StringArray { "None", "Position", "Size", "Density", "Pitch", "Pan", "Cutoff" }, 6));

        return { params.begin(), params.end() };
    }
}
