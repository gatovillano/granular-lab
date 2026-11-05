#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../DSP/SampleBuffer.h"
#include <vector>

namespace GranularSynthLab
{
    class SynthProcessor;

    class SynthWaveformView : public juce::Component,
                              public juce::FileDragAndDropTarget
    {
    public:
        explicit SynthWaveformView(SynthProcessor& processor);
        ~SynthWaveformView() override = default;

        void paint(juce::Graphics& g) override;
        void resized() override;
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;

        // File drag and drop support
        bool isInterestedInFileDrag(const juce::StringArray& files) override;
        void filesDropped(const juce::StringArray& files, int x, int y) override;
        void fileDragEnter(const juce::StringArray& files, int x, int y) override;
        void fileDragExit(const juce::StringArray& files) override;

        void refresh();

    private:
        SynthProcessor& processor;
        std::vector<float> minPeaksL;
        std::vector<float> maxPeaksL;
        std::vector<float> minPeaksR;
        std::vector<float> maxPeaksR;
        bool isDragOver { false };

        void updatePositionFromMouse(float mouseX);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthWaveformView)
    };
}
