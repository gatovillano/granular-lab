#include "SynthParticleView.h"
#include "../SynthProcessor.h"

namespace GranularSynthLab
{
    SynthParticleView::SynthParticleView(SynthProcessor& p)
        : processor(p)
    {
        startTimerHz(60);
    }

    SynthParticleView::~SynthParticleView()
    {
        stopTimer();
    }

    void SynthParticleView::timerCallback()
    {
        GranularLab::VisualSnapshot snap;
        bool hasNew = false;
        while (processor.getVisualQueue().pop(snap))
        {
            currentSnapshot = snap;
            hasNew = true;
        }

        if (hasNew)
            repaint();
    }

    void SynthParticleView::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();

        // Background
        g.setColour(GranularLab::CustomLookAndFeel::bgCard());
        g.fillRoundedRectangle(bounds, 6.0f);

        g.setColour(GranularLab::CustomLookAndFeel::border());
        g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

        float midY = bounds.getCentreY();
        g.setColour(GranularLab::CustomLookAndFeel::border().withAlpha(0.25f));
        g.drawHorizontalLine(static_cast<int>(midY), bounds.getX() + 4.0f, bounds.getRight() - 4.0f);

        // Header label
        g.setColour(juce::Colour(0x55ffffff));
        g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        g.drawText("PARTICLES", 10, 6, 80, 14, juce::Justification::left);

        // Active voice count badge
        int activeVoices = processor.getSynthEngine().getActiveVoiceCount();
        juce::String voiceStr = juce::String(activeVoices) + " VOICES | " + juce::String(currentSnapshot.activeCount) + " GRAINS";
        g.drawText(voiceStr, bounds.reduced(10.0f, 6.0f), juce::Justification::topRight);

        // Render grains
        size_t count = std::min(currentSnapshot.activeCount, GranularLab::MAX_VISUAL_GRAINS);
        for (size_t i = 0; i < count; ++i)
        {
            const auto& gr = currentSnapshot.grains[i];
            if (!gr.active) continue;

            float x = bounds.getX() + gr.normPos * bounds.getWidth();
            float y = midY + gr.pan * (bounds.getHeight() * 0.4f);

            float radius = std::clamp(gr.sizeMs * 0.05f + 3.0f, 3.0f, 15.0f);
            float alpha = std::clamp(gr.amplitude * 1.5f, 0.05f, 1.0f);

            juce::Colour particleColour;
            if (gr.reverse)
            {
                particleColour = juce::Colour(0xffff007f); // Neon pink for reverse
            }
            else
            {
                // Dynamic hue based on pitch shift
                float hue = 0.55f + gr.pitchShift * 0.02f; // Base cyan/violet
                while (hue < 0.0f) hue += 1.0f;
                while (hue > 1.0f) hue -= 1.0f;
                particleColour = juce::Colour::fromHSV(hue, 0.85f, 1.0f, 1.0f);
            }

            // Glow outer halo
            g.setColour(particleColour.withAlpha(alpha * 0.35f));
            g.fillEllipse(x - radius * 1.5f, y - radius * 1.5f, radius * 3.0f, radius * 3.0f);

            // Core grain dot
            g.setColour(particleColour.withAlpha(alpha));
            g.fillEllipse(x - radius * 0.5f, y - radius * 0.5f, radius, radius);
        }
    }
}
