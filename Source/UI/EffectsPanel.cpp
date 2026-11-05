#include "EffectsPanel.h"

namespace GranularLab
{
    EffectsPanel::EffectsPanel(juce::AudioProcessorValueTreeState& state)
        : apvts(state)
    {
        auto initKnob = [this](juce::Slider& s, juce::Label& l)
        {
            s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
            l.setFont(juce::FontOptions(11.0f, juce::Font::bold));
            l.setJustificationType(juce::Justification::centred);
            l.setColour(juce::Label::textColourId, CustomLookAndFeel::textDim());
            addAndMakeVisible(s);
            addAndMakeVisible(l);
        };

        // Filter
        addAndMakeVisible(filterToggle);
        filterToggleAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, ParamIDs::filterEnabled, filterToggle);

        filterTypeCombo.addItem("Low Pass", 1);
        filterTypeCombo.addItem("High Pass", 2);
        filterTypeCombo.addItem("Band Pass", 3);
        addAndMakeVisible(filterTypeCombo);
        filterTypeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            apvts, ParamIDs::filterType, filterTypeCombo);

        initKnob(filterCutoffSlider, filterCutoffLabel);
        filterCutoffAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::filterCutoff, filterCutoffSlider);

        initKnob(filterResoSlider, filterResoLabel);
        filterResoAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::filterResonance, filterResoSlider);

        // Delay
        addAndMakeVisible(delayToggle);
        delayToggleAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, ParamIDs::delayEnabled, delayToggle);

        initKnob(delayTimeSlider, delayTimeLabel);
        delayTimeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::delayTime, delayTimeSlider);

        initKnob(delayFeedbackSlider, delayFeedbackLabel);
        delayFeedbackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::delayFeedback, delayFeedbackSlider);

        initKnob(delayMixSlider, delayMixLabel);
        delayMixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::delayMix, delayMixSlider);

        // Reverb
        addAndMakeVisible(reverbToggle);
        reverbToggleAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, ParamIDs::reverbEnabled, reverbToggle);

        initKnob(reverbSizeSlider, reverbSizeLabel);
        reverbSizeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::reverbSize, reverbSizeSlider);

        initKnob(reverbDampingSlider, reverbDampingLabel);
        reverbDampingAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::reverbDamping, reverbDampingSlider);

        initKnob(reverbMixSlider, reverbMixLabel);
        reverbMixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::reverbMix, reverbMixSlider);

        // Saturation
        addAndMakeVisible(satToggle);
        satToggleAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, ParamIDs::satEnabled, satToggle);

        initKnob(satDriveSlider, satDriveLabel);
        satDriveAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::satDrive, satDriveSlider);

        initKnob(satMixSlider, satMixLabel);
        satMixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::satMix, satMixSlider);
    }

    void EffectsPanel::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(CustomLookAndFeel::bgCard());
        g.fillRoundedRectangle(bounds, 6.0f);
        g.setColour(CustomLookAndFeel::border());
        g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

        // Sub-panel dividing lines
        float w = bounds.getWidth() / 4.0f;
        g.setColour(CustomLookAndFeel::border().withAlpha(0.5f));
        for (int i = 1; i < 4; ++i)
        {
            float x = bounds.getX() + i * w;
            g.drawVerticalLine(static_cast<int>(x), bounds.getY() + 6.0f, bounds.getBottom() - 6.0f);
        }
    }

    void EffectsPanel::resized()
    {
        auto bounds = getLocalBounds().reduced(6);
        int colW = bounds.getWidth() / 4;

        // 1. Filter Column
        auto colFilter = bounds.removeFromLeft(colW).reduced(4);
        auto topFilter = colFilter.removeFromTop(24);
        filterToggle.setBounds(topFilter.removeFromLeft(80));
        colFilter.removeFromTop(4);
        filterTypeCombo.setBounds(colFilter.removeFromTop(22));
        colFilter.removeFromTop(6);
        int knobW = colFilter.getWidth() / 2;
        auto knobRowF = colFilter;
        auto k1 = knobRowF.removeFromLeft(knobW);
        filterCutoffLabel.setBounds(k1.removeFromBottom(16));
        filterCutoffSlider.setBounds(k1);
        filterResoLabel.setBounds(knobRowF.removeFromBottom(16));
        filterResoSlider.setBounds(knobRowF);

        // 2. Delay Column
        auto colDelay = bounds.removeFromLeft(colW).reduced(4);
        auto topDelay = colDelay.removeFromTop(24);
        delayToggle.setBounds(topDelay.removeFromLeft(80));
        colDelay.removeFromTop(10);
        int dW = colDelay.getWidth() / 3;
        auto d1 = colDelay.removeFromLeft(dW);
        delayTimeLabel.setBounds(d1.removeFromBottom(16));
        delayTimeSlider.setBounds(d1);
        auto d2 = colDelay.removeFromLeft(dW);
        delayFeedbackLabel.setBounds(d2.removeFromBottom(16));
        delayFeedbackSlider.setBounds(d2);
        delayMixLabel.setBounds(colDelay.removeFromBottom(16));
        delayMixSlider.setBounds(colDelay);

        // 3. Reverb Column
        auto colReverb = bounds.removeFromLeft(colW).reduced(4);
        auto topReverb = colReverb.removeFromTop(24);
        reverbToggle.setBounds(topReverb.removeFromLeft(80));
        colReverb.removeFromTop(10);
        int rW = colReverb.getWidth() / 3;
        auto r1 = colReverb.removeFromLeft(rW);
        reverbSizeLabel.setBounds(r1.removeFromBottom(16));
        reverbSizeSlider.setBounds(r1);
        auto r2 = colReverb.removeFromLeft(rW);
        reverbDampingLabel.setBounds(r2.removeFromBottom(16));
        reverbDampingSlider.setBounds(r2);
        reverbMixLabel.setBounds(colReverb.removeFromBottom(16));
        reverbMixSlider.setBounds(colReverb);

        // 4. Saturation Column
        auto colSat = bounds.reduced(4);
        auto topSat = colSat.removeFromTop(24);
        satToggle.setBounds(topSat.removeFromLeft(80));
        colSat.removeFromTop(10);
        int sW = colSat.getWidth() / 2;
        auto s1 = colSat.removeFromLeft(sW);
        satDriveLabel.setBounds(s1.removeFromBottom(16));
        satDriveSlider.setBounds(s1);
        satMixLabel.setBounds(colSat.removeFromBottom(16));
        satMixSlider.setBounds(colSat);
    }
}
