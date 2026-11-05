#include "PresetBar.h"

namespace GranularLab
{
    PresetBar::PresetBar(juce::AudioProcessorValueTreeState& state)
        : apvts(state)
    {
        presets = getFactoryPresets();

        int id = 1;
        for (const auto& p : presets)
        {
            presetCombo.addItem(p.name + " (" + p.category + ")", id++);
        }
        presetCombo.setSelectedId(1, juce::dontSendNotification);

        presetCombo.onChange = [this]()
        {
            int idx = presetCombo.getSelectedId() - 1;
            if (idx >= 0 && idx < static_cast<int>(presets.size()))
            {
                loadPresetInternal(presets[idx]);
            }
        };

        btnPrev.onClick = [this]()
        {
            int cur = presetCombo.getSelectedId();
            if (cur > 1) presetCombo.setSelectedId(cur - 1);
            else presetCombo.setSelectedId(static_cast<int>(presets.size()));
        };

        btnNext.onClick = [this]()
        {
            int cur = presetCombo.getSelectedId();
            if (cur < static_cast<int>(presets.size())) presetCombo.setSelectedId(cur + 1);
            else presetCombo.setSelectedId(1);
        };

        btnInit.onClick = [this]()
        {
            // Reset main parameters to default
            for (auto* param : apvts.processor.getParameters())
            {
                param->setValueNotifyingHost(param->getDefaultValue());
            }
        };

        addAndMakeVisible(presetCombo);
        addAndMakeVisible(btnPrev);
        addAndMakeVisible(btnNext);
        addAndMakeVisible(btnInit);
    }

    void PresetBar::loadPreset(int index)
    {
        if (index >= 0 && index < static_cast<int>(presets.size()))
        {
            presetCombo.setSelectedId(index + 1);
        }
    }

    void PresetBar::loadPresetInternal(const Preset& p)
    {
        for (const auto& [paramId, val] : p.paramValues)
        {
            if (auto* param = apvts.getParameter(paramId))
            {
                auto range = apvts.getParameterRange(paramId);
                float normVal = range.convertTo0to1(val);
                param->setValueNotifyingHost(normVal);
            }
        }
    }

    void PresetBar::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();

        // Plugin Title Branding
        g.setColour(CustomLookAndFeel::textLight());
        g.setFont(juce::FontOptions(17.0f, juce::Font::bold));
        g.drawText("GRANULAR LAB", bounds.removeFromLeft(170).reduced(10, 0), juce::Justification::centredLeft, true);

        // Version badge
        auto badge = juce::Rectangle<float>(145.0f, bounds.getY() + 8.0f, 32.0f, 16.0f);
        g.setColour(CustomLookAndFeel::accentViolet().withAlpha(0.25f));
        g.fillRoundedRectangle(badge, 4.0f);
        g.setColour(CustomLookAndFeel::accentViolet());
        g.drawRoundedRectangle(badge, 4.0f, 1.0f);
        g.setFont(juce::FontOptions(9.0f, juce::Font::bold));
        g.drawText("PRO", badge, juce::Justification::centred, true);
    }

    void PresetBar::resized()
    {
        auto bounds = getLocalBounds().reduced(6);
        bounds.removeFromLeft(180); // Title area

        btnInit.setBounds(bounds.removeFromRight(50).reduced(2));
        bounds.removeFromRight(12);

        btnNext.setBounds(bounds.removeFromRight(28).reduced(2));
        presetCombo.setBounds(bounds.removeFromRight(240).reduced(2));
        btnPrev.setBounds(bounds.removeFromRight(28).reduced(2));
    }
}
