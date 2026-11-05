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

        auto addTargets = [](juce::ComboBox& cb)
        {
            cb.addItem("Density", 1);
            cb.addItem("Grain Size", 2);
            cb.addItem("Position", 3);
            cb.addItem("Spray", 4);
            cb.addItem("Pitch", 5);
            cb.addItem("Cutoff", 6);
            cb.addItem("Mix", 7);
        };

        addTargets(comboTargetX);
        addTargets(comboTargetY);

        // Fetch initial values from APVTS
        if (auto* pX = apvts.getRawParameterValue(ParamIDs::xyXTarget))
            comboTargetX.setSelectedId(static_cast<int>(pX->load()) + 1, juce::dontSendNotification);
        else
            comboTargetX.setSelectedId(1, juce::dontSendNotification);

        if (auto* pY = apvts.getRawParameterValue(ParamIDs::xyYTarget))
            comboTargetY.setSelectedId(static_cast<int>(pY->load()) + 1, juce::dontSendNotification);
        else
            comboTargetY.setSelectedId(2, juce::dontSendNotification);

        comboTargetX.onChange = [this]()
        {
            if (auto* param = apvts.getParameter(ParamIDs::xyXTarget))
                param->setValueNotifyingHost(static_cast<float>(comboTargetX.getSelectedId() - 1) / 6.0f);
            applyXYToTargets();
        };

        comboTargetY.onChange = [this]()
        {
            if (auto* param = apvts.getParameter(ParamIDs::xyYTarget))
                param->setValueNotifyingHost(static_cast<float>(comboTargetY.getSelectedId() - 1) / 6.0f);
            applyXYToTargets();
        };

        btnRandom.onClick = [this]() { randomize(); };

        addAndMakeVisible(comboTargetX);
        addAndMakeVisible(comboTargetY);
        addAndMakeVisible(btnRandom);

        startTimerHz(60);
    }

    XYPad::~XYPad()
    {
        stopTimer();
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

        applyXYToTargets();
    }

    void XYPad::timerCallback()
    {
        if (xParam) targetX = xParam->load();
        if (yParam) targetY = yParam->load();

        // Smooth visual easing towards target
        currentX += 0.25f * (targetX - currentX);
        currentY += 0.25f * (targetY - currentY);

        repaint();
    }

    void XYPad::mouseDown(const juce::MouseEvent& e)
    {
        if (padArea.contains(e.position))
            updateFromMouse(e.position);
    }

    void XYPad::mouseDrag(const juce::MouseEvent& e)
    {
        if (padArea.contains(e.position))
            updateFromMouse(e.position);
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

        applyXYToTargets();
        repaint();
    }

    void XYPad::applyXYToTargets()
    {
        auto mapTarget = [this](int targetIdx, float normVal)
        {
            switch (targetIdx)
            {
                case 0: // Density (1 to 100 Hz)
                    if (auto* p = apvts.getParameter(ParamIDs::density))
                        p->setValueNotifyingHost(normVal);
                    break;
                case 1: // Grain Size (10 to 500 ms)
                    if (auto* p = apvts.getParameter(ParamIDs::grainSize))
                        p->setValueNotifyingHost(normVal);
                    break;
                case 2: // Position (0 to 1)
                    if (auto* p = apvts.getParameter(ParamIDs::position))
                        p->setValueNotifyingHost(normVal);
                    break;
                case 3: // Spray (0 to 1)
                    if (auto* p = apvts.getParameter(ParamIDs::spray))
                        p->setValueNotifyingHost(normVal);
                    break;
                case 4: // Pitch (-24 to +24 st)
                    if (auto* p = apvts.getParameter(ParamIDs::pitch))
                        p->setValueNotifyingHost(normVal);
                    break;
                case 5: // Filter Cutoff (20 to 20000 Hz)
                    if (auto* p = apvts.getParameter(ParamIDs::filterCutoff))
                        p->setValueNotifyingHost(normVal);
                    break;
                case 6: // Mix (0 to 1)
                    if (auto* p = apvts.getParameter(ParamIDs::mix))
                        p->setValueNotifyingHost(normVal);
                    break;
                default: break;
            }
        };

        mapTarget(comboTargetX.getSelectedId() - 1, targetX);
        mapTarget(comboTargetY.getSelectedId() - 1, targetY);
    }

    void XYPad::paint(juce::Graphics& g)
    {
        // Background frame
        g.setColour(CustomLookAndFeel::bgCard());
        g.fillRoundedRectangle(padArea, 6.0f);

        g.setColour(CustomLookAndFeel::border());
        g.drawRoundedRectangle(padArea, 6.0f, 1.2f);

        // Subtle grid lines
        g.setColour(CustomLookAndFeel::border().withAlpha(0.35f));
        float midX = padArea.getX() + padArea.getWidth() * 0.5f;
        float midY = padArea.getY() + padArea.getHeight() * 0.5f;
        const float dashes[] = { 4.0f, 4.0f };
        g.drawDashedLine(juce::Line<float>(midX, padArea.getY(), midX, padArea.getBottom()),
                         dashes, 2, 1.0f);
        g.drawDashedLine(juce::Line<float>(padArea.getX(), midY, padArea.getRight(), midY),
                         dashes, 2, 1.0f);

        // Grid quadrants
        for (int i = 1; i <= 3; ++i)
        {
            float qx = padArea.getX() + padArea.getWidth() * (i * 0.25f);
            float qy = padArea.getY() + padArea.getHeight() * (i * 0.25f);
            g.setColour(CustomLookAndFeel::border().withAlpha(0.15f));
            g.drawVerticalLine(static_cast<int>(qx), padArea.getY(), padArea.getBottom());
            g.drawHorizontalLine(static_cast<int>(qy), padArea.getX(), padArea.getRight());
        }

        // Puck position in screen space
        float puckX = padArea.getX() + currentX * padArea.getWidth();
        float puckY = padArea.getY() + (1.0f - currentY) * padArea.getHeight();

        // Crosshair rays with gradient
        juce::Colour crossColour = CustomLookAndFeel::accentViolet().withAlpha(0.3f);
        g.setColour(crossColour);
        g.drawLine(padArea.getX(), puckY, padArea.getRight(), puckY, 1.0f);
        g.drawLine(puckX, padArea.getY(), puckX, padArea.getBottom(), 1.0f);

        // Multi-layer neon glowing puck
        g.setColour(CustomLookAndFeel::accentCyan().withAlpha(0.15f));
        g.fillEllipse(puckX - 22.0f, puckY - 22.0f, 44.0f, 44.0f);

        g.setColour(CustomLookAndFeel::accentViolet().withAlpha(0.35f));
        g.fillEllipse(puckX - 14.0f, puckY - 14.0f, 28.0f, 28.0f);

        juce::ColourGradient puckGrad(CustomLookAndFeel::accentCyan(), puckX - 6.0f, puckY - 6.0f,
                                      CustomLookAndFeel::accentViolet(), puckX + 8.0f, puckY + 8.0f, false);
        g.setGradientFill(puckGrad);
        g.fillEllipse(puckX - 7.0f, puckY - 7.0f, 14.0f, 14.0f);

        g.setColour(CustomLookAndFeel::textLight());
        g.fillEllipse(puckX - 2.5f, puckY - 2.5f, 5.0f, 5.0f);

        // Coordinates badge
        juce::String coordStr = "X: " + juce::String(currentX, 2) + "  Y: " + juce::String(currentY, 2);
        g.setColour(CustomLookAndFeel::textDim().withAlpha(0.7f));
        g.setFont(juce::FontOptions(11.0f));
        g.drawText(coordStr, padArea.reduced(8.0f), juce::Justification::bottomRight, true);
    }

    void XYPad::resized()
    {
        auto bounds = getLocalBounds();
        auto topBar = bounds.removeFromTop(28);

        comboTargetX.setBounds(topBar.removeFromLeft(100).reduced(2));
        topBar.removeFromLeft(6);
        comboTargetY.setBounds(topBar.removeFromLeft(100).reduced(2));
        btnRandom.setBounds(topBar.removeFromRight(50).reduced(2));

        bounds.removeFromTop(4);
        padArea = bounds.toFloat();
    }
}
