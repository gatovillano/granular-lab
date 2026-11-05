#include "SynthSourcePanel.h"
#include "../SynthProcessor.h"

namespace GranularSynthLab
{
    SynthSourcePanel::SynthSourcePanel(SynthProcessor& p)
        : processor(p)
    {
        // Source Mode buttons
        oscModeBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff222633));
        oscModeBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff7928ca));
        oscModeBtn.setClickingTogglesState(true);
        oscModeBtn.setRadioGroupId(1001);
        oscModeBtn.setToggleState(true, juce::dontSendNotification);
        oscModeBtn.onClick = [this]()
        {
            if (oscModeBtn.getToggleState())
            {
                processor.getSynthEngine().getSampleBuffer().setSourceMode(SoundSourceMode::Oscillator);
                updateState();
            }
        };
        addAndMakeVisible(oscModeBtn);

        sampleModeBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff222633));
        sampleModeBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff0070f3));
        sampleModeBtn.setClickingTogglesState(true);
        sampleModeBtn.setRadioGroupId(1001);
        sampleModeBtn.onClick = [this]()
        {
            if (sampleModeBtn.getToggleState())
            {
                processor.getSynthEngine().getSampleBuffer().setSourceMode(SoundSourceMode::Sample);
                updateState();
            }
        };
        addAndMakeVisible(sampleModeBtn);

        // Shape buttons setup
        auto setupShapeBtn = [this](juce::TextButton& btn, OscShape shape, int id)
        {
            btn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1a1d26));
            btn.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff9d4edd));
            btn.setClickingTogglesState(true);
            btn.setRadioGroupId(1002);
            btn.onClick = [this, shape]() { setOscShape(shape); };
            addAndMakeVisible(btn);
        };

        setupShapeBtn(sawBtn,   OscShape::Supersaw,     1);
        setupShapeBtn(sineBtn,  OscShape::Sine,         2);
        setupShapeBtn(triBtn,   OscShape::Triangle,     3);
        setupShapeBtn(sqrBtn,   OscShape::Square,       4);
        setupShapeBtn(noiseBtn, OscShape::Noise,        5);
        setupShapeBtn(fmBtn,    OscShape::FMBell,       6);
        setupShapeBtn(vocalBtn, OscShape::VocalFormant, 7);
        sawBtn.setToggleState(true, juce::dontSendNotification);

        // Load File button
        loadFileBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1a1d26));
        loadFileBtn.onClick = [this]() { openFileBrowser(); };
        addAndMakeVisible(loadFileBtn);

        // Root Note combo
        for (int n = 36; n <= 84; ++n)
        {
            juce::String noteName = juce::MidiMessage::getMidiNoteName(n, true, true, 3);
            rootNoteBox.addItem(noteName, n - 35);
        }
        rootNoteBox.setSelectedId(60 - 35, juce::dontSendNotification);
        addAndMakeVisible(rootNoteBox);

        // Octave combo
        octaveBox.addItem("-2 Oct", 1);
        octaveBox.addItem("-1 Oct", 2);
        octaveBox.addItem(" 0 Oct", 3);
        octaveBox.addItem("+1 Oct", 4);
        octaveBox.addItem("+2 Oct", 5);
        octaveBox.setSelectedId(3, juce::dontSendNotification);
        addAndMakeVisible(octaveBox);

        // Attachments
        rootNoteAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            processor.getAPVTS(), ParamIDs::rootNote, rootNoteBox);
        octaveAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            processor.getAPVTS(), ParamIDs::octave, octaveBox);

        updateState();
    }

    void SynthSourcePanel::setOscShape(OscShape shape)
    {
        processor.setOscillatorShape(shape);
        updateState();
    }

    void SynthSourcePanel::openFileBrowser()
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Seleccionar muestra de audio para Granular Synth Lab",
            juce::File::getSpecialLocation(juce::File::userHomeDirectory),
            "*.wav;*.aiff;*.aif;*.flac;*.ogg;*.mp3");

        auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        fileChooser->launchAsync(flags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                processor.loadSampleFile(file);
                sampleModeBtn.setToggleState(true, juce::sendNotification);
                updateState();
            }
        });
    }

    void SynthSourcePanel::updateState()
    {
        bool isOsc = (processor.getSynthEngine().getSampleBuffer().getSourceMode() == SoundSourceMode::Oscillator);
        oscModeBtn.setToggleState(isOsc, juce::dontSendNotification);
        sampleModeBtn.setToggleState(!isOsc, juce::dontSendNotification);

        sawBtn.setVisible(isOsc);
        sineBtn.setVisible(isOsc);
        triBtn.setVisible(isOsc);
        sqrBtn.setVisible(isOsc);
        noiseBtn.setVisible(isOsc);
        fmBtn.setVisible(isOsc);
        vocalBtn.setVisible(isOsc);

        loadFileBtn.setVisible(!isOsc);

        if (isOsc)
        {
            auto currentShape = processor.getSynthEngine().getSampleBuffer().getCurrentOscShape();
            sawBtn.setToggleState(currentShape == OscShape::Supersaw, juce::dontSendNotification);
            sineBtn.setToggleState(currentShape == OscShape::Sine, juce::dontSendNotification);
            triBtn.setToggleState(currentShape == OscShape::Triangle, juce::dontSendNotification);
            sqrBtn.setToggleState(currentShape == OscShape::Square, juce::dontSendNotification);
            noiseBtn.setToggleState(currentShape == OscShape::Noise, juce::dontSendNotification);
            fmBtn.setToggleState(currentShape == OscShape::FMBell, juce::dontSendNotification);
            vocalBtn.setToggleState(currentShape == OscShape::VocalFormant, juce::dontSendNotification);
        }

        repaint();
    }

    void SynthSourcePanel::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xff12141a));
        g.fillRoundedRectangle(bounds, 6.0f);
        g.setColour(juce::Colour(0xff222633));
        g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

        g.setColour(juce::Colour(0x88ffffff));
        g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        g.drawText("SOURCE", 10, 6, 80, 14, juce::Justification::left);
        g.drawText("ROOT / OCT", bounds.getRight() - 170, 6, 160, 14, juce::Justification::right);
    }

    void SynthSourcePanel::resized()
    {
        auto area = getLocalBounds().reduced(8, 6);
        area.removeFromTop(14); // Header label space

        auto leftArea = area.removeFromLeft(190);
        oscModeBtn.setBounds(leftArea.removeFromLeft(92));
        leftArea.removeFromLeft(6);
        sampleModeBtn.setBounds(leftArea);

        area.removeFromLeft(12);

        // Right side: Root note and Octave
        auto rightArea = area.removeFromRight(150);
        octaveBox.setBounds(rightArea.removeFromRight(70));
        rightArea.removeFromRight(6);
        rootNoteBox.setBounds(rightArea);

        area.removeFromRight(12);

        // Center: Shape buttons or Load File button
        if (sawBtn.isVisible())
        {
            int btnWidth = (area.getWidth() - 6 * 4) / 7;
            sawBtn.setBounds(area.removeFromLeft(btnWidth)); area.removeFromLeft(4);
            sineBtn.setBounds(area.removeFromLeft(btnWidth)); area.removeFromLeft(4);
            triBtn.setBounds(area.removeFromLeft(btnWidth)); area.removeFromLeft(4);
            sqrBtn.setBounds(area.removeFromLeft(btnWidth)); area.removeFromLeft(4);
            noiseBtn.setBounds(area.removeFromLeft(btnWidth)); area.removeFromLeft(4);
            fmBtn.setBounds(area.removeFromLeft(btnWidth)); area.removeFromLeft(4);
            vocalBtn.setBounds(area);
        }
        else
        {
            loadFileBtn.setBounds(area.reduced(10, 0));
        }
    }
}
