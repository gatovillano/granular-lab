#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include "../Source/DSP/AudioBuffer.h"
#include "../Source/DSP/Grain.h"
#include "../Source/DSP/GranularEngine.h"
#include "../Source/DSP/ModulationEngine.h"
#include "../Source/DSP/EffectsEngine.h"

int main()
{
    std::cout << "=== GRANULAR LAB DSP UNIT TESTS ===" << std::endl;

    constexpr double sampleRate = 48000.0;
    constexpr int blockSize = 256;

    // 1. Test CircularAudioBuffer
    {
        std::cout << "[Test 1] CircularAudioBuffer write & Hermite read... ";
        GranularLab::CircularAudioBuffer buffer;
        buffer.prepare(sampleRate, 2.0); // 2 seconds

        // Fill buffer with 440Hz sine wave
        for (int i = 0; i < 48000; ++i)
        {
            float s = std::sin(2.0 * 3.141592653589793 * 440.0 * i / sampleRate);
            buffer.write(s, s);
        }

        assert(buffer.getWriteIndex() == 48000);
        assert(!buffer.getFrozen());

        // Read at fractional position
        float r1 = buffer.readHermite(0, 1000.5);
        assert(!std::isnan(r1) && !std::isinf(r1));

        // Test Freeze
        buffer.setFrozen(true);
        assert(buffer.getFrozen());
        buffer.write(0.5f, 0.5f);
        // Write index should NOT advance when frozen
        assert(buffer.getWriteIndex() == 48000);

        std::cout << "PASSED" << std::endl;
    }

    // 2. Test Grain
    {
        std::cout << "[Test 2] Grain rendering & window envelope... ";
        GranularLab::CircularAudioBuffer buffer;
        buffer.prepare(sampleRate, 1.0);
        for (int i = 0; i < 48000; ++i) buffer.write(0.8f, 0.8f);

        GranularLab::Grain grain;
        grain.init(100.0, 480.0, 1.0, 0.0f, GranularLab::EnvelopeShape::Hann, false, 1.0f, 10.0f, 0.0f, 0.1f);
        assert(grain.isActive());

        float outL = 0.0f, outR = 0.0f;
        int processedSamples = 0;
        while (grain.isActive())
        {
            grain.processSample(buffer, outL, outR);
            processedSamples++;
        }

        assert(processedSamples == 480);
        assert(!grain.isActive());
        assert(!std::isnan(outL) && !std::isinf(outL));

        std::cout << "PASSED" << std::endl;
    }

    // 3. Test GranularEngine real-time block processing
    {
        std::cout << "[Test 3] GranularEngine block processing & polyphony... ";
        GranularLab::GranularEngine engine;
        engine.prepare(sampleRate, blockSize);

        std::vector<float> inL(blockSize, 0.5f);
        std::vector<float> inR(blockSize, 0.5f);
        std::vector<float> outL(blockSize, 0.0f);
        std::vector<float> outR(blockSize, 0.0f);

        const float* inputPointers[2] = { inL.data(), inR.data() };
        float* outputPointers[2] = { outL.data(), outR.data() };

        GranularLab::GranularEngine::Parameters params;
        params.grainSizeMs = 50.0f;
        params.densityHz = 30.0f;
        params.position = 0.2f;
        params.spray = 0.1f;
        params.pitchSemitones = 5.0f;
        params.pitchRandSemitones = 2.0f;
        params.pan = 0.1f;
        params.panRand = 0.3f;
        params.reverseProb = 0.2f;
        params.freeze = false;
        params.feedback = 0.2f;

        // Process 100 blocks
        for (int b = 0; b < 100; ++b)
        {
            engine.processBlock(inputPointers, outputPointers, blockSize, params);
            for (int s = 0; s < blockSize; ++s)
            {
                assert(!std::isnan(outL[s]) && !std::isinf(outL[s]));
                assert(!std::isnan(outR[s]) && !std::isinf(outR[s]));
            }
        }

        // Test visual snapshot queue
        GranularLab::VisualSnapshot snap;
        bool hasSnap = engine.popVisualSnapshot(snap);
        assert(hasSnap);
        assert(snap.activeCount > 0);

        std::cout << "PASSED (" << snap.activeCount << " active grains)" << std::endl;
    }

    // 4. Test ModulationEngine
    {
        std::cout << "[Test 4] ModulationEngine LFOs & destinations... ";
        GranularLab::ModulationEngine modEngine;
        modEngine.prepare(sampleRate);

        GranularLab::ModulationEngine::LFOConfig lfo1;
        lfo1.rateHz = 2.0f;
        lfo1.depth = 0.8f;
        lfo1.target = GranularLab::ModTarget::Pitch;
        lfo1.shape = GranularLab::LFOShape::Sine;

        GranularLab::ModulationEngine::LFOConfig lfo2;
        lfo2.rateHz = 4.0f;
        lfo2.depth = 0.5f;
        lfo2.target = GranularLab::ModTarget::Position;
        lfo2.shape = GranularLab::LFOShape::SampleAndHold;

        for (int i = 0; i < 1000; ++i)
        {
            auto out = modEngine.process(lfo1, lfo2);
            assert(!std::isnan(out.pitchMod) && !std::isinf(out.pitchMod));
            assert(!std::isnan(out.positionMod) && !std::isinf(out.positionMod));
        }

        std::cout << "PASSED" << std::endl;
    }

    // 5. Test EffectsEngine
    {
        std::cout << "[Test 5] EffectsEngine (SVF Filter, Delay, Reverb, Saturation)... ";
        GranularLab::EffectsEngine fx;
        fx.prepare(sampleRate, blockSize);

        std::vector<float> audioL(blockSize, 0.4f);
        std::vector<float> audioR(blockSize, -0.4f);
        float* channels[2] = { audioL.data(), audioR.data() };

        GranularLab::EffectsEngine::Parameters fxParams;
        fxParams.filterEnabled = true;
        fxParams.filterType = GranularLab::FilterType::LowPass;
        fxParams.filterCutoff = 3000.0f;
        fxParams.filterResonance = 1.5f;

        fxParams.delayEnabled = true;
        fxParams.delayTimeMs = 150.0f;
        fxParams.delayFeedback = 0.4f;
        fxParams.delayMix = 0.5f;

        fxParams.reverbEnabled = true;
        fxParams.reverbSize = 0.7f;
        fxParams.reverbMix = 0.4f;

        fxParams.satEnabled = true;
        fxParams.satDrive = 2.5f;
        fxParams.satMix = 0.6f;

        for (int b = 0; b < 20; ++b)
        {
            fx.processBlock(channels, blockSize, fxParams);
            for (int s = 0; s < blockSize; ++s)
            {
                assert(!std::isnan(audioL[s]) && !std::isinf(audioL[s]));
                assert(!std::isnan(audioR[s]) && !std::isinf(audioR[s]));
            }
        }

        std::cout << "PASSED" << std::endl;
    }

    std::cout << "\nALL DSP UNIT TESTS PASSED SUCCESSFULLY!" << std::endl;
    return 0;
}
