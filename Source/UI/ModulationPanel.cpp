#include "ModulationPanel.h"

namespace GranularLab
{
    ModulationPanel::ModulationPanel(juce::AudioProcessorValueTreeState& state)
        : apvts(state)
    {
        auto setupKnob = [this](juce::Slider& s, juce::Label& l)
        {
            s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 16);
            l.setFont(juce::FontOptions(11.0f, juce::Font::bold));
            l.setJustificationType(juce::Justification::centred);
            l.setColour(juce::Label::textColourId, CustomLookAndFeel::textDim());
            addAndMakeVisible(s);
            addAndMakeVisible(l);
        };

        auto populateShapes = [](juce::ComboBox& cb)
        {
            cb.addItem("Sine", 1);
            cb.addItem("Triangle", 2);
            cb.addItem("Saw", 3);
            cb.addItem("S & H", 4);
        };

        auto populateTargets = [](juce::ComboBox& cb)
        {
            cb.addItem("None", 1);
            cb.addItem("Position", 2);
            cb.addItem("Grain Size", 3);
            cb.addItem("Density", 4);
            cb.addItem("Pitch", 5);
            cb.addItem("Pan", 6);
            cb.addItem("Cutoff", 7);
        };

        // LFO 1
        setupKnob(lfo1RateSlider, lfo1RateLabel);
        setupKnob(lfo1DepthSlider, lfo1DepthLabel);
        populateShapes(lfo1ShapeCombo);
        populateTargets(lfo1TargetCombo);
        addAndMakeVisible(lfo1ShapeCombo);
        addAndMakeVisible(lfo1TargetCombo);

        lfo1RateAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::lfo1Rate, lfo1RateSlider);
        lfo1DepthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::lfo1Depth, lfo1DepthSlider);
        lfo1ShapeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            apvts, ParamIDs::lfo1Shape, lfo1ShapeCombo);
        lfo1TargetAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            apvts, ParamIDs::lfo1Target, lfo1TargetCombo);

        // LFO 2
        setupKnob(lfo2RateSlider, lfo2RateLabel);
        setupKnob(lfo2DepthSlider, lfo2DepthLabel);
        populateShapes(lfo2ShapeCombo);
        populateTargets(lfo2TargetCombo);
        addAndMakeVisible(lfo2ShapeCombo);
        addAndMakeVisible(lfo2TargetCombo);

        lfo2RateAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::lfo2Rate, lfo2RateSlider);
        lfo2DepthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::lfo2Depth, lfo2DepthSlider);
        lfo2ShapeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            apvts, ParamIDs::lfo2Shape, lfo2ShapeCombo);
        lfo2TargetAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            apvts, ParamIDs::lfo2Target, lfo2TargetCombo);
    }

    void ModulationPanel::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(CustomLookAndFeel::bgCard());
        g.fillRoundedRectangle(bounds, 6.0f);
        g.setColour(CustomLookAndFeel::border());
        g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

        // Center split
        float midX = bounds.getCentreX();
        g.setColour(CustomLookAndFeel::border().withAlpha(0.5f));
        g.drawVerticalLine(static_cast<int>(midX), bounds.getY() + 6.0f, bounds.getBottom() - 6.0f);
    }

    void ModulationPanel::resized()
    {
        auto bounds = getLocalBounds().reduced(6);
        int halfW = bounds.getWidth() / 2;

        auto setupCol = [](juce::Rectangle<int> col, const juce::String& title,
                           juce::ComboBox& shape, juce::ComboBox& target,
                           juce::Slider& rate, juce::Label& rateLbl,
                           juce::Slider& depth, juce::Label& depthLbl)
        {
            col.reduce(4, 0);
            auto topBar = col.removeFromTop(24);
            shape.setBounds(topBar.removeFromLeft(90).reduced(1));
            target.setBounds(topBar.reduced(1));

            col.removeFromTop(4);
            int kw = col.getWidth() / 2;
            auto k1 = col.removeFromLeft(kw);
            rateLbl.setBounds(k1.removeFromBottom(16));
            rate.setBounds(k1);
            depthLbl.setBounds(col.removeFromBottom(16));
            depth.setBounds(col);
        };

        setupCol(bounds.removeFromLeft(halfW), "LFO 1", lfo1ShapeCombo, lfo1TargetCombo,
                 lfo1RateSlider, lfo1RateLabel, lfo1DepthSlider, lfo1DepthLabel);

        setupCol(bounds, "LFO 2", lfo2ShapeCombo, lfo2TargetCombo,
                 lfo2RateSlider, lfo2RateLabel, lfo2DepthSlider, lfo2DepthLabel);
    }
}
