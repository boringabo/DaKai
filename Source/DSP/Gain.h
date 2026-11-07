/*
  ==============================================================================

    Gain.h
    Created: 7 Nov 2026 8:32:02am
    Author:  gabriel

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Gain {
    public:
        Gain();
        ~Gain();
        void process(juce::AudioBuffer<float>& buffer);
        void setGain(float gain);

    private:
        float gain {1.0f};
};
