/*
  ==============================================================================

    Gain.cpp
    Created: 7 Nov 2026 8:32:02am
    Author:  gabriel

  ==============================================================================
*/

#include "Gain.h"

Gain::Gain(){}
Gain::~Gain(){}

void Gain::process(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            float inSample = buffer.getSample(channel, sample);
            float outSample = inSample * gain; 
            buffer.setSample(channel, sample, outSample);
        }
    }
}

void Gain::setGain(float g)
{
    gain = g;
}
