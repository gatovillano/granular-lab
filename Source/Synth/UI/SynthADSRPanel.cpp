#include "SynthADSRPanel.h"
#include "../SynthProcessor.h"
#include <juce_graphics/juce_graphics.h>

namespace GranularSynthLab
{
    SynthADSRPanel::SynthADSRPanel(SynthProcessor& p)
        : processor(p)
    {
        auto setupSlider = [this](juce::Slider& s, juce::Label& l, const juce::String& text, const juce::String& suffix)
        {
            s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 16);
            s.setTextValueSuffix(suffix);
            addAndMakeVisible(s);

            l.setText(text, juce::dontSendNotification);
            l.setFont(juce::FontOptions(11.0f, juce::Font::bold));
            l.setJustificationType(juce::Justification::centred);
            l.setColour(juce::Label::textColourId, juce::Colour(0xbbffffff));
            addAndMakeVisible(l);

            s.onValueChange = [this]() { repaint(); };
        };

        setupSlider(attackSlider, attackLabel, "ATTACK", " ms");
        setupSlider(decaySlider, decayLabel, "DECAY", " ms");
        setupSlider(sustainSlider, sustainLabel, "SUSTAIN", "");
        setupSlider(releaseSlider, releaseLabel, "RELEASE", " ms");

        attackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.getAPVTS(), ParamIDs::attack, attackSlider);
        decayAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.getAPVTS(), ParamIDs::decay, decaySlider);
        sustainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.getAPVTS(), ParamIDs::sustain, sustainSlider);
        releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.getAPVTS(), ParamIDs::release, releaseSlider);
    }

    void SynthADSRPanel::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xff12141a));
        g.fillRoundedRectangle(bounds, 6.0f);
        g.setColour(juce::Colour(0xff222633));
        g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

        // Header
        g.setColour(juce::Colour(0x88ffffff));
        g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        g.drawText("AMP ENVELOPE", 10, 6, 120, 14, juce::Justification::left);

        // Visual curve box
        auto curveArea = bounds.removeFromTop(75.0f).reduced(12.0f, 6.0f);
        curveArea.removeFromTop(14.0f); // Label offset

        g.setColour(juce::Colour(0xff0d0e14));
        g.fillRoundedRectangle(curveArea, 4.0f);
        g.setColour(juce::Colour(0x22ffffff));
        g.drawRoundedRectangle(curveArea, 4.0f, 1.0f);

        // Calculate normalized segments
        float att = static_cast<float>(attackSlider.getValue());
        float dec = static_cast<float>(decaySlider.getValue());
        float sus = static_cast<float>(sustainSlider.getValue());
        float rel = static_cast<float>(releaseSlider.getValue());

        float totalTime = att + dec + 500.0f + rel;
        float w = curveArea.getWidth();
        float h = curveArea.getHeight() - 6.0f;
        float baseLine = curveArea.getBottom() - 3.0f;

        float attW = (att / totalTime) * w;
        float decW = (dec / totalTime) * w;
        float susW = (500.0f / totalTime) * w;
        float relW = (rel / totalTime) * w;

        float x0 = curveArea.getX();
        float x1 = x0 + attW;
        float y1 = baseLine - h;
        float x2 = x1 + decW;
        float y2 = baseLine - h * sus;
        float x3 = x2 + susW;
        float y3 = y2;
        float x4 = std::min(curveArea.getRight(), x3 + relW);
        float y4 = baseLine;

        juce::Path envelopePath;
        envelopePath.startNewSubPath(x0, baseLine);
        envelopePath.lineTo(x1, y1);
        envelopePath.lineTo(x2, y2);
        envelopePath.lineTo(x3, y3);
        envelopePath.lineTo(x4, y4);

        // Fill under curve
        juce::Path fillPath = envelopePath;
        fillPath.lineTo(x4, baseLine);
        fillPath.closeSubPath();

        juce::ColourGradient fillGrad(juce::Colour(0x339d4edd), 0, curveArea.getY(),
                                     juce::Colour(0x0500d2ff), 0, baseLine, false);
        g.setGradientFill(fillGrad);
        g.fillPath(fillPath);

        // Outline
        juce::ColourGradient strokeGrad(juce::Colour(0xff00d2ff), x0, curveArea.getCentreY(),
                                       juce::Colour(0xffb5179e), x4, curveArea.getCentreY(), false);
        g.setGradientFill(strokeGrad);
        g.strokePath(envelopePath, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved));
    }

    void SynthADSRPanel::resized()
    {
        auto area = getLocalBounds().reduced(8, 6);
        area.removeFromTop(75); // Space for curve box

        int knobW = area.getWidth() / 4;

        auto setupCol = [&](juce::Slider& s, juce::Label& l, juce::Rectangle<int> col)
        {
            l.setBounds(col.removeFromTop(16));
            s.setBounds(col);
        };

        setupCol(attackSlider, attackLabel, area.removeFromLeft(knobW));
        setupCol(decaySlider, decayLabel, area.removeFromLeft(knobW));
        setupCol(sustainSlider, sustainLabel, area.removeFromLeft(knobW));
        setupCol(releaseSlider, releaseLabel, area);
    }
}
