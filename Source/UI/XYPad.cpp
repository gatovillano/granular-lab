#include "XYPad.h"
#include <random>

namespace GranularLab
{
    XYPad::XYPad(juce::AudioProcessorValueTreeState& state)
        : apvts(state)
    {
        xParam = apvts.getRawParameterValue(ParamIDs::xyX);
        yParam = apvts.getRawParameterValue(ParamIDs::xyY);

        if (xParam) currentX = targetX = xParam->load();
        if (yParam) currentY = targetY = yParam->load();

        // Header Tab buttons
        auto setupTabBtn = [this](juce::TextButton& btn, ViewMode mode)
        {
            btn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff181a24));
            btn.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff00d2ff));
            btn.setClickingTogglesState(true);
            btn.setRadioGroupId(3001);
            btn.onClick = [this, mode]() { setViewMode(mode); };
            addAndMakeVisible(btn);
        };

        setupTabBtn(btnTabPad,  ViewMode::Pad);
        setupTabBtn(btnTabMapX, ViewMode::MapX);
        setupTabBtn(btnTabMapY, ViewMode::MapY);
        btnTabPad.setToggleState(true, juce::dontSendNotification);

        btnRandom.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1a1d26));
        btnRandom.onClick = [this]() { randomize(); };
        addAndMakeVisible(btnRandom);

        // Setup 4 Slots for Macro X
        for (int i = 0; i < 4; ++i)
            setupSlotUI(xSlots[i], i, true);

        // Setup 4 Slots for Macro Y
        for (int i = 0; i < 4; ++i)
            setupSlotUI(ySlots[i], i, false);

        updateVisibility();
        startTimerHz(60);
    }

    XYPad::~XYPad()
    {
        stopTimer();
    }

    void XYPad::setupSlotUI(SlotUI& slot, int slotIndex, bool isXAxis)
    {
        const juce::String targetID = isXAxis
            ? (slotIndex == 0 ? ParamIDs::xyXTarget1 : (slotIndex == 1 ? ParamIDs::xyXTarget2 : (slotIndex == 2 ? ParamIDs::xyXTarget3 : ParamIDs::xyXTarget4)))
            : (slotIndex == 0 ? ParamIDs::xyYTarget1 : (slotIndex == 1 ? ParamIDs::xyYTarget2 : (slotIndex == 2 ? ParamIDs::xyYTarget3 : ParamIDs::xyYTarget4)));

        const juce::String depthID = isXAxis
            ? (slotIndex == 0 ? ParamIDs::xyXDepth1 : (slotIndex == 1 ? ParamIDs::xyXDepth2 : (slotIndex == 2 ? ParamIDs::xyXDepth3 : ParamIDs::xyXDepth4)))
            : (slotIndex == 0 ? ParamIDs::xyYDepth1 : (slotIndex == 1 ? ParamIDs::xyYDepth2 : (slotIndex == 2 ? ParamIDs::xyYDepth3 : ParamIDs::xyYDepth4)));

        // Target dropdown
        slot.targetCombo.addItemList(getXYTargetNames(), 1);
        slot.targetCombo.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff14161f));
        slot.targetCombo.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff292d3d));
        addChildComponent(slot.targetCombo);

        // Depth slider
        slot.depthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        slot.depthSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 44, 16);
        slot.depthSlider.setRange(-1.0, 1.0, 0.01);
        slot.depthSlider.setTextValueSuffix("");
        addChildComponent(slot.depthSlider);

        // Label
        slot.slotLabel.setText(juce::String(slotIndex + 1), juce::dontSendNotification);
        slot.slotLabel.setFont(juce::FontOptions(11.0f, juce::Font::bold));
        slot.slotLabel.setJustificationType(juce::Justification::centred);
        slot.slotLabel.setColour(juce::Label::textColourId, isXAxis ? juce::Colour(0xff00e5ff) : juce::Colour(0xffc77dff));
        addChildComponent(slot.slotLabel);

        // APVTS Attachments
        slot.targetAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, targetID, slot.targetCombo);
        slot.depthAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, depthID, slot.depthSlider);

        // Sync legacy target if slot 0
        if (slotIndex == 0)
        {
            slot.targetCombo.onChange = [this, isXAxis, &slot]()
            {
                const juce::String legacyID = isXAxis ? ParamIDs::xyXTarget : ParamIDs::xyYTarget;
                if (auto* p = apvts.getParameter(legacyID))
                    p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(slot.targetCombo.getSelectedId() - 1)));
            };
        }
    }

    void XYPad::setViewMode(ViewMode mode)
    {
        activeView = mode;
        btnTabPad.setToggleState(mode == ViewMode::Pad, juce::dontSendNotification);
        btnTabMapX.setToggleState(mode == ViewMode::MapX, juce::dontSendNotification);
        btnTabMapY.setToggleState(mode == ViewMode::MapY, juce::dontSendNotification);

        updateVisibility();
        resized();
        repaint();
    }

    void XYPad::updateVisibility()
    {
        bool showX = (activeView == ViewMode::MapX);
        bool showY = (activeView == ViewMode::MapY);

        for (auto& s : xSlots)
        {
            s.targetCombo.setVisible(showX);
            s.depthSlider.setVisible(showX);
            s.slotLabel.setVisible(showX);
        }

        for (auto& s : ySlots)
        {
            s.targetCombo.setVisible(showY);
            s.depthSlider.setVisible(showY);
            s.slotLabel.setVisible(showY);
        }
    }

    void XYPad::randomize()
    {
        static std::mt19937 rng(1337);
        std::uniform_real_distribution<float> dist(0.05f, 0.95f);
        targetX = dist(rng);
        targetY = dist(rng);

        if (auto* pX = apvts.getParameter(ParamIDs::xyX))
            pX->setValueNotifyingHost(targetX);
        if (auto* pY = apvts.getParameter(ParamIDs::xyY))
            pY->setValueNotifyingHost(targetY);

        repaint();
    }

    void XYPad::timerCallback()
    {
        if (xParam) targetX = xParam->load(std::memory_order_relaxed);
        if (yParam) targetY = yParam->load(std::memory_order_relaxed);

        currentX += 0.25f * (targetX - currentX);
        currentY += 0.25f * (targetY - currentY);

        repaint();
    }

    void XYPad::mouseDown(const juce::MouseEvent& e)
    {
        if (activeView == ViewMode::Pad)
        {
            if (chipXArea.contains(e.position))
            {
                setViewMode(ViewMode::MapX);
                return;
            }
            if (chipYArea.contains(e.position))
            {
                setViewMode(ViewMode::MapY);
                return;
            }
            if (padArea.contains(e.position))
            {
                updateFromMouse(e.position);
            }
        }
    }

    void XYPad::mouseDrag(const juce::MouseEvent& e)
    {
        if (activeView == ViewMode::Pad && padArea.contains(e.position))
        {
            updateFromMouse(e.position);
        }
    }

    void XYPad::updateFromMouse(const juce::Point<float>& pos)
    {
        float normX = std::clamp((pos.x - padArea.getX()) / padArea.getWidth(), 0.0f, 1.0f);
        float normY = std::clamp(1.0f - (pos.y - padArea.getY()) / padArea.getHeight(), 0.0f, 1.0f);

        targetX = normX;
        targetY = normY;

        if (auto* pX = apvts.getParameter(ParamIDs::xyX))
            pX->setValueNotifyingHost(targetX);
        if (auto* pY = apvts.getParameter(ParamIDs::xyY))
            pY->setValueNotifyingHost(targetY);

        repaint();
    }

    juce::String XYPad::getActiveMappingsSummary(bool isXAxis) const
    {
        const auto& slots = isXAxis ? xSlots : ySlots;
        juce::String summary;
        int count = 0;

        for (int i = 0; i < 4; ++i)
        {
            int targetId = slots[i].targetCombo.getSelectedId() - 1;
            if (targetId > 0 && targetId < getXYTargetNames().size())
            {
                if (count > 0) summary += ", ";
                summary += getXYTargetNames()[targetId];
                float depth = static_cast<float>(slots[i].depthSlider.getValue());
                summary += " (" + juce::String(static_cast<int>(depth * 100)) + "%)";
                ++count;
            }
        }

        if (count == 0)
            return isXAxis ? "X: Sin mapeos (Click para asignar)" : "Y: Sin mapeos (Click para asignar)";

        return (isXAxis ? "X: " : "Y: ") + summary;
    }

    void XYPad::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();

        // Card Background
        g.setColour(CustomLookAndFeel::bgCard());
        g.fillRoundedRectangle(bounds, 6.0f);

        g.setColour(CustomLookAndFeel::border());
        g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

        if (activeView == ViewMode::Pad)
        {
            // Pad Area Box
            g.setColour(juce::Colour(0xff0b0d12));
            g.fillRoundedRectangle(padArea, 6.0f);
            g.setColour(juce::Colour(0xff1f2330));
            g.drawRoundedRectangle(padArea, 6.0f, 1.0f);

            // Subtle Crosshairs & Grid
            float puckPxX = padArea.getX() + currentX * padArea.getWidth();
            float puckPxY = padArea.getY() + (1.0f - currentY) * padArea.getHeight();

            g.setColour(juce::Colour(0x22ffffff));
            const float dashes[] = { 4.0f, 4.0f };
            g.drawDashedLine(juce::Line<float>(puckPxX, padArea.getY(), puckPxX, padArea.getBottom()), dashes, 2, 1.0f);
            g.drawDashedLine(juce::Line<float>(padArea.getX(), puckPxY, padArea.getRight(), puckPxY), dashes, 2, 1.0f);

            // Radial Glow behind puck
            juce::Colour puckColor = juce::Colour(0xff00d2ff).interpolatedWith(juce::Colour(0xffb5179e), currentY);
            juce::ColourGradient glowGrad(puckColor.withAlpha(0.45f), puckPxX, puckPxY,
                                        puckColor.withAlpha(0.0f), puckPxX + 45.0f, puckPxY + 45.0f, true);
            g.setGradientFill(glowGrad);
            g.fillEllipse(puckPxX - 35.0f, puckPxY - 35.0f, 70.0f, 70.0f);

            // Puck Outer Ring
            g.setColour(puckColor);
            g.drawEllipse(puckPxX - 10.0f, puckPxY - 10.0f, 20.0f, 20.0f, 2.0f);

            // Puck Center Dot
            g.setColour(juce::Colours::white);
            g.fillEllipse(puckPxX - 4.0f, puckPxY - 4.0f, 8.0f, 8.0f);

            // Chips at bottom
            g.setFont(juce::FontOptions(9.5f, juce::Font::bold));

            // Chip X
            g.setColour(juce::Colour(0xff121520));
            g.fillRoundedRectangle(chipXArea, 4.0f);
            g.setColour(juce::Colour(0x4400e5ff));
            g.drawRoundedRectangle(chipXArea, 4.0f, 1.0f);
            g.setColour(juce::Colour(0xff00e5ff));
            g.drawText(getActiveMappingsSummary(true), chipXArea.reduced(6, 0), juce::Justification::centredLeft, true);

            // Chip Y
            g.setColour(juce::Colour(0xff121520));
            g.fillRoundedRectangle(chipYArea, 4.0f);
            g.setColour(juce::Colour(0x44c77dff));
            g.drawRoundedRectangle(chipYArea, 4.0f, 1.0f);
            g.setColour(juce::Colour(0xffc77dff));
            g.drawText(getActiveMappingsSummary(false), chipYArea.reduced(6, 0), juce::Justification::centredLeft, true);
        }
        else
        {
            // Banner for Map X or Map Y
            bool isX = (activeView == ViewMode::MapX);
            auto bannerArea = bounds.reduced(8.0f, 6.0f).removeFromTop(56.0f);
            bannerArea.removeFromTop(24.0f); // Space for tabs

            g.setColour(isX ? juce::Colour(0xff00e5ff) : juce::Colour(0xffc77dff));
            g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
            g.drawText(isX ? "MACRO X MAPPINGS (HASTA 4 DESTINOS)" : "MACRO Y MAPPINGS (HASTA 4 DESTINOS)",
                       bannerArea, juce::Justification::left);

            float val = isX ? currentX : currentY;
            juce::String valStr = (isX ? "X: " : "Y: ") + juce::String(static_cast<int>(val * 100)) + "%";
            g.drawText(valStr, bannerArea, juce::Justification::right);
        }
    }

    void XYPad::resized()
    {
        auto bounds = getLocalBounds().reduced(6);

        // Header Tab bar
        auto header = bounds.removeFromTop(22);
        btnRandom.setBounds(header.removeFromRight(34));
        header.removeFromRight(4);

        int tabW = 48;
        btnTabPad.setBounds(header.removeFromLeft(tabW));
        header.removeFromLeft(3);
        btnTabMapX.setBounds(header.removeFromLeft(tabW));
        header.removeFromLeft(3);
        btnTabMapY.setBounds(header.removeFromLeft(tabW));

        bounds.removeFromTop(6);

        if (activeView == ViewMode::Pad)
        {
            chipYArea = bounds.removeFromBottom(18).toFloat();
            bounds.removeFromBottom(3);
            chipXArea = bounds.removeFromBottom(18).toFloat();
            bounds.removeFromBottom(4);
            padArea = bounds.toFloat();
        }
        else
        {
            // Mapping slots view (4 rows)
            bounds.removeFromTop(16); // Header title space
            auto& slots = (activeView == ViewMode::MapX) ? xSlots : ySlots;

            int rowH = bounds.getHeight() / 4;

            for (int i = 0; i < 4; ++i)
            {
                auto row = bounds.removeFromTop(rowH).reduced(0, 2);
                slots[i].slotLabel.setBounds(row.removeFromLeft(16));
                row.removeFromLeft(4);
                slots[i].targetCombo.setBounds(row.removeFromLeft(row.getWidth() * 0.48f));
                row.removeFromLeft(4);
                slots[i].depthSlider.setBounds(row);
            }
        }
    }
}
