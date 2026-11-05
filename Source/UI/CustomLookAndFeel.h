#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace GranularLab
{
    class CustomLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        CustomLookAndFeel();

        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                              float sliderPosProportional, float rotaryStartAngle,
                              float rotaryEndAngle, juce::Slider& slider) override;

        void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                          int buttonX, int buttonY, int buttonW, int buttonH,
                          juce::ComboBox& box) override;

        void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                               bool isSeparator, bool isActive, bool isHighlighted, bool isTicked,
                               bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText,
                               const juce::Drawable* icon, const juce::Colour* textColour) override;

        // Custom palette
        static inline juce::Colour bgDark()       { return juce::Colour(0xff121318); }
        static inline juce::Colour bgCard()       { return juce::Colour(0xff1a1c24); }
        static inline juce::Colour bgCardLight()  { return juce::Colour(0xff222530); }
        static inline juce::Colour border()       { return juce::Colour(0xff2d3142); }
        static inline juce::Colour accentViolet() { return juce::Colour(0xff9d4edd); }
        static inline juce::Colour accentCyan()   { return juce::Colour(0xff00f0ff); }
        static inline juce::Colour accentBlue()   { return juce::Colour(0xff3a86ff); }
        static inline juce::Colour textLight()    { return juce::Colour(0xfff0f4f8); }
        static inline juce::Colour textDim()      { return juce::Colour(0xff8c96a8); }
    };
}
