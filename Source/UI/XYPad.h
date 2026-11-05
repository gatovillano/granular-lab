#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "CustomLookAndFeel.h"
#include "../Common/Parameters.h"
#include <array>

namespace GranularLab
{
    class XYPad : public juce::Component, public juce::Timer
    {
    public:
        enum class ViewMode
        {
            Pad = 0,
            MapX,
            MapY
        };

        explicit XYPad(juce::AudioProcessorValueTreeState& apvts);
        ~XYPad() override;

        void paint(juce::Graphics& g) override;
        void resized() override;

        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void timerCallback() override;

        void randomize();
        void setViewMode(ViewMode mode);

    private:
        juce::AudioProcessorValueTreeState& apvts;
        std::atomic<float>* xParam { nullptr };
        std::atomic<float>* yParam { nullptr };

        float currentX { 0.5f };
        float currentY { 0.5f };
        float targetX { 0.5f };
        float targetY { 0.5f };

        ViewMode activeView { ViewMode::Pad };

        // Header controls
        juce::TextButton btnTabPad { "PAD" };
        juce::TextButton btnTabMapX { "MAP X" };
        juce::TextButton btnTabMapY { "MAP Y" };
        juce::TextButton btnRandom { "RND" };

        juce::Rectangle<float> padArea;
        juce::Rectangle<float> chipXArea;
        juce::Rectangle<float> chipYArea;

        // Slot UI components for Macro X (4 slots)
        struct SlotUI
        {
            juce::ComboBox targetCombo;
            juce::Slider depthSlider;
            juce::Label slotLabel;
            std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> targetAttach;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttach;
        };

        std::array<SlotUI, 4> xSlots;
        std::array<SlotUI, 4> ySlots;

        void updateFromMouse(const juce::Point<float>& pos);
        void setupSlotUI(SlotUI& slot, int slotIndex, bool isXAxis);
        void updateVisibility();

        juce::String getActiveMappingsSummary(bool isXAxis) const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XYPad)
    };
}
