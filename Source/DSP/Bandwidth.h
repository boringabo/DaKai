/*
  ==============================================================================

    Bandwidth.h
    Created: 16 Nov 2026 2:55:42pm
    Author:  gabriel

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Bandwidth
{
    public:
        Bandwidth();
        ~Bandwidth();

        void process(juce::AudioBuffer<float>& buffer);
        float processSample(float inSample, int channel);
        void prepare(double inHostSampleRate, int numChannels);
        void setTargetSampleRate(float newTargetRate);
        
    private:
        void updateIncrement();

        double hostSampleRate = 44100.0;
        float targetSampleRate {44100.0f};
        float phaseIncrement = 1.0f;
        std::vector<float> phases;
        std::vector<float> heldValues;
};
