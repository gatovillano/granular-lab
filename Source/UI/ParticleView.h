#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../DSP/GranularEngine.h"
#include "CustomLookAndFeel.h"

namespace GranularLab
{
    class ParticleView : public juce::Component, public juce::Timer
    {
    public:
        ParticleView(GranularEngine& engine);
        ~ParticleView() override;

        void paint(juce::Graphics& g) override;
        void timerCallback() override;

    private:
        GranularEngine& granularEngine;
        VisualSnapshot currentSnapshot;
    };
}
