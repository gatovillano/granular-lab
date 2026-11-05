#include "SynthEditor.h"

namespace GranularSynthLab
{
    SynthEditor::SynthEditor(SynthProcessor& p)
        : AudioProcessorEditor(&p),
          processor(p),
          presetBar(p),
          sourcePanel(p),
          waveformView(p),
          particleView(p),
          xyPad(p.getAPVTS()),
          adsrPanel(p),
          effectsPanel(p.getAPVTS()),
          modulationPanel(p.getAPVTS()),
          keyboardComponent(p.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
    {
        setLookAndFeel(&customLookAndFeel);

        addAndMakeVisible(presetBar);
        addAndMakeVisible(sourcePanel);
        addAndMakeVisible(waveformView);
        addAndMakeVisible(particleView);
        addAndMakeVisible(xyPad);
        addAndMakeVisible(adsrPanel);

        // Granular Knobs
        setupKnob(sizeSlider,      sizeLabel,      " ms");
        setupKnob(densitySlider,   densityLabel,   " Hz");
        setupKnob(spraySlider,     sprayLabel,     "");
        setupKnob(pitchRandSlider, pitchRandLabel, " st");
        setupKnob(panRandSlider,   panRandLabel,   "");
        setupKnob(reverseSlider,   reverseLabel,   " %");

        // Window Shape
        windowShapeCombo.addItem("Hann", 1);
        windowShapeCombo.addItem("Gaussian", 2);
        windowShapeCombo.addItem("Tukey", 3);
        windowShapeCombo.addItem("Triangle", 4);
        windowShapeCombo.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff161922));
        windowShapeCombo.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff2a2e3d));
        addAndMakeVisible(windowShapeCombo);

        windowLabel.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        windowLabel.setJustificationType(juce::Justification::centred);
        windowLabel.setColour(juce::Label::textColourId, juce::Colour(0x99ffffff));
        addAndMakeVisible(windowLabel);

        // Attachments
        auto& apvts = processor.getAPVTS();
        sizeAttach      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ParamIDs::grainSize, sizeSlider);
        densityAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ParamIDs::density, densitySlider);
        sprayAttach     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ParamIDs::spray, spraySlider);
        pitchRandAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ParamIDs::pitchRand, pitchRandSlider);
        panRandAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ParamIDs::panRand, panRandSlider);
        reverseAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ParamIDs::reverseProb, reverseSlider);
        windowShapeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, ParamIDs::envelopeShape, windowShapeCombo);

        // Tab buttons
        btnTabFX.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1c1f2b));
        btnTabFX.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff7928ca));
        btnTabFX.setClickingTogglesState(true);
        btnTabFX.setRadioGroupId(2001);
        btnTabFX.setToggleState(true, juce::dontSendNotification);
        btnTabFX.onClick = [this]() { switchTab(true); };
        addAndMakeVisible(btnTabFX);

        btnTabMod.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1c1f2b));
        btnTabMod.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff0070f3));
        btnTabMod.setClickingTogglesState(true);
        btnTabMod.setRadioGroupId(2001);
        btnTabMod.onClick = [this]() { switchTab(false); };
        addAndMakeVisible(btnTabMod);

        addAndMakeVisible(effectsPanel);
        addChildComponent(modulationPanel);

        // Virtual Keyboard
        keyboardComponent.setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour(0xff222530));
        keyboardComponent.setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xff111318));
        keyboardComponent.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, juce::Colour(0xaa00e5ff));
        keyboardComponent.setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, juce::Colour(0x3300e5ff));
        addAndMakeVisible(keyboardComponent);

        setSize(1060, 780);
        setResizable(true, true);
        setResizeLimits(940, 680, 1600, 1200);

        // Initialize waveform snapshot
        waveformView.refresh();

        startTimerHz(60);
    }

    SynthEditor::~SynthEditor()
    {
        stopTimer();
        setLookAndFeel(nullptr);
    }

    void SynthEditor::setupKnob(juce::Slider& slider, juce::Label& label, const juce::String& suffix)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 16);
        slider.setTextValueSuffix(suffix);
        addAndMakeVisible(slider);

        label.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colour(0x99ffffff));
        addAndMakeVisible(label);
    }

    void SynthEditor::switchTab(bool showFX)
    {
        effectsPanel.setVisible(showFX);
        modulationPanel.setVisible(!showFX);
        btnTabFX.setToggleState(showFX, juce::dontSendNotification);
        btnTabMod.setToggleState(!showFX, juce::dontSendNotification);
    }

    void SynthEditor::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colour(0xff0e1017));
    }

    void SynthEditor::resized()
    {
        auto bounds = getLocalBounds();

        // 1. Header (Preset bar)
        presetBar.setBounds(bounds.removeFromTop(36));

        // 2. Virtual Keyboard at bottom
        keyboardComponent.setBounds(bounds.removeFromBottom(68));

        // 3. Effects / Modulation section at bottom
        auto bottomArea = bounds.removeFromBottom(145);
        auto tabBtnArea = bottomArea.removeFromTop(24).reduced(8, 0);
        btnTabFX.setBounds(tabBtnArea.removeFromLeft(110));
        tabBtnArea.removeFromLeft(6);
        btnTabMod.setBounds(tabBtnArea.removeFromLeft(120));

        effectsPanel.setBounds(bottomArea.reduced(8, 2));
        modulationPanel.setBounds(bottomArea.reduced(8, 2));

        // 4. Source selection bar
        sourcePanel.setBounds(bounds.removeFromTop(44).reduced(8, 2));

        // 5. Granular Knobs Row
        auto knobsArea = bounds.removeFromBottom(84).reduced(8, 4);

        int totalItems = 7;
        int itemW = knobsArea.getWidth() / totalItems;

        auto placeKnob = [&](juce::Slider& s, juce::Label& l, juce::Rectangle<int> col)
        {
            l.setBounds(col.removeFromTop(15));
            s.setBounds(col);
        };

        placeKnob(sizeSlider, sizeLabel, knobsArea.removeFromLeft(itemW));
        placeKnob(densitySlider, densityLabel, knobsArea.removeFromLeft(itemW));
        placeKnob(spraySlider, sprayLabel, knobsArea.removeFromLeft(itemW));
        placeKnob(pitchRandSlider, pitchRandLabel, knobsArea.removeFromLeft(itemW));
        placeKnob(panRandSlider, panRandLabel, knobsArea.removeFromLeft(itemW));
        placeKnob(reverseSlider, reverseLabel, knobsArea.removeFromLeft(itemW));

        // Window shape combo in last column
        auto windowCol = knobsArea;
        windowLabel.setBounds(windowCol.removeFromTop(15));
        windowCol.removeFromTop(12);
        windowShapeCombo.setBounds(windowCol.removeFromTop(24).reduced(6, 0));

        // 6. Middle Section: XY Pad (Left) | Waveform & Particles (Center) | ADSR (Right)
        auto midArea = bounds.reduced(8, 4);
        xyPad.setBounds(midArea.removeFromLeft(220));
        midArea.removeFromLeft(8);

        adsrPanel.setBounds(midArea.removeFromRight(220));
        midArea.removeFromRight(8);

        // Center split: Waveform (top) + Particles (bottom)
        int centerH = midArea.getHeight() / 2;
        waveformView.setBounds(midArea.removeFromTop(centerH - 3));
        midArea.removeFromTop(6);
        particleView.setBounds(midArea);
    }

    void SynthEditor::timerCallback()
    {
        auto& apvts = processor.getAPVTS();

        const auto* pX = apvts.getRawParameterValue(ParamIDs::xyX);
        const auto* pY = apvts.getRawParameterValue(ParamIDs::xyY);

        float macroX = pX ? pX->load(std::memory_order_relaxed) : 0.5f;
        float macroY = pY ? pY->load(std::memory_order_relaxed) : 0.5f;

        float xyMod[static_cast<int>(GranularLab::XYTarget::NumTargets)] = { 0.0f };

        std::atomic<float>* pXTargets[4] = {
            apvts.getRawParameterValue(ParamIDs::xyXTarget1),
            apvts.getRawParameterValue(ParamIDs::xyXTarget2),
            apvts.getRawParameterValue(ParamIDs::xyXTarget3),
            apvts.getRawParameterValue(ParamIDs::xyXTarget4)
        };
        std::atomic<float>* pXDepths[4] = {
            apvts.getRawParameterValue(ParamIDs::xyXDepth1),
            apvts.getRawParameterValue(ParamIDs::xyXDepth2),
            apvts.getRawParameterValue(ParamIDs::xyXDepth3),
            apvts.getRawParameterValue(ParamIDs::xyXDepth4)
        };
        std::atomic<float>* pYTargets[4] = {
            apvts.getRawParameterValue(ParamIDs::xyYTarget1),
            apvts.getRawParameterValue(ParamIDs::xyYTarget2),
            apvts.getRawParameterValue(ParamIDs::xyYTarget3),
            apvts.getRawParameterValue(ParamIDs::xyYTarget4)
        };
        std::atomic<float>* pYDepths[4] = {
            apvts.getRawParameterValue(ParamIDs::xyYDepth1),
            apvts.getRawParameterValue(ParamIDs::xyYDepth2),
            apvts.getRawParameterValue(ParamIDs::xyYDepth3),
            apvts.getRawParameterValue(ParamIDs::xyYDepth4)
        };

        for (int s = 0; s < 4; ++s)
        {
            if (pXTargets[s] && pXDepths[s])
            {
                int t = static_cast<int>(pXTargets[s]->load(std::memory_order_relaxed));
                float d = pXDepths[s]->load(std::memory_order_relaxed);
                if (t > 0 && t < static_cast<int>(GranularLab::XYTarget::NumTargets))
                    xyMod[t] += macroX * d;
            }

            if (pYTargets[s] && pYDepths[s])
            {
                int t = static_cast<int>(pYTargets[s]->load(std::memory_order_relaxed));
                float d = pYDepths[s]->load(std::memory_order_relaxed);
                if (t > 0 && t < static_cast<int>(GranularLab::XYTarget::NumTargets))
                    xyMod[t] += macroY * d;
            }
        }

        auto updateSlider = [&](juce::Slider& s, GranularLab::XYTarget target)
        {
            float mod = xyMod[static_cast<int>(target)];
            float cur = static_cast<float>(s.getProperties().getWithDefault("macroMod", 0.0f));
            if (std::abs(mod - cur) > 0.001f)
            {
                s.getProperties().set("macroMod", mod);
                s.repaint();
            }
        };

        updateSlider(densitySlider, GranularLab::XYTarget::Density);
        updateSlider(sizeSlider, GranularLab::XYTarget::GrainSize);
        updateSlider(spraySlider, GranularLab::XYTarget::Spray);
        updateSlider(pitchRandSlider, GranularLab::XYTarget::Pitch);
        updateSlider(panRandSlider, GranularLab::XYTarget::Pan);
        updateSlider(reverseSlider, GranularLab::XYTarget::ReverseProb);

        effectsPanel.updateMacroModulations(xyMod);
    }
}
