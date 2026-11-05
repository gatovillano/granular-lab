#include "GranularEngine.h"

namespace GranularLab
{
    GranularEngine::GranularEngine()
    {
        for (auto& g : grains)
            g.kill();
    }

    void GranularEngine::prepare(double sampleRate, int maxBlockSize)
    {
        juce::ignoreUnused(maxBlockSize);
        currentSampleRate = sampleRate;
        circularBuffer.prepare(sampleRate, 10.0); // 10 second memory
        spawnAccumulator = 0.0;
        visualCounter = 0;
        visualQueue.reset();

        for (auto& g : grains)
            g.kill();
    }

    void GranularEngine::reset()
    {
        circularBuffer.reset();
        spawnAccumulator = 0.0;
        visualCounter = 0;
        visualQueue.reset();

        for (auto& g : grains)
            g.kill();
    }

    void GranularEngine::spawnGrain(const Parameters& params) noexcept
    {
        // Find an available inactive grain
        Grain* targetGrain = nullptr;
        for (auto& g : grains)
        {
            if (!g.isActive())
            {
                targetGrain = &g;
                break;
            }
        }

        if (targetGrain == nullptr)
            return; // All 128 voices currently busy

        const int bufSize = circularBuffer.getBufferSize();
        if (bufSize <= 0)
            return;

        // Position: normalized offset behind the write head
        // 0.0 = right at write head (recent audio)
        // 1.0 = oldest audio in buffer
        float posNorm = std::clamp(params.position, 0.0f, 1.0f);

        // Add spray jitter
        if (params.spray > 0.0001f)
        {
            float jitter = randomBipolar() * params.spray * 0.5f;
            posNorm = std::clamp(posNorm + jitter, 0.0f, 1.0f);
        }

        double writeHead = static_cast<double>(circularBuffer.getWriteIndex());
        double delayInSamples = posNorm * static_cast<double>(bufSize - 1);
        double startPos = writeHead - delayInSamples;
        while (startPos < 0.0) startPos += bufSize;
        while (startPos >= bufSize) startPos -= bufSize;

        // Grain duration
        float durationMs = std::clamp(params.grainSizeMs, 10.0f, 500.0f);
        double durationSamples = (durationMs * 0.001) * currentSampleRate;

        // Pitch & Pitch Randomization
        float pitchSemitones = params.pitchSemitones;
        if (params.pitchRandSemitones > 0.001f)
        {
            pitchSemitones += randomBipolar() * params.pitchRandSemitones;
        }
        pitchSemitones = std::clamp(pitchSemitones, -36.0f, 36.0f);
        double playbackRate = std::pow(2.0, static_cast<double>(pitchSemitones) / 12.0);

        // Reverse playback
        bool isReverse = false;
        if (params.reverseProb > 0.001f)
        {
            if (randomFloat() < params.reverseProb)
            {
                isReverse = true;
                // For reverse, start at the end of the grain segment so it plays backward into startPos
                startPos += durationSamples * playbackRate;
                while (startPos >= bufSize) startPos -= bufSize;
            }
        }

        // Panning
        float pan = params.pan;
        if (params.panRand > 0.001f)
        {
            pan += randomBipolar() * params.panRand;
        }
        pan = std::clamp(pan, -1.0f, 1.0f);

        // Voice gain scaling according to active voice count (equal energy normalization)
        float voiceGain = 1.0f / std::sqrt(std::max(1.0f, params.densityHz * 0.35f));

        targetGrain->init(startPos, durationSamples, playbackRate, pan,
                          params.envelope, isReverse, voiceGain, durationMs, pitchSemitones, posNorm);
    }

    void GranularEngine::emitVisualSnapshot() noexcept
    {
        VisualSnapshot snap;
        snap.activeCount = 0;
        snap.writePosNorm = circularBuffer.getNormalizedWritePos();
        snap.isFrozen = circularBuffer.getFrozen();

        for (size_t i = 0; i < grains.size() && snap.activeCount < MAX_VISUAL_GRAINS; ++i)
        {
            const auto& g = grains[i];
            if (g.isActive())
            {
                auto& vi = snap.grains[snap.activeCount++];
                vi.active = true;
                vi.normPos = g.getNormalizedPos();
                vi.lifeProgress = g.getLifeProgress();
                vi.pan = g.getPan();
                vi.pitchShift = g.getPitchShift();
                vi.amplitude = g.getCurrentAmplitude();
                vi.sizeMs = g.getDurationMs();
                vi.reverse = g.getIsReverse();
            }
        }

        visualQueue.push(snap);
    }

    void GranularEngine::processBlock(const float* const* inputChannels,
                                      float* const* outputChannels,
                                      int numSamples,
                                      const Parameters& params) noexcept
    {
        circularBuffer.setFrozen(params.freeze);

        // Density interval in samples
        float safeDensity = std::clamp(params.densityHz, 1.0f, 100.0f);
        double spawnInterval = currentSampleRate / static_cast<double>(safeDensity);

        const float* inL = inputChannels[0];
        const float* inR = inputChannels[1] ? inputChannels[1] : inL;
        float* outL = outputChannels[0];
        float* outR = outputChannels[1];

        for (int i = 0; i < numSamples; ++i)
        {
            float wetL = 0.0f;
            float wetR = 0.0f;

            // Render all active grains
            for (auto& g : grains)
            {
                if (g.isActive())
                {
                    g.processSample(circularBuffer, wetL, wetR);
                }
            }

            // Write input + feedback into circular buffer
            circularBuffer.write(inL[i], inR[i], params.feedback, wetL, wetR);

            outL[i] = wetL;
            outR[i] = wetR;

            // Grain spawning
            spawnAccumulator += 1.0;
            if (spawnAccumulator >= spawnInterval)
            {
                spawnAccumulator -= spawnInterval;
                spawnGrain(params);
            }
        }

        // Send visual snapshot periodically (~60 FPS)
        visualCounter += numSamples;
        if (visualCounter >= static_cast<int>(currentSampleRate / 60.0))
        {
            visualCounter = 0;
            emitVisualSnapshot();
        }
    }
}
