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
                { "filter_enabled", 1.0f }, { "filter_cutoff", 9500.0f }, { "filter_resonance", 0.7f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.8f }, { "reverb_mix", 0.45f }
            }
        });

        // 2. Glitch Reverse
        list.push_back({
            "Glitch Reverse", "Rhythmic",
            {
                { "grain_size", 45.0f }, { "density", 40.0f }, { "position", 0.3f }, { "spray", 0.5f },
                { "pitch", -12.0f }, { "pitch_rand", 7.0f }, { "pan", 0.0f }, { "pan_rand", 0.8f },
                { "reverse_prob", 0.85f }, { "feedback", 0.15f }, { "mix", 0.75f }, { "output_gain", 0.0f },
                { "sat_enabled", 1.0f }, { "sat_drive", 2.8f }, { "sat_mix", 0.4f },
                { "delay_enabled", 1.0f }, { "delay_time", 180.0f }, { "delay_feedback", 0.4f }, { "delay_mix", 0.35f }
            }
        });

        // 3. Shimmer Pad
        list.push_back({
            "Shimmer Pad", "Textures",
            {
                { "grain_size", 280.0f }, { "density", 35.0f }, { "position", 0.7f }, { "spray", 0.2f },
                { "pitch", 12.0f }, { "pitch_rand", 0.5f }, { "pan", 0.0f }, { "pan_rand", 0.5f },
                { "reverse_prob", 0.1f }, { "feedback", 0.3f }, { "mix", 0.6f }, { "output_gain", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_cutoff", 11000.0f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.9f }, { "reverb_mix", 0.6f }
            }
        });

        // 4. Frozen Drone
        list.push_back({
            "Frozen Drone", "Drones",
            {
                { "grain_size", 120.0f }, { "density", 60.0f }, { "position", 0.5f }, { "spray", 0.05f },
                { "pitch", -7.0f }, { "pitch_rand", 0.0f }, { "pan", 0.0f }, { "pan_rand", 0.3f },
                { "freeze", 1.0f }, { "feedback", 0.4f }, { "mix", 0.85f }, { "output_gain", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_cutoff", 4200.0f }, { "filter_resonance", 2.5f },
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.7f }, { "reverb_mix", 0.3f }
            }
        });

        // 5. Vocal Scatter
        list.push_back({
            "Vocal Scatter", "Experimental",
            {
                { "grain_size", 70.0f }, { "density", 22.0f }, { "position", 0.4f }, { "spray", 0.6f },
                { "pitch", 0.0f }, { "pitch_rand", 12.0f }, { "pan", 0.0f }, { "pan_rand", 0.9f },
                { "reverse_prob", 0.4f }, { "feedback", 0.1f }, { "mix", 0.7f }, { "output_gain", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_cutoff", 14000.0f },
                { "delay_enabled", 1.0f }, { "delay_time", 420.0f }, { "delay_feedback", 0.5f }, { "delay_mix", 0.3f }
            }
        });

        // 6. Cosmic Dust
        list.push_back({
            "Cosmic Dust", "Ambient",
            {
                { "grain_size", 18.0f }, { "density", 75.0f }, { "position", 0.8f }, { "spray", 0.7f },
                { "pitch", 24.0f }, { "pitch_rand", 12.0f }, { "pan", 0.0f }, { "pan_rand", 0.95f },
                { "reverse_prob", 0.5f }, { "feedback", 0.2f }, { "mix", 0.5f }, { "output_gain", 0.0f },
                { "filter_enabled", 1.0f }, { "filter_type", 1.0f }, { "filter_cutoff", 1200.0f }, // Highpass
                { "reverb_enabled", 1.0f }, { "reverb_size", 0.95f }, { "reverb_mix", 0.5f }
            }
        });

        return list;
    }
}
