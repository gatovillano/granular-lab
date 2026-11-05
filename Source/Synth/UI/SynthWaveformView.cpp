#include "SynthWaveformView.h"
#include "../SynthProcessor.h"
#include <juce_graphics/juce_graphics.h>

namespace GranularSynthLab
{
    SynthWaveformView::SynthWaveformView(SynthProcessor& p)
        : processor(p)
    {
        minPeaksL.assign(512, 0.0f);
        maxPeaksL.assign(512, 0.0f);
        minPeaksR.assign(512, 0.0f);
        maxPeaksR.assign(512, 0.0f);
    }

    void SynthWaveformView::resized()
    {
    }

    void SynthWaveformView::refresh()
    {
        processor.getSynthEngine().getSampleBuffer().getWaveformSnapshot(minPeaksL, maxPeaksL, minPeaksR, maxPeaksR);
        repaint();
    }

    void SynthWaveformView::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();

        // 1. Dark container background
        g.setColour(juce::Colour(0xff12141a));
        g.fillRoundedRectangle(bounds, 6.0f);

        // Border
        if (isDragOver)
        {
            g.setColour(juce::Colour(0xff00e5ff));
            const float dashes[] = { 6.0f, 4.0f };
            g.drawDashedLine(juce::Line<float>(bounds.getX(), bounds.getY(), bounds.getRight(), bounds.getY()), dashes, 2, 2.0f);
            g.drawDashedLine(juce::Line<float>(bounds.getRight(), bounds.getY(), bounds.getRight(), bounds.getBottom()), dashes, 2, 2.0f);
            g.drawDashedLine(juce::Line<float>(bounds.getRight(), bounds.getBottom(), bounds.getX(), bounds.getBottom()), dashes, 2, 2.0f);
            g.drawDashedLine(juce::Line<float>(bounds.getX(), bounds.getBottom(), bounds.getX(), bounds.getY()), dashes, 2, 2.0f);
        }
        else
        {
            g.setColour(juce::Colour(0xff222633));
            g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
        }

        const float midY = bounds.getCentreY();
        const float height = bounds.getHeight();
        const float width = bounds.getWidth();

        // Subtle center line
        g.setColour(juce::Colour(0x18ffffff));
        g.drawHorizontalLine(static_cast<int>(midY), bounds.getX(), bounds.getRight());

        // 2. Draw Waveform
        const size_t numPoints = minPeaksL.size();
        if (numPoints > 0 && width > 10.0f)
        {
            juce::Path wavePathL, wavePathR;

            float xStep = width / static_cast<float>(numPoints);
            float halfH = (height * 0.45f);

            // Channel L (top half)
            float chMidYL = midY - halfH * 0.5f;
            for (size_t i = 0; i < numPoints; ++i)
            {
                float x = bounds.getX() + static_cast<float>(i) * xStep;
                float topY = chMidYL - maxPeaksL[i] * halfH * 0.45f;
                float botY = chMidYL - minPeaksL[i] * halfH * 0.45f;

                if (i == 0)
                    wavePathL.startNewSubPath(x, topY);
                else
                    wavePathL.lineTo(x, topY);

                wavePathL.lineTo(x, botY);
            }

            // Channel R (bottom half)
            float chMidYR = midY + halfH * 0.5f;
            for (size_t i = 0; i < numPoints; ++i)
            {
                float x = bounds.getX() + static_cast<float>(i) * xStep;
                float topY = chMidYR - maxPeaksR[i] * halfH * 0.45f;
                float botY = chMidYR - minPeaksR[i] * halfH * 0.45f;

                if (i == 0)
                    wavePathR.startNewSubPath(x, topY);
                else
                    wavePathR.lineTo(x, topY);

                wavePathR.lineTo(x, botY);
            }

            // Draw with neon gradient
            juce::ColourGradient grad(juce::Colour(0xff00d2ff), bounds.getX(), midY,
                                      juce::Colour(0xffb5179e), bounds.getRight(), midY, false);
            g.setGradientFill(grad);
            g.strokePath(wavePathL, juce::PathStrokeType(1.2f));
            g.strokePath(wavePathR, juce::PathStrokeType(1.2f));
        }

        // 3. Spray Region overlay
        float posNorm = processor.getAPVTS().getRawParameterValue(ParamIDs::position)->load();
        float sprayNorm = processor.getAPVTS().getRawParameterValue(ParamIDs::spray)->load();

        float sprayLeft = std::clamp(posNorm - sprayNorm * 0.5f, 0.0f, 1.0f) * width;
        float sprayRight = std::clamp(posNorm + sprayNorm * 0.5f, 0.0f, 1.0f) * width;
        float sprayW = std::max(2.0f, sprayRight - sprayLeft);

        g.setColour(juce::Colour(0x1fffffff));
        g.fillRect(bounds.getX() + sprayLeft, bounds.getY() + 2.0f, sprayW, height - 4.0f);

        g.setColour(juce::Colour(0x3500d2ff));
        g.drawRect(bounds.getX() + sprayLeft, bounds.getY() + 2.0f, sprayW, height - 4.0f, 1.0f);

        // 4. Position Playhead Bar
        float posX = bounds.getX() + posNorm * width;
        g.setColour(juce::Colour(0xff00ffff));
        g.drawLine(posX, bounds.getY() + 2.0f, posX, bounds.getBottom() - 2.0f, 2.0f);

        // Playhead handle
        juce::Path handle;
        handle.addTriangle(posX - 5.0f, bounds.getY() + 2.0f, posX + 5.0f, bounds.getY() + 2.0f, posX, bounds.getY() + 10.0f);
        g.setColour(juce::Colour(0xff00ffff));
        g.fillPath(handle);

        // 5. Title & Info text
        g.setColour(juce::Colour(0x99ffffff));
        g.setFont(juce::FontOptions(11.0f));

        std::string sourceTitle = processor.getSynthEngine().getSampleBuffer().getSourceTitle();
        double sampleRate = processor.getSynthEngine().getSampleBuffer().getBufferSampleRate();
        double durationSec = static_cast<double>(processor.getSynthEngine().getSampleBuffer().getLengthSamples()) / sampleRate;

        juce::String infoText = juce::String(sourceTitle) + " | " + juce::String(durationSec, 2) + "s @ " + juce::String(sampleRate * 0.001, 1) + "kHz";
        g.drawText(infoText, bounds.reduced(8.0f), juce::Justification::bottomLeft);

        if (isDragOver)
        {
            g.setColour(juce::Colour(0xdd000000));
            g.fillRect(bounds);
            g.setColour(juce::Colour(0xff00e5ff));
            g.setFont(juce::FontOptions(14.0f, juce::Font::bold));
            g.drawText("Soltar archivo de audio aquí (.wav, .flac, .aiff, .ogg)", bounds, juce::Justification::centred);
        }
    }

    void SynthWaveformView::updatePositionFromMouse(float mouseX)
    {
        float norm = std::clamp(mouseX / getWidth(), 0.0f, 1.0f);
        if (auto* param = processor.getAPVTS().getParameter(ParamIDs::position))
        {
            param->setValueNotifyingHost(norm);
            repaint();
        }
    }

    void SynthWaveformView::mouseDown(const juce::MouseEvent& e)
    {
        updatePositionFromMouse(static_cast<float>(e.x));
    }

    void SynthWaveformView::mouseDrag(const juce::MouseEvent& e)
    {
        updatePositionFromMouse(static_cast<float>(e.x));
    }

    bool SynthWaveformView::isInterestedInFileDrag(const juce::StringArray& files)
    {
        for (const auto& file : files)
        {
            auto ext = juce::File(file).getFileExtension().toLowerCase();
            if (ext == ".wav" || ext == ".aiff" || ext == ".aif" || ext == ".flac" || ext == ".ogg" || ext == ".mp3")
                return true;
        }
        return false;
    }

    void SynthWaveformView::fileDragEnter(const juce::StringArray&, int, int)
    {
        isDragOver = true;
        repaint();
    }

    void SynthWaveformView::fileDragExit(const juce::StringArray&)
    {
        isDragOver = false;
        repaint();
    }

    void SynthWaveformView::filesDropped(const juce::StringArray& files, int, int)
    {
        isDragOver = false;
        for (const auto& f : files)
        {
            juce::File audioFile(f);
            if (audioFile.existsAsFile())
            {
                processor.loadSampleFile(audioFile);
                refresh();
                break;
            }
        }
        repaint();
    }
}
