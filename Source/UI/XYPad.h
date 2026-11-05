#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "CustomLookAndFeel.h"
#include "../Common/Parameters.h"

namespace GranularLab
{
    class XYPad : public juce::Component, public juce::Timer
    {
    public:
        XYPad(juce::AudioProcessorValueTreeState& apvts);
        ~XYPad() override;

        void paint(juce::Graphics& g) override;
        void resized() override;

        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void timerCallback() override;

        void setTargetParamX(int targetIdx);
        void setTargetParamY(int targetIdx);
        void randomize();

    private:
        juce::AudioProcessorValueTreeState& apvts;
        std::atomic<float>* xParam { nullptr };
        std::atomic<float>* yParam { nullptr };

        float currentX { 0.5f };
        float currentY { 0.5f };
        float targetX { 0.5f };
        float targetY { 0.5f };

        juce::Rectangle<float> padArea;
        juce::ComboBox comboTargetX;
        juce::ComboBox comboTargetY;
        juce::TextButton btnRandom { "RND" };

        void updateFromMouse(const juce::Point<float>& pos);
        void applyXYToTargets();

        juce::String getTargetName(int idx) const;
    };
}
