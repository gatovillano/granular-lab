#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace GranularLab
{
    GranularLabAudioProcessorEditor::GranularLabAudioProcessorEditor(GranularLabAudioProcessor& p)
        : AudioProcessorEditor(&p),
          processorRef(p),
          presetBar(p.getAPVTS()),
          xyPad(p.getAPVTS()),
          waveformView(p.getGranularEngine().getAudioBuffer(), p.getAPVTS()),
          particleView(p.getGranularEngine()),
          effectsPanel(p.getAPVTS()),
          modulationPanel(p.getAPVTS())
    {
        juce::LookAndFeel::setDefaultLookAndFeel(&customLookAndFeel);

        addAndMakeVisible(presetBar);
        addAndMakeVisible(xyPad);
        addAndMakeVisible(waveformView);
        addAndMakeVisible(particleView);

        // Setup Main Knobs
        setupKnob(knobGrainSize, ParamIDs::grainSize, "SIZE");
        setupKnob(knobDensity, ParamIDs::density, "DENSITY");
        setupKnob(knobPosition, ParamIDs::position, "POSITION");
        setupKnob(knobSpray, ParamIDs::spray, "SPRAY");
        setupKnob(knobPitch, ParamIDs::pitch, "PITCH");
        setupKnob(knobPitchRand, ParamIDs::pitchRand, "P. RAND");
        setupKnob(knobPan, ParamIDs::pan, "PAN");
        setupKnob(knobPanRand, ParamIDs::panRand, "SPREAD");
        setupKnob(knobReverse, ParamIDs::reverseProb, "REVERSE");
        setupKnob(knobFeedback, ParamIDs::feedback, "FEEDBACK");
        setupKnob(knobMix, ParamIDs::mix, "DRY/WET");
        setupKnob(knobOutput, ParamIDs::outputGain, "OUTPUT");

        // Freeze Button
        addAndMakeVisible(btnFreeze);
        freezeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            processorRef.getAPVTS(), ParamIDs::freeze, btnFreeze);

        // Window Shape
        comboWindow.addItem("Hann", 1);
        comboWindow.addItem("Gaussian", 2);
        comboWindow.addItem("Tukey", 3);
        comboWindow.addItem("Triangle", 4);
        addAndMakeVisible(comboWindow);
        windowAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            processorRef.getAPVTS(), ParamIDs::envelopeShape, comboWindow);

        // Bottom Deck
        addAndMakeVisible(tabEffectsBtn);
        addAndMakeVisible(tabModBtn);
        addAndMakeVisible(effectsPanel);
        addChildComponent(modulationPanel);

        tabEffectsBtn.onClick = [this]()
        {
            showingEffectsTab = true;
            effectsPanel.setVisible(true);
            modulationPanel.setVisible(false);
            tabEffectsBtn.setColour(juce::TextButton::buttonColourId, CustomLookAndFeel::accentViolet().withAlpha(0.4f));
            tabModBtn.setColour(juce::TextButton::buttonColourId, CustomLookAndFeel::bgCardLight());
            repaint();
        };

        tabModBtn.onClick = [this]()
        {
            showingEffectsTab = false;
            effectsPanel.setVisible(false);
            modulationPanel.setVisible(true);
            tabModBtn.setColour(juce::TextButton::buttonColourId, CustomLookAndFeel::accentViolet().withAlpha(0.4f));
            tabEffectsBtn.setColour(juce::TextButton::buttonColourId, CustomLookAndFeel::bgCardLight());
            repaint();
        };

        tabEffectsBtn.setColour(juce::TextButton::buttonColourId, CustomLookAndFeel::accentViolet().withAlpha(0.4f));
        tabModBtn.setColour(juce::TextButton::buttonColourId, CustomLookAndFeel::bgCardLight());

        setSize(1000, 720);
        setResizable(true, true);
        setResizeLimits(880, 640, 1400, 1000);
    }

    GranularLabAudioProcessorEditor::~GranularLabAudioProcessorEditor()
    {
        juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    }

    void GranularLabAudioProcessorEditor::setupKnob(KnobWithLabel& k, const juce::String& paramId, const juce::String& labelText)
    {
        k.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        k.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 16);
        k.label.setText(labelText, juce::dontSendNotification);
        k.label.setFont(juce::FontOptions(11.0f, juce::Font::bold));
        k.label.setJustificationType(juce::Justification::centred);
        k.label.setColour(juce::Label::textColourId, CustomLookAndFeel::textDim());

        addAndMakeVisible(k.slider);
        addAndMakeVisible(k.label);

        k.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processorRef.getAPVTS(), paramId, k.slider);
    }

    void GranularLabAudioProcessorEditor::paint(juce::Graphics& g)
    {
        g.fillAll(CustomLookAndFeel::bgDark());

        // Header separator
        g.setColour(CustomLookAndFeel::border());
        g.drawHorizontalLine(44, 0.0f, static_cast<float>(getWidth()));

        // Middle deck card background
        auto bounds = getLocalBounds();
        bounds.removeFromTop(48);
        int middleY = 48 + 320 + 8;
        auto middleStrip = juce::Rectangle<float>(12.0f, static_cast<float>(middleY),
                                                  static_cast<float>(getWidth() - 24), 110.0f);
        g.setColour(CustomLookAndFeel::bgCard());
        g.fillRoundedRectangle(middleStrip, 6.0f);
        g.setColour(CustomLookAndFeel::border());
        g.drawRoundedRectangle(middleStrip, 6.0f, 1.0f);
    }

    void GranularLabAudioProcessorEditor::resized()
    {
        auto bounds = getLocalBounds();

        // 1. Header Bar
        presetBar.setBounds(bounds.removeFromTop(44));
        bounds.removeFromTop(6);

        // 2. Main Visual Deck (Top section: Height ~320px)
        auto visualArea = bounds.removeFromTop(310).reduced(12, 0);
        int xyWidth = static_cast<int>(visualArea.getWidth() * 0.44f);
        xyPad.setBounds(visualArea.removeFromLeft(xyWidth));

        visualArea.removeFromLeft(10); // spacing
        int halfH = (visualArea.getHeight() - 8) / 2;
        waveformView.setBounds(visualArea.removeFromTop(halfH));
        visualArea.removeFromTop(8);
        particleView.setBounds(visualArea);

        bounds.removeFromTop(10);

        // 3. Middle Strip: Main Granular Knobs (Height ~110px)
        auto middleArea = bounds.removeFromTop(110).reduced(16, 4);

        // Freeze & Window shape controls on left side of strip
        auto leftSide = middleArea.removeFromLeft(90).reduced(4);
        btnFreeze.setBounds(leftSide.removeFromTop(36));
        leftSide.removeFromTop(8);
        comboWindow.setBounds(leftSide.removeFromTop(24));

        middleArea.removeFromLeft(12);

        // 12 knobs evenly distributed
        std::array<KnobWithLabel*, 12> knobs = {
            &knobGrainSize, &knobDensity, &knobPosition, &knobSpray,
            &knobPitch, &knobPitchRand, &knobPan, &knobPanRand,
            &knobReverse, &knobFeedback, &knobMix, &knobOutput
        };

        int knobW = middleArea.getWidth() / static_cast<int>(knobs.size());
        for (auto* k : knobs)
        {
            auto col = middleArea.removeFromLeft(knobW).reduced(2);
            k->label.setBounds(col.removeFromTop(18));
            k->slider.setBounds(col);
        }

        bounds.removeFromTop(10);

        // 4. Bottom Section: Tabs + Effects / Modulation (Remaining height)
        auto bottomDeck = bounds.reduced(12, 6);
        auto tabRow = bottomDeck.removeFromTop(28);
        tabEffectsBtn.setBounds(tabRow.removeFromLeft(280).reduced(2));
        tabModBtn.setBounds(tabRow.removeFromLeft(220).reduced(2));

        bottomDeck.removeFromTop(4);
        effectsPanel.setBounds(bottomDeck);
        modulationPanel.setBounds(bottomDeck);
    }
}
