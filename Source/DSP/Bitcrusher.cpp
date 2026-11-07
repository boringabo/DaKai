/*
  ==============================================================================

    Bitcrusher.cpp
    Created: 7 Nov 2026 10:52:33am
    Author:  gabriel

  ==============================================================================
*/

#include "Bitcrusher.h"
#include <cmath>

Bitcrusher::Bitcrusher()
{
    numLevels = std::pow(2.0f, bitDepth - 1);
}

Bitcrusher::~Bitcrusher(){};

void Bitcrusher::process(juce::AudioBuffer<float>& buffer) {
    for(int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        for(int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            float inSample = buffer.getSample(channel, sample);
            float quantizedSample = processSample(inSample);
            buffer.setSample(channel, sample, quantizedSample);
        }
    }
}

float Bitcrusher::processSample(float inputSample) {
    float scaled = inputSample * numLevels;
    float quantized = std::round(scaled);
    return quantized / numLevels;
}
