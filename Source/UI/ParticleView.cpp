#include "ParticleView.h"

namespace GranularLab
{
    ParticleView::ParticleView(GranularEngine& engine)
        : granularEngine(engine)
    {
        startTimerHz(60);
    }

    ParticleView::~ParticleView()
    {
        stopTimer();
    }

    void ParticleView::timerCallback()
    {
        if (granularEngine.popVisualSnapshot(currentSnapshot))
        {
            repaint();
        }
    }

    void ParticleView::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();

        // Background
        g.setColour(CustomLookAndFeel::bgCard());
        g.fillRoundedRectangle(bounds, 6.0f);

        g.setColour(CustomLookAndFeel::border());
        g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

        // Center stereo divider line
        float midY = bounds.getCentreY();
        g.setColour(CustomLookAndFeel::border().withAlpha(0.25f));
        g.drawHorizontalLine(static_cast<int>(midY), bounds.getX() + 4.0f, bounds.getRight() - 4.0f);

        // Draw grains
        size_t count = std::min(currentSnapshot.activeCount, MAX_VISUAL_GRAINS);
        for (size_t i = 0; i < count; ++i)
        {
            const auto& gr = currentSnapshot.grains[i];
            if (!gr.active) continue;

            float x = bounds.getX() + gr.normPos * bounds.getWidth();
            // Pan maps from -1 (L, top) to +1 (R, bottom)
            float y = midY + gr.pan * (bounds.getHeight() * 0.4f);

            // Radius scaled by duration
            float radius = std::clamp(gr.sizeMs * 0.05f + 3.0f, 3.0f, 16.0f);

            // Opacity scaled by amplitude envelope
            float alpha = std::clamp(gr.amplitude * 1.5f, 0.05f, 1.0f);

            // Dynamic color based on pitch shift and reverse
            juce::Colour particleColour;
            if (gr.reverse)
            {
                // Green / mint for reversed grains
                particleColour = juce::Colour(0xff00f5d4);
            }
            else
            {
                // Pitch coloring: neutral is Cyan, high is Violet/Magenta, low is Amber/Orange
                if (gr.pitchShift > 0.5f)
                {
                    float t = std::clamp(gr.pitchShift / 24.0f, 0.0f, 1.0f);
                    particleColour = CustomLookAndFeel::accentCyan().interpolatedWith(CustomLookAndFeel::accentViolet(), t);
                }
                else if (gr.pitchShift < -0.5f)
                {
                    float t = std::clamp(-gr.pitchShift / 24.0f, 0.0f, 1.0f);
                    particleColour = CustomLookAndFeel::accentCyan().interpolatedWith(juce::Colour(0xffff9e00), t);
                }
                else
                {
                    particleColour = CustomLookAndFeel::accentCyan();
                }
            }

            // Glow aura
            g.setColour(particleColour.withAlpha(alpha * 0.25f));
            g.fillEllipse(x - radius * 1.8f, y - radius * 1.8f, radius * 3.6f, radius * 3.6f);

            // Core
            g.setColour(particleColour.withAlpha(alpha));
            g.fillEllipse(x - radius, y - radius, radius * 2.0f, radius * 2.0f);

            // Center hot spot
            g.setColour(juce::Colours::white.withAlpha(alpha * 0.8f));
            g.fillEllipse(x - radius * 0.3f, y - radius * 0.3f, radius * 0.6f, radius * 0.6f);
        }

        // Header label
        g.setColour(CustomLookAndFeel::textDim());
        g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        juce::String title = "GRAIN PARTICLES (" + juce::String(currentSnapshot.activeCount) + " ACTIVE)";
        g.drawText(title, bounds.reduced(8.0f, 4.0f), juce::Justification::topLeft, true);
    }
}
