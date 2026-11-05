#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "UI/CustomLookAndFeel.h"
#include "Common/LockFreeQueue.h"

namespace GranularSynthLab
{
    class SynthProcessor;

    class SynthParticleView : public juce::Component, public juce::Timer
    {
    public:
        explicit SynthParticleView(SynthProcessor& processor);
        ~SynthParticleView() override;

        void paint(juce::Graphics& g) override;
        void timerCallback() override;

    private:
        SynthProcessor& processor;
        GranularLab::VisualSnapshot currentSnapshot;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthParticleView)
    };
}
