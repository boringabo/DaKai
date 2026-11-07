/*
  ==============================================================================

    Bitcrusher.h
    Created: 7 Nov 2026 10:52:33am
    Author:  gabriel

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Bitcrusher
{
    public:
        Bitcrusher();
        ~Bitcrusher();

        void process(juce::AudioBuffer<float>& buffer);
        float processSample(float inputSample);

    private:
        int bitDepth = 12;
        float numLevels = 2048.0f;
};
