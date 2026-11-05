#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include "../Source/Synth/DSP/SampleBuffer.h"
#include "../Source/Synth/DSP/SynthGrain.h"
#include "../Source/Synth/DSP/SynthVoice.h"
#include "../Source/Synth/DSP/SynthEngine.h"
#include "../Source/DSP/EffectsEngine.h"

int main()
{
    std::cout << "=== GRANULAR SYNTH LAB DSP TESTS ===" << std::endl;

    constexpr double sampleRate = 48000.0;
    constexpr int blockSize = 256;

    // 1. Test SampleBuffer oscillator generation
    {
        std::cout << "[Test 1] SampleBuffer synthesis (7 shapes) & Hermite read... ";
        GranularSynthLab::SampleBuffer sbuf;
        sbuf.prepare(sampleRate);

        for (int s = 0; s < static_cast<int>(GranularSynthLab::OscShape::NumShapes); ++s)
        {
            sbuf.generateOscillator(static_cast<GranularSynthLab::OscShape>(s));
            assert(sbuf.getLengthSamples() > 0);
            assert(!sbuf.getSourceTitle().empty());

            // Hermite cubic interpolation read
            float valL = sbuf.readHermite(0, 500.35);
            float valR = sbuf.readHermite(1, 500.35);
            assert(!std::isnan(valL) && !std::isinf(valL));
            assert(!std::isnan(valR) && !std::isinf(valR));
        }
        std::cout << "PASSED" << std::endl;
    }

    // 2. Test SynthGrain lifecycle
    {
        std::cout << "[Test 2] SynthGrain lifecycle and envelopes... ";
        GranularSynthLab::SampleBuffer sbuf;
        sbuf.prepare(sampleRate);
        sbuf.generateOscillator(GranularSynthLab::OscShape::Supersaw);

        GranularSynthLab::SynthGrain grain;
        grain.init(100.0, 480.0, 1.0, 0.0f, GranularLab::EnvelopeShape::Hann, false, 1.0f, 10.0f, 0.0f, 0.1f);
        assert(grain.isActive());

        float outL = 0.0f, outR = 0.0f;
        int count = 0;
        while (grain.isActive() && count < 1000)
        {
            grain.processSample(sbuf, outL, outR);
            ++count;
        }

        assert(!grain.isActive());
        assert(count == 480);
        std::cout << "PASSED" << std::endl;
    }

    // 3. Test SynthVoice with MIDI NoteOn and ADSR
    {
        std::cout << "[Test 3] SynthVoice noteOn, ADSR envelope & grain spawning... ";
        GranularSynthLab::SampleBuffer sbuf;
        sbuf.prepare(sampleRate);
        sbuf.generateOscillator(GranularSynthLab::OscShape::Supersaw);

        GranularSynthLab::SynthVoice voice;
        voice.prepare(sampleRate);

        GranularSynthLab::VoiceParams params;
        params.sampleRate = sampleRate;
        params.rootNote = 60;
        params.densityHz = 50.0f;
        params.grainSizeMs = 40.0f;
        params.adsrParams.attack = 0.005f;  // 5ms
        params.adsrParams.decay = 0.05f;    // 50ms
        params.adsrParams.sustain = 0.7f;
        params.adsrParams.release = 0.05f;  // 50ms

        voice.noteOn(60, 0.8f, params);
        assert(voice.isActive());
        assert(voice.getMidiNote() == 60);

        std::vector<float> bufL(blockSize, 0.0f);
        std::vector<float> bufR(blockSize, 0.0f);

        // Process a block
        voice.renderNextBlock(sbuf, bufL.data(), bufR.data(), blockSize, params);

        float peak = 0.0f;
        for (int i = 0; i < blockSize; ++i)
        {
            assert(!std::isnan(bufL[i]) && !std::isinf(bufL[i]));
            peak = std::max(peak, std::abs(bufL[i]));
        }
        assert(peak > 0.0001f); // Voice produced sound

        // Test NoteOff
        voice.noteOff(0.0f, true);
        // Process through release
        for (int b = 0; b < 20; ++b)
        {
            std::fill(bufL.begin(), bufL.end(), 0.0f);
            std::fill(bufR.begin(), bufR.end(), 0.0f);
            voice.renderNextBlock(sbuf, bufL.data(), bufR.data(), blockSize, params);
        }
        assert(!voice.isActive()); // Completed release
        std::cout << "PASSED" << std::endl;
    }

    // 4. Test Polyphonic SynthEngine (Chord test)
    {
        std::cout << "[Test 4] SynthEngine polyphonic chord (4 voices) & pitch wheel... ";
        GranularSynthLab::SynthEngine engine;
        engine.prepare(sampleRate, blockSize);

        GranularSynthLab::VoiceParams params;
        params.sampleRate = sampleRate;
        params.rootNote = 60;
        params.adsrParams.attack = 0.01f;
        params.adsrParams.decay = 0.1f;
        params.adsrParams.sustain = 0.8f;
        params.adsrParams.release = 0.2f;

        // Play C Major chord: C4 (60), E4 (64), G4 (67), B4 (71)
        juce::MidiBuffer midi;
        midi.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);
        midi.addEvent(juce::MidiMessage::noteOn(1, 64, 0.8f), 0);
        midi.addEvent(juce::MidiMessage::noteOn(1, 67, 0.8f), 0);
        midi.addEvent(juce::MidiMessage::noteOn(1, 71, 0.8f), 0);

        engine.processMidi(midi, params);
        assert(engine.getActiveVoiceCount() == 4);

        std::vector<float> outL(blockSize, 0.0f);
        std::vector<float> outR(blockSize, 0.0f);
        engine.renderVoices(outL.data(), outR.data(), blockSize, params);

        float chordPeak = 0.0f;
        for (int i = 0; i < blockSize; ++i)
        {
            assert(!std::isnan(outL[i]) && !std::isinf(outL[i]));
            chordPeak = std::max(chordPeak, std::abs(outL[i]));
        }
        assert(chordPeak > 0.001f);

        // Test Pitch Wheel
        juce::MidiBuffer wheelMidi;
        wheelMidi.addEvent(juce::MidiMessage::pitchWheel(1, 12000), 0);
        engine.processMidi(wheelMidi, params);

        // Release chord
        juce::MidiBuffer offMidi;
        offMidi.addEvent(juce::MidiMessage::noteOff(1, 60, 0.0f), 0);
        offMidi.addEvent(juce::MidiMessage::noteOff(1, 64, 0.0f), 0);
        offMidi.addEvent(juce::MidiMessage::noteOff(1, 67, 0.0f), 0);
        offMidi.addEvent(juce::MidiMessage::noteOff(1, 71, 0.0f), 0);
        engine.processMidi(offMidi, params);

        std::cout << "PASSED" << std::endl;
    }

    std::cout << "ALL GRANULAR SYNTH LAB DSP TESTS PASSED!" << std::endl;
    return 0;
}
