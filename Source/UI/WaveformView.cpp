#include "WaveformView.h"
#include "../Common/Parameters.h"

namespace GranularLab
{
    WaveformView::WaveformView(const CircularAudioBuffer& buf, juce::AudioProcessorValueTreeState& state)
        : buffer(buf), apvts(state)
    {
        startTimerHz(30);
    }

    WaveformView::~WaveformView()
    {
        stopTimer();
    }

    void WaveformView::timerCallback()
    {
        buffer.getWaveformSnapshot(minL, maxL, minR, maxR);
        currentWritePos = buffer.getNormalizedWritePos();
        isFrozen = buffer.getFrozen();
        repaint();
    }

    void WaveformView::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();

        // Background
        g.setColour(CustomLookAndFeel::bgCard());
        g.fillRoundedRectangle(bounds, 6.0f);

        // Border (glowing cyan if frozen)
        if (isFrozen)
        {
            g.setColour(CustomLookAndFeel::accentCyan());
            g.drawRoundedRectangle(bounds, 6.0f, 1.8f);
        }
        else
        {
            g.setColour(CustomLookAndFeel::border());
            g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
        }

        // Center line
        float midY = bounds.getCentreY();
        g.setColour(CustomLookAndFeel::border().withAlpha(0.4f));
        g.drawHorizontalLine(static_cast<int>(midY), bounds.getX() + 4.0f, bounds.getRight() - 4.0f);

        // Grain Read Region Highlight
        float normPos = 0.5f;
        float spray = 0.15f;
        if (auto* p = apvts.getRawParameterValue(ParamIDs::position))
            normPos = p->load();
        if (auto* p = apvts.getRawParameterValue(ParamIDs::spray))
            spray = p->load();

        // Grain spawn read position relative to write position
        float readNorm = currentWritePos - normPos;
        while (readNorm < 0.0f) readNorm += 1.0f;
        while (readNorm >= 1.0f) readNorm -= 1.0f;

        float sprayWidthNorm = std::max(0.02f, spray);
        float regionX = bounds.getX() + (readNorm - sprayWidthNorm * 0.5f) * bounds.getWidth();
        float regionW = sprayWidthNorm * bounds.getWidth();

        // Shaded spray span
        g.setColour(CustomLookAndFeel::accentViolet().withAlpha(0.18f));
        g.fillRect(regionX, bounds.getY() + 2.0f, regionW, bounds.getHeight() - 4.0f);

        g.setColour(CustomLookAndFeel::accentViolet().withAlpha(0.6f));
        float centerScanX = bounds.getX() + readNorm * bounds.getWidth();
        g.drawVerticalLine(static_cast<int>(centerScanX), bounds.getY() + 4.0f, bounds.getBottom() - 4.0f);

        // Render stereo waveform
        if (!minL.empty())
        {
            size_t n = minL.size();
            float halfH = (bounds.getHeight() - 8.0f) * 0.45f;
            float stepX = bounds.getWidth() / static_cast<float>(n);

            juce::Path wavePathL;
            juce::Path wavePathR;

            for (size_t i = 0; i < n; ++i)
            {
                float x = bounds.getX() + i * stepX;
                float yTopL = midY - maxL[i] * halfH;
                float yBotL = midY - minL[i] * halfH;
                if (i == 0) wavePathL.startNewSubPath(x, yTopL);
                wavePathL.lineTo(x, yTopL);
                wavePathL.lineTo(x, yBotL);

                float yTopR = midY + minR[i] * halfH;
                float yBotR = midY + maxR[i] * halfH;
                if (i == 0) wavePathR.startNewSubPath(x, yTopR);
                wavePathR.lineTo(x, yTopR);
                wavePathR.lineTo(x, yBotR);
            }

            g.setColour(CustomLookAndFeel::accentCyan().withAlpha(0.7f));
            g.strokePath(wavePathL, juce::PathStrokeType(1.2f));

            g.setColour(CustomLookAndFeel::accentBlue().withAlpha(0.6f));
            g.strokePath(wavePathR, juce::PathStrokeType(1.2f));
        }

        // Write head line (yellow-white)
        float writeX = bounds.getX() + currentWritePos * bounds.getWidth();
        g.setColour(juce::Colours::white.withAlpha(0.85f));
        g.drawVerticalLine(static_cast<int>(writeX), bounds.getY() + 2.0f, bounds.getBottom() - 2.0f);

        // Header label
        g.setColour(CustomLookAndFeel::textDim());
        g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        g.drawText("BUFFER WAVEFORM (10s)", bounds.reduced(8.0f, 4.0f), juce::Justification::topLeft, true);

        if (isFrozen)
        {
            g.setColour(CustomLookAndFeel::accentCyan());
            g.drawText("[ FROZEN ]", bounds.reduced(8.0f, 4.0f), juce::Justification::topRight, true);
        }
    }
}
