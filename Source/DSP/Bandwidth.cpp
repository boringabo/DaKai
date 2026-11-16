/*
  ==============================================================================

    Bandwidth.cpp
    Created: 16 Nov 2026 2:55:42pm
    Author:  gabriel

  ==============================================================================
*/

#include "Bandwidth.h"

Bandwidth::Bandwidth() {};
Bandwidth::~Bandwidth() {};

void Bandwidth::prepare(double inHostSampleRate, int numChannels)
{
    hostSampleRate = inHostSampleRate;
    phases.assign(numChannels, 0.0f);
    heldValues.assign(numChannels, 0.0f);
    updateIncrement();
}

void Bandwidth::setTargetSampleRate(float newTargetRate)
{
    float maxRate = juce::jmin(44100.0f, static_cast<float>(hostSampleRate));
    targetSampleRate = juce::jlimit(7500.0f, maxRate, newTargetRate);
    updateIncrement();
}

void Bandwidth::updateIncrement()
{
    if (hostSampleRate > 0.0f)
    {
        phaseIncrement = targetSampleRate / static_cast<float>(hostSampleRate);
    }
}

float Bandwidth::processSample(float inSample, int channel)
{
    if (channel >= static_cast<int>(heldValues.size()))
        return inSample;

    phases[channel] += phaseIncrement;

    if (phases[channel] >= 1.0f)
    {
        phases[channel] = phases[channel] - 1.0f;
        heldValues[channel] = inSample;
    }

    return heldValues[channel];
}

void Bandwidth::process(juce::AudioBuffer<float>& buffer)
{
    for(int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        for(int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            float inSample = buffer.getSample(channel, sample);
            float lowerRateSample = processSample(inSample, channel);
            buffer.setSample(channel, sample, lowerRateSample); 
        }
    }
}
