#include "CustomLookAndFeel.h"

namespace GranularLab
{
    CustomLookAndFeel::CustomLookAndFeel()
    {
        setColour(juce::ResizableWindow::backgroundColourId, bgDark());
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxTextColourId, textLight());
        setColour(juce::ComboBox::backgroundColourId, bgCardLight());
        setColour(juce::ComboBox::outlineColourId, border());
        setColour(juce::ComboBox::textColourId, textLight());
        setColour(juce::ComboBox::arrowColourId, accentCyan());
        setColour(juce::PopupMenu::backgroundColourId, bgCard());
        setColour(juce::PopupMenu::textColourId, textLight());
        setColour(juce::PopupMenu::highlightedBackgroundColourId, accentViolet().withAlpha(0.4f));
        setColour(juce::PopupMenu::highlightedTextColourId, textLight());
    }

    void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPosProportional, float rotaryStartAngle,
                                            float rotaryEndAngle, juce::Slider& slider)
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto center = bounds.getCentre();

        // Check if there is an active macro modulation offset on this slider
        float modOffset = static_cast<float>(slider.getProperties().getWithDefault("macroMod", 0.0f));
        float effectivePos = std::clamp(sliderPosProportional + modOffset, 0.0f, 1.0f);

        // Background track arc
        auto trackRadius = radius - 5.0f;
        juce::Path bgArc;
        bgArc.addCentredArc(center.x, center.y, trackRadius, trackRadius, 0.0f,
                            rotaryStartAngle, rotaryEndAngle, true);

        g.setColour(border());
        g.strokePath(bgArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // If modulated, draw the glowing modulation trail connecting base value to modulated value
        if (std::abs(modOffset) > 0.001f)
        {
            float baseAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
            float effAngle  = rotaryStartAngle + effectivePos * (rotaryEndAngle - rotaryStartAngle);

            float startMod = std::min(baseAngle, effAngle);
            float endMod   = std::max(baseAngle, effAngle);

            juce::Path modArc;
            modArc.addCentredArc(center.x, center.y, trackRadius, trackRadius, 0.0f,
                                 startMod, endMod, true);

            juce::Colour modColor = (modOffset > 0.0f) ? accentCyan() : accentViolet();

            // Glow underneath
            g.setColour(modColor.withAlpha(0.35f));
            g.strokePath(modArc, juce::PathStrokeType(7.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            // Core modulation arc
            g.setColour(modColor);
            g.strokePath(modArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Active value arc - drawn up to effectivePos so the arc visually tracks macro movement!
        auto toAngle = rotaryStartAngle + effectivePos * (rotaryEndAngle - rotaryStartAngle);
        juce::Path valArc;
        valArc.addCentredArc(center.x, center.y, trackRadius, trackRadius, 0.0f,
                             rotaryStartAngle, toAngle, true);

        juce::ColourGradient grad(accentViolet(), center.x - radius, center.y,
                                  accentCyan(), center.x + radius, center.y, false);
        g.setGradientFill(grad);
        g.strokePath(valArc, juce::PathStrokeType(4.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Inner knob dial body
        auto innerRadius = radius - 11.0f;
        g.setColour(bgCardLight());
        g.fillEllipse(center.x - innerRadius, center.y - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

        g.setColour(border().brighter(0.1f));
        g.drawEllipse(center.x - innerRadius, center.y - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 1.2f);

        // If modulated, draw a subtle notch/pip at the base position so user knows original knob setting
        if (std::abs(modOffset) > 0.001f)
        {
            float baseAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
            juce::Point<float> basePip(center.x + (innerRadius - 3.5f) * std::cos(baseAngle - juce::MathConstants<float>::halfPi),
                                       center.y + (innerRadius - 3.5f) * std::sin(baseAngle - juce::MathConstants<float>::halfPi));
            g.setColour(juce::Colour(0x99ffffff));
            g.fillEllipse(basePip.x - 2.0f, basePip.y - 2.0f, 4.0f, 4.0f);
        }

        // Active indicator pip / pointer at effectivePos (rotates in real-time with the XY pad!)
        juce::Point<float> pip(center.x + (innerRadius - 4.0f) * std::cos(toAngle - juce::MathConstants<float>::halfPi),
                               center.y + (innerRadius - 4.0f) * std::sin(toAngle - juce::MathConstants<float>::halfPi));

        juce::Colour pipColor = (std::abs(modOffset) > 0.001f)
            ? ((modOffset > 0.0f) ? accentCyan() : accentViolet())
            : accentCyan();

        g.setColour(pipColor);
        g.fillEllipse(pip.x - 3.0f, pip.y - 3.0f, 6.0f, 6.0f);

        // Subtle glow if mouse is over or actively modulated
        if (slider.isMouseOverOrDragging() || std::abs(modOffset) > 0.001f)
        {
            g.setColour(pipColor.withAlpha(0.28f));
            g.drawEllipse(center.x - innerRadius, center.y - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 2.5f);
        }
    }

    void CustomLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
    {
        juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        auto bounds = button.getLocalBounds().toFloat();
        bool isOn = button.getToggleState();

        // Minimalist switch pill
        auto pillArea = bounds.removeFromLeft(36.0f).reduced(2.0f, 7.0f);
        auto corner = pillArea.getHeight() * 0.5f;

        g.setColour(isOn ? accentViolet().withAlpha(0.35f) : bgCardLight());
        g.fillRoundedRectangle(pillArea, corner);

        g.setColour(isOn ? accentViolet() : border());
        g.drawRoundedRectangle(pillArea, corner, 1.2f);

        // Thumb dot
        float dotSize = pillArea.getHeight() - 4.0f;
        float dotX = isOn ? (pillArea.getRight() - dotSize - 2.0f) : (pillArea.getX() + 2.0f);
        float dotY = pillArea.getY() + 2.0f;

        g.setColour(isOn ? accentCyan() : textDim());
        g.fillEllipse(dotX, dotY, dotSize, dotSize);

        // Button label text
        g.setColour(isOn ? textLight() : textDim());
        g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
        g.drawText(button.getButtonText(), bounds.reduced(4.0f, 0.0f),
                   juce::Justification::centredLeft, true);
    }

    void CustomLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                        int buttonX, int buttonY, int buttonW, int buttonH,
                                        juce::ComboBox& box)
    {
        juce::ignoreUnused(isButtonDown, buttonX, buttonY, buttonW, buttonH, box);
        auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

        g.setColour(bgCardLight());
        g.fillRoundedRectangle(bounds, 4.0f);

        g.setColour(border());
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

        // Arrow
        juce::Path arrow;
        float ax = width - 14.0f;
        float ay = height * 0.5f - 2.0f;
        arrow.startNewSubPath(ax - 4.0f, ay);
        arrow.lineTo(ax, ay + 4.0f);
        arrow.lineTo(ax + 4.0f, ay);

        g.setColour(accentCyan());
        g.strokePath(arrow, juce::PathStrokeType(1.5f, juce::PathStrokeType::mitered, juce::PathStrokeType::rounded));
    }

    void CustomLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                             bool isSeparator, bool isActive, bool isHighlighted, bool isTicked,
                                             bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText,
                                             const juce::Drawable* icon, const juce::Colour* textColour)
    {
        juce::ignoreUnused(isSeparator, isActive, isTicked, hasSubMenu, shortcutKeyText, icon, textColour);

        if (isHighlighted)
        {
            g.setColour(accentViolet().withAlpha(0.35f));
            g.fillRect(area);
        }

        g.setColour(isHighlighted ? accentCyan() : textLight());
        g.setFont(juce::FontOptions(13.0f));
        g.drawText(text, area.reduced(8, 0), juce::Justification::centredLeft, true);
    }
}
