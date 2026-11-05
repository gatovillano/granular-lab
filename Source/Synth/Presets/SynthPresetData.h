#pragma once

#include <vector>
#include <string>

namespace GranularSynthLab
{
    struct SynthPreset
    {
        std::string name;
        std::string category;
        int sourceMode { 0 };       // 0: Osc, 1: Sample
        int oscShape { 0 };         // 0: Supersaw, 1: Sine, 2: Tri, 3: Sqr, 4: Noise, 5: FM, 6: Vocal
        int rootNote { 60 };
        int octave { 0 };
        float fineTune { 0.0f };
        float grainSize { 80.0f };
        float density { 24.0f };
        float position { 0.25f };
        float spray { 0.15f };
        float pitchRand { 0.0f };
        float panRand { 0.4f };
        float reverse { 0.0f };
        int windowShape { 0 };      // 0: Hann, 1: Gaussian, 2: Tukey, 3: Triangle
        float attack { 20.0f };
        float decay { 350.0f };
        float sustain { 0.8f };
        float release { 600.0f };
        int filterType { 0 };       // 0: LP, 1: HP, 2: BP
        float filterCutoff { 8000.0f };
        float filterRes { 0.707f };
        float filterEnv { 0.0f };
        float delayTime { 250.0f };
        float delayFeedback { 0.35f };
        float delayMix { 0.2f };
        float reverbSize { 0.6f };
        float reverbDamp { 0.4f };
        float reverbMix { 0.3f };
        float drive { 0.15f };
        float masterGain { 1.0f };
        float padX { 0.5f };
        float padY { 0.5f };
    };

    inline const std::vector<SynthPreset>& getFactorySynthPresets()
    {
        static const std::vector<SynthPreset> presets = {
            // Default
            { "Init Synth", "Default", 0, 0, 60, 0, 0.0f, 80.0f, 24.0f, 0.25f, 0.15f, 0.0f, 0.4f, 0.0f, 0, 15.0f, 350.0f, 0.8f, 600.0f, 0, 12000.0f, 0.707f, 0.0f, 250.0f, 0.2f, 0.15f, 0.4f, 0.3f, 0.2f, 0.1f, 1.0f, 0.5f, 0.5f },

            // Ambient Pads
            { "Ethereal Pad", "Ambient", 0, 6, 60, 0, 0.0f, 220.0f, 18.0f, 0.3f, 0.25f, 0.0f, 0.6f, 0.0f, 1, 800.0f, 1200.0f, 0.9f, 2500.0f, 0, 6500.0f, 0.6f, 0.15f, 380.0f, 0.55f, 0.35f, 0.85f, 0.3f, 0.55f, 0.1f, 1.0f, 0.4f, 0.7f },
            { "Celestial Choir", "Ambient", 0, 6, 60, 0, 0.0f, 180.0f, 28.0f, 0.5f, 0.35f, 0.2f, 0.8f, 0.1f, 0, 600.0f, 1500.0f, 0.85f, 3000.0f, 0, 7800.0f, 0.7f, 0.0f, 420.0f, 0.6f, 0.4f, 0.9f, 0.25f, 0.65f, 0.05f, 1.0f, 0.6f, 0.6f },
            { "Warm Analog Strings", "Ambient", 0, 0, 60, 0, 0.0f, 140.0f, 32.0f, 0.2f, 0.1f, 0.1f, 0.5f, 0.0f, 0, 350.0f, 800.0f, 0.8f, 1800.0f, 0, 5200.0f, 0.8f, 0.2f, 290.0f, 0.45f, 0.25f, 0.7f, 0.5f, 0.4f, 0.2f, 1.0f, 0.5f, 0.5f },
            { "Sunken Memory", "Ambient", 0, 1, 60, -1, 0.0f, 300.0f, 12.0f, 0.4f, 0.4f, 0.0f, 0.7f, 0.2f, 1, 1200.0f, 2000.0f, 0.75f, 4000.0f, 0, 2400.0f, 0.9f, 0.0f, 500.0f, 0.65f, 0.45f, 0.95f, 0.7f, 0.6f, 0.25f, 1.05f, 0.3f, 0.8f },
            { "Drifting Aurora", "Ambient", 0, 5, 60, 0, 0.0f, 160.0f, 22.0f, 0.6f, 0.3f, 0.3f, 0.75f, 0.15f, 0, 500.0f, 1000.0f, 0.8f, 2200.0f, 0, 9500.0f, 0.65f, 0.1f, 350.0f, 0.5f, 0.35f, 0.8f, 0.35f, 0.5f, 0.1f, 0.95f, 0.5f, 0.55f },

            // Keys & Plucks
            { "Crystal Pluck", "Keys", 0, 5, 60, 0, 0.0f, 45.0f, 35.0f, 0.1f, 0.05f, 0.0f, 0.5f, 0.0f, 2, 2.0f, 450.0f, 0.1f, 750.0f, 0, 11000.0f, 1.1f, 0.4f, 180.0f, 0.4f, 0.3f, 0.65f, 0.3f, 0.35f, 0.15f, 1.0f, 0.65f, 0.25f },
            { "Velvet Rhodes Cloud", "Keys", 0, 2, 60, 0, 0.0f, 95.0f, 20.0f, 0.2f, 0.15f, 0.0f, 0.45f, 0.0f, 0, 5.0f, 700.0f, 0.3f, 900.0f, 0, 4800.0f, 0.85f, 0.25f, 260.0f, 0.35f, 0.25f, 0.55f, 0.5f, 0.3f, 0.25f, 1.05f, 0.45f, 0.4f },
            { "Glass Harp", "Keys", 0, 5, 60, 1, 0.0f, 60.0f, 26.0f, 0.15f, 0.1f, 0.0f, 0.6f, 0.0f, 0, 3.0f, 600.0f, 0.2f, 1100.0f, 2, 3800.0f, 1.5f, 0.3f, 220.0f, 0.45f, 0.3f, 0.75f, 0.3f, 0.45f, 0.1f, 1.0f, 0.55f, 0.3f },
            { "Cassette Memory Piano", "Keys", 0, 1, 60, 0, 0.0f, 85.0f, 22.0f, 0.35f, 0.2f, 0.15f, 0.5f, 0.05f, 3, 4.0f, 900.0f, 0.25f, 1200.0f, 0, 3200.0f, 0.7f, 0.1f, 310.0f, 0.4f, 0.25f, 0.6f, 0.65f, 0.35f, 0.35f, 1.1f, 0.4f, 0.35f },

            // Leads & Arps
            { "Supersaw Granular Lead", "Leads", 0, 0, 60, 0, 0.0f, 50.0f, 48.0f, 0.2f, 0.08f, 0.0f, 0.4f, 0.0f, 0, 8.0f, 400.0f, 0.85f, 350.0f, 0, 14000.0f, 1.2f, 0.3f, 240.0f, 0.4f, 0.25f, 0.5f, 0.4f, 0.25f, 0.35f, 0.95f, 0.8f, 0.3f },
            { "Cyberpunk Pulse", "Leads", 0, 3, 60, 0, 0.0f, 35.0f, 40.0f, 0.1f, 0.05f, 0.0f, 0.3f, 0.0f, 2, 5.0f, 300.0f, 0.7f, 250.0f, 0, 6200.0f, 1.8f, 0.5f, 160.0f, 0.5f, 0.35f, 0.4f, 0.4f, 0.2f, 0.5f, 1.0f, 0.7f, 0.2f },
            { "Singing Resonator", "Leads", 0, 2, 60, 0, 0.0f, 70.0f, 30.0f, 0.3f, 0.1f, 0.0f, 0.4f, 0.0f, 0, 45.0f, 500.0f, 0.9f, 600.0f, 2, 2200.0f, 4.5f, 0.2f, 330.0f, 0.55f, 0.3f, 0.7f, 0.35f, 0.4f, 0.3f, 1.0f, 0.5f, 0.45f },

            // Drones & Bass
            { "Dark Sub Bass", "Bass", 0, 1, 60, -2, 0.0f, 120.0f, 25.0f, 0.1f, 0.05f, 0.0f, 0.1f, 0.0f, 0, 8.0f, 300.0f, 0.95f, 200.0f, 0, 850.0f, 0.707f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.8f, 0.05f, 0.4f, 1.1f, 0.5f, 0.6f },
            { "Subterranean Monolith", "Bass", 0, 0, 60, -2, 0.0f, 180.0f, 20.0f, 0.2f, 0.1f, 0.0f, 0.3f, 0.0f, 0, 25.0f, 600.0f, 0.9f, 450.0f, 0, 1400.0f, 1.4f, 0.15f, 190.0f, 0.3f, 0.15f, 0.45f, 0.6f, 0.2f, 0.55f, 1.05f, 0.4f, 0.65f },
            { "Cosmic Drone C3", "Drones", 0, 0, 60, -1, 0.0f, 350.0f, 15.0f, 0.45f, 0.3f, 0.0f, 0.8f, 0.2f, 1, 1500.0f, 2500.0f, 1.0f, 5000.0f, 0, 3100.0f, 0.9f, 0.0f, 450.0f, 0.6f, 0.4f, 0.9f, 0.5f, 0.6f, 0.2f, 1.0f, 0.3f, 0.85f },
            { "Void Whispers", "Drones", 0, 4, 60, 0, 0.0f, 260.0f, 20.0f, 0.5f, 0.5f, 0.0f, 0.9f, 0.3f, 1, 2000.0f, 3000.0f, 0.9f, 6000.0f, 2, 1600.0f, 2.2f, 0.0f, 520.0f, 0.7f, 0.5f, 0.95f, 0.4f, 0.7f, 0.15f, 1.0f, 0.35f, 0.75f },

            // Glitch & Experimental
            { "Time Shifter", "Glitch", 0, 0, 60, 0, 0.0f, 30.0f, 55.0f, 0.4f, 0.35f, 12.0f, 0.8f, 0.45f, 2, 2.0f, 250.0f, 0.6f, 400.0f, 0, 9200.0f, 1.5f, 0.0f, 140.0f, 0.65f, 0.45f, 0.5f, 0.3f, 0.3f, 0.4f, 0.95f, 0.85f, 0.2f },
            { "Reverse Dream Particles", "Glitch", 0, 5, 60, 0, 0.0f, 110.0f, 30.0f, 0.6f, 0.25f, 0.0f, 0.7f, 1.0f, 0, 50.0f, 600.0f, 0.7f, 1200.0f, 0, 7500.0f, 0.8f, 0.0f, 320.0f, 0.5f, 0.4f, 0.8f, 0.35f, 0.5f, 0.2f, 1.0f, 0.6f, 0.5f },
            { "Static Grain Tempest", "Glitch", 0, 4, 60, 0, 0.0f, 22.0f, 65.0f, 0.5f, 0.6f, 7.0f, 0.95f, 0.25f, 3, 1.0f, 200.0f, 0.5f, 350.0f, 1, 1500.0f, 1.8f, 0.0f, 120.0f, 0.7f, 0.5f, 0.65f, 0.2f, 0.4f, 0.5f, 0.95f, 0.9f, 0.15f },
            { "Morphing Wavetable Odyssey", "Experimental", 0, 5, 60, 0, 0.0f, 150.0f, 26.0f, 0.35f, 0.4f, 0.0f, 0.7f, 0.1f, 0, 400.0f, 1200.0f, 0.8f, 2500.0f, 0, 5600.0f, 1.2f, 0.3f, 360.0f, 0.55f, 0.35f, 0.75f, 0.4f, 0.45f, 0.25f, 1.0f, 0.55f, 0.55f }
        };

        return presets;
    }
}
