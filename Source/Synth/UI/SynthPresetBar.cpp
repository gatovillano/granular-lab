#include "SynthPresetBar.h"
#include "../SynthProcessor.h"
#include "../Presets/SynthPresetData.h"

namespace GranularSynthLab
{
    SynthPresetBar::SynthPresetBar(SynthProcessor& p)
        : processor(p)
    {
        presetCombo.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff161922));
        presetCombo.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff2a2e3d));

        const auto& presets = getFactorySynthPresets();
        for (size_t i = 0; i < presets.size(); ++i)
        {
            juce::String text = "[" + presets[i].category + "] " + presets[i].name;
            presetCombo.addItem(text, static_cast<int>(i + 1));
        }

        presetCombo.setSelectedId(1, juce::dontSendNotification);
        presetCombo.onChange = [this]() { loadSelectedPreset(); };
        addAndMakeVisible(presetCombo);

        auto setupBtn = [this](juce::TextButton& btn)
        {
            btn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1a1d26));
            addAndMakeVisible(btn);
        };

        setupBtn(prevBtn);
        setupBtn(nextBtn);
        setupBtn(initBtn);

        prevBtn.onClick = [this]() { selectPrevPreset(); };
        nextBtn.onClick = [this]() { selectNextPreset(); };
        initBtn.onClick = [this]()
        {
            presetCombo.setSelectedId(1, juce::sendNotification);
        };
    }

    void SynthPresetBar::loadSelectedPreset()
    {
        int id = presetCombo.getSelectedId();
        if (id > 0)
        {
            processor.loadPreset(id - 1);
        }
    }

    void SynthPresetBar::selectPrevPreset()
    {
        int cur = presetCombo.getSelectedId();
        if (cur > 1)
            presetCombo.setSelectedId(cur - 1, juce::sendNotification);
        else
            presetCombo.setSelectedId(presetCombo.getNumItems(), juce::sendNotification);
    }

    void SynthPresetBar::selectNextPreset()
    {
        int cur = presetCombo.getSelectedId();
        if (cur < presetCombo.getNumItems())
            presetCombo.setSelectedId(cur + 1, juce::sendNotification);
        else
            presetCombo.setSelectedId(1, juce::sendNotification);
    }

    void SynthPresetBar::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xff0c0d12));
        g.fillRect(bounds);

        // Subtle bottom border
        g.setColour(juce::Colour(0xff1f2330));
        g.drawHorizontalLine(getHeight() - 1, 0, static_cast<float>(getWidth()));

        // Title branding
        g.setFont(juce::FontOptions(15.0f, juce::Font::bold));
        juce::ColourGradient titleGrad(juce::Colour(0xff00ffff), 12.0f, 0.0f,
                                      juce::Colour(0xffc77dff), 190.0f, 0.0f, false);
        g.setGradientFill(titleGrad);
        g.drawText("GRANULAR SYNTH LAB", 14, 0, 210, getHeight(), juce::Justification::centredLeft);

        // Subtitle badge
        g.setFont(juce::FontOptions(9.0f));
        g.setColour(juce::Colour(0x77ffffff));
        g.drawText("POLYPHONIC SYNTHESIZER", 230, 0, 160, getHeight(), juce::Justification::centredLeft);
    }

    void SynthPresetBar::resized()
    {
        auto area = getLocalBounds().reduced(8, 6);
        area.removeFromLeft(380); // Title space

        initBtn.setBounds(area.removeFromRight(46));
        area.removeFromRight(6);
        nextBtn.setBounds(area.removeFromRight(28));
        area.removeFromRight(4);
        prevBtn.setBounds(area.removeFromRight(28));
        area.removeFromRight(6);

        presetCombo.setBounds(area);
    }
}
