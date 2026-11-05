#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace GranularLab
{
    namespace ParamIDs
    {
        // Granular Engine Parameters
        inline constexpr auto grainSize       = "grain_size";       // ms (10 to 500)
        inline constexpr auto density         = "density";          // Hz / grains per sec (1 to 100)
        inline constexpr auto position        = "position";         // normalized (0.0 to 1.0)
        inline constexpr auto spray           = "spray";            // position randomization (0.0 to 1.0)
        inline constexpr auto pitch           = "pitch";            // semitones (-24.0 to +24.0)
        inline constexpr auto pitchRand       = "pitch_rand";       // semitones range (0.0 to 24.0)
        inline constexpr auto pan             = "pan";              // stereo center (-1.0 to 1.0)
        inline constexpr auto panRand         = "pan_rand";         // pan randomization (0.0 to 1.0)
        inline constexpr auto reverseProb     = "reverse_prob";     // probability (0.0 to 1.0)
        inline constexpr auto freeze          = "freeze";           // toggle (0 or 1)
        inline constexpr auto feedback        = "feedback";         // feedback into buffer (0.0 to 0.95)
        inline constexpr auto envelopeShape   = "envelope_shape";   // 0: Hann, 1: Gaussian, 2: Tukey, 3: Triangle
        inline constexpr auto mix             = "mix";              // Dry/Wet (0.0 to 1.0)
        inline constexpr auto outputGain      = "output_gain";      // dB (-36.0 to +12.0)

        // Effects Engine Parameters
        inline constexpr auto filterEnabled   = "filter_enabled";   // bool
        inline constexpr auto filterType      = "filter_type";      // 0: LowPass, 1: HighPass, 2: BandPass
        inline constexpr auto filterCutoff    = "filter_cutoff";    // Hz (20 to 20000)
        inline constexpr auto filterResonance = "filter_resonance"; // Q (0.1 to 10.0)

        inline constexpr auto delayEnabled    = "delay_enabled";    // bool
        inline constexpr auto delayTime       = "delay_time";       // ms (10 to 2000)
        inline constexpr auto delayFeedback   = "delay_feedback";   // (0.0 to 0.95)
        inline constexpr auto delayMix        = "delay_mix";        // (0.0 to 1.0)

        inline constexpr auto reverbEnabled   = "reverb_enabled";   // bool
        inline constexpr auto reverbSize      = "reverb_size";      // (0.0 to 1.0)
        inline constexpr auto reverbDamping   = "reverb_damping";   // (0.0 to 1.0)
        inline constexpr auto reverbMix       = "reverb_mix";       // (0.0 to 1.0)

        inline constexpr auto satEnabled      = "sat_enabled";      // bool
        inline constexpr auto satDrive        = "sat_drive";        // (1.0 to 10.0)
        inline constexpr auto satMix          = "sat_mix";          // (0.0 to 1.0)

        // Modulation Parameters
        inline constexpr auto lfo1Rate        = "lfo1_rate";        // Hz (0.05 to 20.0)
        inline constexpr auto lfo1Depth       = "lfo1_depth";       // (0.0 to 1.0)
        inline constexpr auto lfo1Target      = "lfo1_target";      // 0: None, 1: Position, 2: Size, 3: Density, 4: Pitch, 5: Pan, 6: Cutoff
        inline constexpr auto lfo1Shape       = "lfo1_shape";       // 0: Sine, 1: Tri, 2: Saw, 3: S&H

        inline constexpr auto lfo2Rate        = "lfo2_rate";        // Hz (0.05 to 20.0)
        inline constexpr auto lfo2Depth       = "lfo2_depth";       // (0.0 to 1.0)
        inline constexpr auto lfo2Target      = "lfo2_target";      // 0: None, 1: Position, 2: Size, 3: Density, 4: Pitch, 5: Pan, 6: Cutoff
        inline constexpr auto lfo2Shape       = "lfo2_shape";       // 0: Sine, 1: Tri, 2: Saw, 3: S&H

        // XY Pad Parameters
        inline constexpr auto xyX             = "xy_x";             // 0.0 to 1.0
        inline constexpr auto xyY             = "xy_y";             // 0.0 to 1.0
        inline constexpr auto xyXTarget       = "xy_x_target";      // 0: Density, 1: Size, 2: Position, 3: Spray, 4: Pitch, 5: Cutoff, 6: Mix
        inline constexpr auto xyYTarget       = "xy_y_target";      // 0: Density, 1: Size, 2: Position, 3: Spray, 4: Pitch, 5: Cutoff, 6: Mix
    }

    enum class ModTarget : int
    {
        None = 0,
        Position = 1,
        GrainSize = 2,
        Density = 3,
        Pitch = 4,
        Pan = 5,
        Cutoff = 6,
        NumTargets
    };

    enum class XYTarget : int
    {
        Density = 0,
        GrainSize = 1,
        Position = 2,
        Spray = 3,
        Pitch = 4,
        Cutoff = 5,
        Mix = 6,
        NumTargets
    };

    enum class EnvelopeShape : int
    {
        Hann = 0,
        Gaussian = 1,
        Tukey = 2,
        Triangle = 3,
        NumShapes
    };

    enum class FilterType : int
    {
        LowPass = 0,
        HighPass = 1,
        BandPass = 2,
        NumTypes
    };

    enum class LFOShape : int
    {
        Sine = 0,
        Triangle = 1,
        Saw = 2,
        SampleAndHold = 3,
        NumShapes
    };

    inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        // Granular
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::grainSize, 1),
            "Grain Size",
            juce::NormalisableRange<float>(10.0f, 500.0f, 0.1f, 0.4f),
            80.0f,
            juce::AudioParameterFloatAttributes().withLabel("ms")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::density, 1),
            "Density",
            juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f, 0.4f),
            25.0f,
            juce::AudioParameterFloatAttributes().withLabel("Hz")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::position, 1),
            "Position",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
            0.5f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::spray, 1),
            "Spray",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
            0.15f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::pitch, 1),
            "Pitch",
            juce::NormalisableRange<float>(-24.0f, 24.0f, 0.05f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("st")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::pitchRand, 1),
            "Pitch Random",
            juce::NormalisableRange<float>(0.0f, 24.0f, 0.05f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("st")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::pan, 1),
            "Pan",
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::panRand, 1),
            "Pan Random",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.4f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::reverseProb, 1),
            "Reverse",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(ParamIDs::freeze, 1),
            "Freeze",
            false));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::feedback, 1),
            "Feedback",
            juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParamIDs::envelopeShape, 1),
            "Envelope Shape",
            juce::StringArray { "Hann", "Gaussian", "Tukey", "Triangle" },
            0));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::mix, 1),
            "Dry/Wet Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.005f),
            0.5f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::outputGain, 1),
            "Output Gain",
            juce::NormalisableRange<float>(-36.0f, 12.0f, 0.1f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB")));

        // Effects - Filter
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(ParamIDs::filterEnabled, 1),
            "Filter Enable",
            true));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParamIDs::filterType, 1),
            "Filter Type",
            juce::StringArray { "Low Pass", "High Pass", "Band Pass" },
            0));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::filterCutoff, 1),
            "Cutoff",
            juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.3f),
            12000.0f,
            juce::AudioParameterFloatAttributes().withLabel("Hz")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::filterResonance, 1),
            "Resonance",
            juce::NormalisableRange<float>(0.1f, 10.0f, 0.05f, 0.5f),
            0.707f,
            juce::AudioParameterFloatAttributes().withLabel("Q")));

        // Effects - Delay
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(ParamIDs::delayEnabled, 1),
            "Delay Enable",
            false));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::delayTime, 1),
            "Delay Time",
            juce::NormalisableRange<float>(10.0f, 2000.0f, 0.1f, 0.4f),
            320.0f,
            juce::AudioParameterFloatAttributes().withLabel("ms")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::delayFeedback, 1),
            "Delay Feedback",
            juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f),
            0.35f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::delayMix, 1),
            "Delay Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.3f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        // Effects - Reverb
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(ParamIDs::reverbEnabled, 1),
            "Reverb Enable",
            false));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::reverbSize, 1),
            "Reverb Size",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.65f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::reverbDamping, 1),
            "Reverb Damping",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.4f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::reverbMix, 1),
            "Reverb Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.35f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        // Effects - Saturation
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(ParamIDs::satEnabled, 1),
            "Drive Enable",
            false));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::satDrive, 1),
            "Drive",
            juce::NormalisableRange<float>(1.0f, 10.0f, 0.05f, 0.5f),
            1.5f,
            juce::AudioParameterFloatAttributes().withLabel("x")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::satMix, 1),
            "Drive Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.5f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        // Modulation - LFO 1
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::lfo1Rate, 1),
            "LFO 1 Rate",
            juce::NormalisableRange<float>(0.05f, 20.0f, 0.01f, 0.35f),
            0.5f,
            juce::AudioParameterFloatAttributes().withLabel("Hz")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::lfo1Depth, 1),
            "LFO 1 Depth",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParamIDs::lfo1Target, 1),
            "LFO 1 Target",
            juce::StringArray { "None", "Position", "Grain Size", "Density", "Pitch", "Pan", "Cutoff" },
            0));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParamIDs::lfo1Shape, 1),
            "LFO 1 Shape",
            juce::StringArray { "Sine", "Triangle", "Saw", "S&H" },
            0));

        // Modulation - LFO 2
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::lfo2Rate, 1),
            "LFO 2 Rate",
            juce::NormalisableRange<float>(0.05f, 20.0f, 0.01f, 0.35f),
            1.5f,
            juce::AudioParameterFloatAttributes().withLabel("Hz")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::lfo2Depth, 1),
            "LFO 2 Depth",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParamIDs::lfo2Target, 1),
            "LFO 2 Target",
            juce::StringArray { "None", "Position", "Grain Size", "Density", "Pitch", "Pan", "Cutoff" },
            0));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParamIDs::lfo2Shape, 1),
            "LFO 2 Shape",
            juce::StringArray { "Sine", "Triangle", "Saw", "S&H" },
            0));

        // XY Pad
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::xyX, 1),
            "XY Pad X",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
            0.5f,
            juce::AudioParameterFloatAttributes().withLabel("")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::xyY, 1),
            "XY Pad Y",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
            0.5f,
            juce::AudioParameterFloatAttributes().withLabel("")));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParamIDs::xyXTarget, 1),
            "XY X Assign",
            juce::StringArray { "Density", "Grain Size", "Position", "Spray", "Pitch", "Cutoff", "Mix" },
            0)); // Default X -> Density

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParamIDs::xyYTarget, 1),
            "XY Y Assign",
            juce::StringArray { "Density", "Grain Size", "Position", "Spray", "Pitch", "Cutoff", "Mix" },
            1)); // Default Y -> Grain Size

        return { params.begin(), params.end() };
    }
}
