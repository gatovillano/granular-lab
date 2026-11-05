#pragma once

#include <vector>
#include <juce_core/juce_core.h>

namespace GranularLab
{
    struct Preset
    {
        juce::String name;
        juce::String category;
        std::vector<std::pair<juce::String, float>> paramValues;
    };

    inline std::vector<Preset> getFactoryPresets()
    {
        std::vector<Preset> list;

        // 1. Atmospheric Cloud
        list.push_back({
            "Atmospheric Cloud", "Ambient",
            {
                { "grain_size", 180.0f }, { "density", 28.0f }, { "position", 0.65f }, { "spray", 0.35f },
                { "pitch", 0.0f }, { "pitch_rand", 2.0f }, { "pan", 0.0f }, { "pan_rand", 0.6f },
                { "reverse_prob", 0.15f }, { "feedback", 0.25f }, { "mix", 0.65f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 9500.0f }, { "filter_resonance", 0.7f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.8f }, { "reverb_damping", 0.4f }, { "reverb_mix", 0.45f },
                { "delay_enabled", 0.0f }, { "sat_enabled", 0.0f },
                { "lfo1_depth", 0.0f }, { "lfo2_depth", 0.0f }
            }
        });

        // 2. Glitch Reverse
        list.push_back({
            "Glitch Reverse", "Rhythmic",
            {
                { "grain_size", 45.0f }, { "density", 40.0f }, { "position", 0.3f }, { "spray", 0.5f },
                { "pitch", -12.0f }, { "pitch_rand", 7.0f }, { "pan", 0.0f }, { "pan_rand", 0.8f },
                { "reverse_prob", 0.85f }, { "feedback", 0.15f }, { "mix", 0.75f }, { "output_gain", 0.0f },
                { "envelope_shape", 2.0f },
                { "sat_enabled", 1.0f }, { "sat_drive", 2.8f }, { "sat_mix", 0.4f },
                { "delay_enabled", 1.0f }, { "delay_time", 180.0f }, { "delay_feedback", 0.4f }, { "delay_mix", 0.35f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 14000.0f }, { "filter_resonance", 0.8f },
                { "reverb_enabled", 0.0f },
                { "lfo1_depth", 0.0f }, { "lfo2_depth", 0.0f }
            }
        });

        // 3. Shimmer Pad
        list.push_back({
            "Shimmer Pad", "Textures",
            {
                { "grain_size", 280.0f }, { "density", 35.0f }, { "position", 0.7f }, { "spray", 0.2f },
                { "pitch", 12.0f }, { "pitch_rand", 0.5f }, { "pan", 0.0f }, { "pan_rand", 0.5f },
                { "reverse_prob", 0.1f }, { "feedback", 0.3f }, { "mix", 0.6f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 11000.0f }, { "filter_resonance", 0.7f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.9f }, { "reverb_damping", 0.3f }, { "reverb_mix", 0.6f },
                { "delay_enabled", 0.0f }, { "sat_enabled", 0.0f },
                { "lfo1_depth", 0.0f }, { "lfo2_depth", 0.0f }
            }
        });

        // 4. Frozen Drone
        list.push_back({
            "Frozen Drone", "Drones",
            {
                { "grain_size", 120.0f }, { "density", 60.0f }, { "position", 0.5f }, { "spray", 0.05f },
                { "pitch", -7.0f }, { "pitch_rand", 0.0f }, { "pan", 0.0f }, { "pan_rand", 0.3f },
                { "freeze", 1.0f }, { "feedback", 0.4f }, { "mix", 0.85f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 4200.0f }, { "filter_resonance", 2.5f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.7f }, { "reverb_damping", 0.5f }, { "reverb_mix", 0.3f },
                { "delay_enabled", 0.0f }, { "sat_enabled", 0.0f },
                { "lfo1_depth", 0.0f }, { "lfo2_depth", 0.0f }
            }
        });

        // 5. Vocal Scatter
        list.push_back({
            "Vocal Scatter", "Experimental",
            {
                { "grain_size", 70.0f }, { "density", 22.0f }, { "position", 0.4f }, { "spray", 0.6f },
                { "pitch", 0.0f }, { "pitch_rand", 12.0f }, { "pan", 0.0f }, { "pan_rand", 0.9f },
                { "reverse_prob", 0.4f }, { "feedback", 0.1f }, { "mix", 0.7f }, { "output_gain", 0.0f },
                { "envelope_shape", 1.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 14000.0f }, { "filter_resonance", 0.7f },
                { "delay_enabled", 1.0f }, { "delay_time", 420.0f }, { "delay_feedback", 0.5f }, { "delay_mix", 0.3f },
                { "reverb_enabled", 0.0f }, { "sat_enabled", 0.0f },
                { "lfo1_depth", 0.0f }, { "lfo2_depth", 0.0f }
            }
        });

        // 6. Cosmic Dust
        list.push_back({
            "Cosmic Dust", "Ambient",
            {
                { "grain_size", 18.0f }, { "density", 75.0f }, { "position", 0.8f }, { "spray", 0.7f },
                { "pitch", 24.0f }, { "pitch_rand", 12.0f }, { "pan", 0.0f }, { "pan_rand", 0.95f },
                { "reverse_prob", 0.5f }, { "feedback", 0.2f }, { "mix", 0.5f }, { "output_gain", 0.0f },
                { "envelope_shape", 1.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 1.0f }, { "filter_cutoff", 1200.0f }, { "filter_resonance", 1.0f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.95f }, { "reverb_damping", 0.2f }, { "reverb_mix", 0.5f },
                { "delay_enabled", 0.0f }, { "sat_enabled", 0.0f },
                { "lfo1_depth", 0.0f }, { "lfo2_depth", 0.0f }
            }
        });

        // 7. Nebula Drift
        list.push_back({
            "Nebula Drift", "Ambient",
            {
                { "grain_size", 220.0f }, { "density", 20.0f }, { "position", 0.6f }, { "spray", 0.4f },
                { "pitch", 7.0f }, { "pitch_rand", 0.8f }, { "pan", 0.0f }, { "pan_rand", 0.75f },
                { "reverse_prob", 0.2f }, { "feedback", 0.28f }, { "mix", 0.7f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 8000.0f }, { "filter_resonance", 0.8f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.88f }, { "reverb_damping", 0.35f }, { "reverb_mix", 0.5f },
                { "delay_enabled", 1.0f }, { "delay_time", 480.0f }, { "delay_feedback", 0.3f }, { "delay_mix", 0.25f },
                { "lfo1_rate", 0.15f }, { "lfo1_depth", 0.45f }, { "lfo1_target", 1.0f }, { "lfo1_shape", 0.0f } // LFO -> Position
            }
        });

        // 8. Deep Abyss
        list.push_back({
            "Deep Abyss", "Drones",
            {
                { "grain_size", 340.0f }, { "density", 45.0f }, { "position", 0.75f }, { "spray", 0.15f },
                { "pitch", -24.0f }, { "pitch_rand", 0.2f }, { "pan", 0.0f }, { "pan_rand", 0.4f },
                { "reverse_prob", 0.0f }, { "feedback", 0.35f }, { "mix", 0.8f }, { "output_gain", 1.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 750.0f }, { "filter_resonance", 2.0f },
                { "sat_enabled", 1.0f }, { "sat_drive", 2.2f }, { "sat_mix", 0.45f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.9f }, { "reverb_damping", 0.7f }, { "reverb_mix", 0.4f },
                { "delay_enabled", 0.0f }
            }
        });

        // 9. Crystal Echoes
        list.push_back({
            "Crystal Echoes", "Textures",
            {
                { "grain_size", 32.0f }, { "density", 30.0f }, { "position", 0.35f }, { "spray", 0.3f },
                { "pitch", 19.0f }, { "pitch_rand", 0.5f }, { "pan", 0.0f }, { "pan_rand", 0.85f },
                { "reverse_prob", 0.25f }, { "feedback", 0.2f }, { "mix", 0.6f }, { "output_gain", -1.0f },
                { "envelope_shape", 1.0f }, // Gaussian
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 12500.0f }, { "filter_resonance", 1.2f },
                { "delay_enabled", 1.0f }, { "delay_time", 340.0f }, { "delay_feedback", 0.55f }, { "delay_mix", 0.45f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.75f }, { "reverb_damping", 0.3f }, { "reverb_mix", 0.35f }
            }
        });

        // 10. Solar Flare
        list.push_back({
            "Solar Flare", "Textures",
            {
                { "grain_size", 140.0f }, { "density", 38.0f }, { "position", 0.5f }, { "spray", 0.25f },
                { "pitch", 0.0f }, { "pitch_rand", 3.0f }, { "pan", 0.0f }, { "pan_rand", 0.6f },
                { "reverse_prob", 0.2f }, { "feedback", 0.35f }, { "mix", 0.7f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "sat_enabled", 1.0f }, { "sat_drive", 3.8f }, { "sat_mix", 0.6f },
                { "filter_enabled", 1.0f }, { "filter_type", 2.0f }, { "filter_cutoff", 2200.0f }, { "filter_resonance", 2.2f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.7f }, { "reverb_damping", 0.4f }, { "reverb_mix", 0.35f },
                { "lfo1_rate", 0.4f }, { "lfo1_depth", 0.6f }, { "lfo1_target", 6.0f }, { "lfo1_shape", 1.0f } // LFO -> Cutoff
            }
        });

        // 11. Ethereal Whispers
        list.push_back({
            "Ethereal Whispers", "Ambient",
            {
                { "grain_size", 90.0f }, { "density", 26.0f }, { "position", 0.45f }, { "spray", 0.5f },
                { "pitch", 0.0f }, { "pitch_rand", 0.5f }, { "pan", 0.0f }, { "pan_rand", 0.9f },
                { "reverse_prob", 0.45f }, { "feedback", 0.18f }, { "mix", 0.65f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 15000.0f }, { "filter_resonance", 0.6f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.85f }, { "reverb_damping", 0.25f }, { "reverb_mix", 0.5f },
                { "lfo1_rate", 0.2f }, { "lfo1_depth", 0.5f }, { "lfo1_target", 5.0f }, { "lfo1_shape", 0.0f } // LFO -> Pan
            }
        });

        // 12. Stutter Machine
        list.push_back({
            "Stutter Machine", "Rhythmic",
            {
                { "grain_size", 22.0f }, { "density", 65.0f }, { "position", 0.15f }, { "spray", 0.0f },
                { "pitch", 0.0f }, { "pitch_rand", 0.0f }, { "pan", 0.0f }, { "pan_rand", 0.2f },
                { "reverse_prob", 0.0f }, { "feedback", 0.25f }, { "mix", 0.8f }, { "output_gain", 0.0f },
                { "envelope_shape", 2.0f }, // Tukey
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 16000.0f }, { "filter_resonance", 0.7f },
                { "delay_enabled", 1.0f }, { "delay_time", 125.0f }, { "delay_feedback", 0.5f }, { "delay_mix", 0.4f },
                { "sat_enabled", 1.0f }, { "sat_drive", 1.8f }, { "sat_mix", 0.3f }
            }
        });

        // 13. Digital Artefacts
        list.push_back({
            "Digital Artefacts", "Glitch",
            {
                { "grain_size", 14.0f }, { "density", 50.0f }, { "position", 0.28f }, { "spray", 0.75f },
                { "pitch", -7.0f }, { "pitch_rand", 14.0f }, { "pan", 0.0f }, { "pan_rand", 0.95f },
                { "reverse_prob", 0.8f }, { "feedback", 0.3f }, { "mix", 0.85f }, { "output_gain", -1.0f },
                { "envelope_shape", 2.0f },
                { "sat_enabled", 1.0f }, { "sat_drive", 6.5f }, { "sat_mix", 0.7f },
                { "filter_enabled", 1.0f }, { "filter_type", 1.0f }, { "filter_cutoff", 900.0f }, { "filter_resonance", 1.8f },
                { "delay_enabled", 1.0f }, { "delay_time", 95.0f }, { "delay_feedback", 0.45f }, { "delay_mix", 0.3f }
            }
        });

        // 14. Downsample Crunch
        list.push_back({
            "Downsample Crunch", "Glitch",
            {
                { "grain_size", 35.0f }, { "density", 32.0f }, { "position", 0.2f }, { "spray", 0.4f },
                { "pitch", -12.0f }, { "pitch_rand", 12.0f }, { "pan", 0.0f }, { "pan_rand", 0.7f },
                { "reverse_prob", 0.35f }, { "feedback", 0.2f }, { "mix", 0.75f }, { "output_gain", 0.0f },
                { "envelope_shape", 3.0f }, // Triangle
                { "sat_enabled", 1.0f }, { "sat_drive", 5.2f }, { "sat_mix", 0.65f },
                { "filter_enabled", 1.0f }, { "filter_type", 2.0f }, { "filter_cutoff", 1400.0f }, { "filter_resonance", 2.8f }
            }
        });

        // 15. Time Bender
        list.push_back({
            "Time Bender", "Experimental",
            {
                { "grain_size", 160.0f }, { "density", 35.0f }, { "position", 0.5f }, { "spray", 0.2f },
                { "pitch", -5.0f }, { "pitch_rand", 1.5f }, { "pan", 0.0f }, { "pan_rand", 0.5f },
                { "reverse_prob", 0.3f }, { "feedback", 0.6f }, { "mix", 0.8f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 6500.0f }, { "filter_resonance", 1.2f },
                { "delay_enabled", 1.0f }, { "delay_time", 480.0f }, { "delay_feedback", 0.4f }, { "delay_mix", 0.35f },
                { "lfo1_rate", 0.6f }, { "lfo1_depth", 0.75f }, { "lfo1_target", 1.0f }, { "lfo1_shape", 0.0f } // LFO -> Position
            }
        });

        // 16. Chaos Generator
        list.push_back({
            "Chaos Generator", "Glitch",
            {
                { "grain_size", 28.0f }, { "density", 55.0f }, { "position", 0.4f }, { "spray", 0.85f },
                { "pitch", 0.0f }, { "pitch_rand", 18.0f }, { "pan", 0.0f }, { "pan_rand", 0.95f },
                { "reverse_prob", 0.6f }, { "feedback", 0.3f }, { "mix", 0.9f }, { "output_gain", -1.0f },
                { "envelope_shape", 2.0f },
                { "sat_enabled", 1.0f }, { "sat_drive", 3.0f }, { "sat_mix", 0.5f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 11000.0f }, { "filter_resonance", 1.5f },
                { "lfo1_rate", 3.5f }, { "lfo1_depth", 0.8f }, { "lfo1_target", 4.0f }, { "lfo1_shape", 3.0f } // LFO -> Pitch (S&H)
            }
        });

        // 17. Dark Monolith
        list.push_back({
            "Dark Monolith", "Drones",
            {
                { "grain_size", 260.0f }, { "density", 40.0f }, { "position", 0.7f }, { "spray", 0.1f },
                { "pitch", -12.0f }, { "pitch_rand", 0.0f }, { "pan", 0.0f }, { "pan_rand", 0.35f },
                { "reverse_prob", 0.0f }, { "feedback", 0.45f }, { "mix", 0.85f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 1100.0f }, { "filter_resonance", 1.8f },
                { "sat_enabled", 1.0f }, { "sat_drive", 2.0f }, { "sat_mix", 0.35f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.85f }, { "reverb_damping", 0.6f }, { "reverb_mix", 0.4f },
                { "lfo1_rate", 0.25f }, { "lfo1_depth", 0.6f }, { "lfo1_target", 2.0f }, { "lfo1_shape", 1.0f } // LFO -> Size
            }
        });

        // 18. Glass Reflections
        list.push_back({
            "Glass Reflections", "Textures",
            {
                { "grain_size", 40.0f }, { "density", 36.0f }, { "position", 0.5f }, { "spray", 0.35f },
                { "pitch", 24.0f }, { "pitch_rand", 0.2f }, { "pan", 0.0f }, { "pan_rand", 0.8f },
                { "reverse_prob", 0.2f }, { "feedback", 0.25f }, { "mix", 0.65f }, { "output_gain", -1.0f },
                { "envelope_shape", 1.0f }, // Gaussian
                { "filter_enabled", 1.0f }, { "filter_type", 2.0f }, { "filter_cutoff", 3800.0f }, { "filter_resonance", 3.2f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.9f }, { "reverb_damping", 0.2f }, { "reverb_mix", 0.55f },
                { "delay_enabled", 1.0f }, { "delay_time", 240.0f }, { "delay_feedback", 0.4f }, { "delay_mix", 0.3f }
            }
        });

        // 19. Tape Degrader
        list.push_back({
            "Tape Degrader", "Lo-Fi",
            {
                { "grain_size", 65.0f }, { "density", 28.0f }, { "position", 0.3f }, { "spray", 0.2f },
                { "pitch", 0.0f }, { "pitch_rand", 0.4f }, { "pan", 0.0f }, { "pan_rand", 0.5f },
                { "reverse_prob", 0.1f }, { "feedback", 0.2f }, { "mix", 0.7f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "sat_enabled", 1.0f }, { "sat_drive", 2.4f }, { "sat_mix", 0.55f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 5200.0f }, { "filter_resonance", 0.9f },
                { "delay_enabled", 1.0f }, { "delay_time", 280.0f }, { "delay_feedback", 0.35f }, { "delay_mix", 0.25f },
                { "lfo1_rate", 1.8f }, { "lfo1_depth", 0.15f }, { "lfo1_target", 4.0f }, { "lfo1_shape", 1.0f } // Wow & flutter
            }
        });

        // 20. Sub Harmonic Cloud
        list.push_back({
            "Sub Harmonic Cloud", "Drones",
            {
                { "grain_size", 200.0f }, { "density", 45.0f }, { "position", 0.65f }, { "spray", 0.2f },
                { "pitch", -12.0f }, { "pitch_rand", 5.0f }, { "pan", 0.0f }, { "pan_rand", 0.65f },
                { "reverse_prob", 0.15f }, { "feedback", 0.3f }, { "mix", 0.75f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 3200.0f }, { "filter_resonance", 1.5f },
                { "sat_enabled", 1.0f }, { "sat_drive", 1.9f }, { "sat_mix", 0.35f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.8f }, { "reverb_damping", 0.5f }, { "reverb_mix", 0.4f }
            }
        });

        // 21. Liquid Resonator
        list.push_back({
            "Liquid Resonator", "Experimental",
            {
                { "grain_size", 32.0f }, { "density", 42.0f }, { "position", 0.4f }, { "spray", 0.3f },
                { "pitch", 5.0f }, { "pitch_rand", 2.0f }, { "pan", 0.0f }, { "pan_rand", 0.7f },
                { "reverse_prob", 0.35f }, { "feedback", 0.45f }, { "mix", 0.8f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 2.0f }, { "filter_cutoff", 2800.0f }, { "filter_resonance", 4.0f },
                { "delay_enabled", 1.0f }, { "delay_time", 210.0f }, { "delay_feedback", 0.45f }, { "delay_mix", 0.35f },
                { "lfo1_rate", 0.8f }, { "lfo1_depth", 0.7f }, { "lfo1_target", 6.0f }, { "lfo1_shape", 0.0f } // Sweeping cutoff
            }
        });

        // 22. Ghost Choir
        list.push_back({
            "Ghost Choir", "Ambient",
            {
                { "grain_size", 190.0f }, { "density", 30.0f }, { "position", 0.55f }, { "spray", 0.45f },
                { "pitch", 12.0f }, { "pitch_rand", 1.2f }, { "pan", 0.0f }, { "pan_rand", 0.85f },
                { "reverse_prob", 0.25f }, { "feedback", 0.25f }, { "mix", 0.7f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 9000.0f }, { "filter_resonance", 0.9f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.95f }, { "reverb_damping", 0.25f }, { "reverb_mix", 0.65f },
                { "delay_enabled", 1.0f }, { "delay_time", 390.0f }, { "delay_feedback", 0.35f }, { "delay_mix", 0.2f }
            }
        });

        // 23. Radio Transmission
        list.push_back({
            "Radio Transmission", "Lo-Fi",
            {
                { "grain_size", 45.0f }, { "density", 35.0f }, { "position", 0.25f }, { "spray", 0.5f },
                { "pitch", 0.0f }, { "pitch_rand", 0.8f }, { "pan", 0.0f }, { "pan_rand", 0.4f },
                { "reverse_prob", 0.3f }, { "feedback", 0.25f }, { "mix", 0.75f }, { "output_gain", 0.0f },
                { "envelope_shape", 2.0f },
                { "sat_enabled", 1.0f }, { "sat_drive", 4.2f }, { "sat_mix", 0.6f },
                { "filter_enabled", 1.0f }, { "filter_type", 2.0f }, { "filter_cutoff", 1100.0f }, { "filter_resonance", 2.5f },
                { "delay_enabled", 1.0f }, { "delay_time", 140.0f }, { "delay_feedback", 0.4f }, { "delay_mix", 0.25f }
            }
        });

        // 24. Detuned Ensemble
        list.push_back({
            "Detuned Ensemble", "Textures",
            {
                { "grain_size", 85.0f }, { "density", 32.0f }, { "position", 0.35f }, { "spray", 0.25f },
                { "pitch", 0.0f }, { "pitch_rand", 1.4f }, { "pan", 0.0f }, { "pan_rand", 0.9f },
                { "reverse_prob", 0.05f }, { "feedback", 0.15f }, { "mix", 0.55f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 13500.0f }, { "filter_resonance", 0.7f },
                { "delay_enabled", 1.0f }, { "delay_time", 290.0f }, { "delay_feedback", 0.3f }, { "delay_mix", 0.25f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.6f }, { "reverb_damping", 0.4f }, { "reverb_mix", 0.3f }
            }
        });

        // 25. Reverse Siren
        list.push_back({
            "Reverse Siren", "Experimental",
            {
                { "grain_size", 120.0f }, { "density", 28.0f }, { "position", 0.4f }, { "spray", 0.4f },
                { "pitch", 0.0f }, { "pitch_rand", 2.0f }, { "pan", 0.0f }, { "pan_rand", 0.8f },
                { "reverse_prob", 0.95f }, { "feedback", 0.4f }, { "mix", 0.8f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 7500.0f }, { "filter_resonance", 1.8f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.85f }, { "reverb_damping", 0.3f }, { "reverb_mix", 0.45f },
                { "lfo1_rate", 0.45f }, { "lfo1_depth", 0.7f }, { "lfo1_target", 4.0f }, { "lfo1_shape", 2.0f } // Saw -> Pitch
            }
        });

        // 26. Frozen Shimmer
        list.push_back({
            "Frozen Shimmer", "Ambient",
            {
                { "grain_size", 150.0f }, { "density", 40.0f }, { "position", 0.5f }, { "spray", 0.15f },
                { "pitch", 12.0f }, { "pitch_rand", 0.3f }, { "pan", 0.0f }, { "pan_rand", 0.6f },
                { "freeze", 1.0f }, { "feedback", 0.35f }, { "mix", 0.75f }, { "output_gain", 0.0f },
                { "envelope_shape", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 0.0f }, { "filter_cutoff", 6000.0f }, { "filter_resonance", 1.4f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.9f }, { "reverb_damping", 0.35f }, { "reverb_mix", 0.55f },
                { "lfo1_rate", 0.3f }, { "lfo1_depth", 0.4f }, { "lfo1_target", 6.0f }, { "lfo1_shape", 0.0f }
            }
        });

        return list;
    }
}
