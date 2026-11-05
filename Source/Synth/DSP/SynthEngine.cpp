#include "SynthEngine.h"
#include <algorithm>

namespace GranularSynthLab
{
    SynthEngine::SynthEngine()
    {
    }

    void SynthEngine::prepare(double sampleRate, int maxBlockSize)
    {
        currentSampleRate = (sampleRate > 8000.0) ? sampleRate : 44100.0;
        sampleBuffer.prepare(currentSampleRate);

        for (auto& v : voices)
            v.prepare(currentSampleRate);

        voiceBufferL.assign(static_cast<size_t>(maxBlockSize), 0.0f);
        voiceBufferR.assign(static_cast<size_t>(maxBlockSize), 0.0f);
        sustainPedal = false;
        sustainedNotes.clear();
    }

    void SynthEngine::reset()
    {
        for (auto& v : voices)
            v.kill();

        sustainPedal = false;
        sustainedNotes.clear();
    }

    SynthVoice* SynthEngine::findFreeVoice()
    {
        // 1. Look for an inactive voice
        for (auto& v : voices)
        {
            if (!v.isActive())
                return &v;
        }

        // 2. Otherwise steal the oldest voice
        uint32_t oldestAge = 0;
        SynthVoice* oldest = &voices[0];
        for (auto& v : voices)
        {
            if (v.getAge() > oldestAge)
            {
                oldestAge = v.getAge();
                oldest = &v;
            }
        }
        return oldest;
    }

    SynthVoice* SynthEngine::findVoicePlayingNote(int midiNote)
    {
        for (auto& v : voices)
        {
            if (v.isActive() && v.getMidiNote() == midiNote)
                return &v;
        }
        return nullptr;
    }

    void SynthEngine::processMidi(const juce::MidiBuffer& midiMessages, const VoiceParams& params)
    {
        for (const auto metadata : midiMessages)
        {
            const auto msg = metadata.getMessage();

            if (msg.isNoteOn())
            {
                int note = msg.getNoteNumber();
                float vel = msg.getFloatVelocity();

                // If already playing note, retrigger
                SynthVoice* v = findVoicePlayingNote(note);
                if (v == nullptr)
                    v = findFreeVoice();

                if (v != nullptr)
                {
                    v->pitchWheel(currentPitchBendSemis);
                    v->noteOn(note, vel, params);
                }
            }
            else if (msg.isNoteOff())
            {
                int note = msg.getNoteNumber();
                float vel = msg.getFloatVelocity();

                if (sustainPedal)
                {
                    sustainedNotes.push_back(note);
                }
                else
                {
                    SynthVoice* v = findVoicePlayingNote(note);
                    if (v != nullptr)
                    {
                        v->noteOff(vel, true);
                    }
                }
            }
            else if (msg.isPitchWheel())
            {
                // Pitch wheel value 0..16383, 8192 is center
                int wheel = msg.getPitchWheelValue();
                float norm = static_cast<float>(wheel - 8192) / 8192.0f;
                currentPitchBendSemis = norm * static_cast<float>(params.pitchBendSemis);

                for (auto& v : voices)
                {
                    if (v.isActive())
                        v.pitchWheel(currentPitchBendSemis);
                }
            }
            else if (msg.isController() && msg.getControllerNumber() == 64) // Sustain pedal
            {
                bool down = (msg.getControllerValue() >= 64);
                if (!down && sustainPedal)
                {
                    // Release sustained notes
                    for (int note : sustainedNotes)
                    {
                        SynthVoice* v = findVoicePlayingNote(note);
                        if (v != nullptr)
                            v->noteOff(0.0f, true);
                    }
                    sustainedNotes.clear();
                }
                sustainPedal = down;
            }
            else if (msg.isAllNotesOff() || msg.isAllSoundOff())
            {
                reset();
            }
        }
    }

    void SynthEngine::renderVoices(float* outL, float* outR, int numSamples, const VoiceParams& params) noexcept
    {
        if (numSamples <= 0)
            return;

        for (auto& v : voices)
        {
            if (v.isActive())
            {
                v.renderNextBlock(sampleBuffer, outL, outR, numSamples, params);
            }
        }
    }

    int SynthEngine::getActiveVoiceCount() const noexcept
    {
        int count = 0;
        for (const auto& v : voices)
        {
            if (v.isActive())
                ++count;
        }
        return count;
    }

    void SynthEngine::pushVisualSnapshot(GranularLab::SPSCQueue<GranularLab::VisualSnapshot, 8>& visualQueue)
    {
        GranularLab::VisualSnapshot snapshot;
        snapshot.activeCount = 0;
        snapshot.writePosNorm = 0.0f;
        snapshot.isFrozen = false;

        size_t totalGrains = 0;
        for (const auto& v : voices)
        {
            if (v.isActive() && totalGrains < GranularLab::MAX_VISUAL_GRAINS)
            {
                size_t added = v.collectVisualGrains(snapshot.grains.data() + totalGrains,
                                                     GranularLab::MAX_VISUAL_GRAINS - totalGrains);
                totalGrains += added;
            }
        }

        snapshot.activeCount = totalGrains;
        visualQueue.push(snapshot);
    }
}
