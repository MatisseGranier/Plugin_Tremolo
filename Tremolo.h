/*
  ==============================================================================

    Tremolo.h
    Created: 13 Feb 2025 3:58:25pm
    Author:  matis

  ==============================================================================
*/

#pragma once

#include "Oscillator.h"

class Tremolo
{
public:
    Oscillator lfo;
    float modulationDepth;

    void reset()
    {
        lfo.reset();
        lfo.amplitude = 1.0;
    }

    void setFrequency(float tremoloFreq, double sampleRate)
    {
        lfo.inc = tremoloFreq / sampleRate;
    }

    float nextSample()
    {
        lfo.setAmplitude(modulationDepth);
        return (1 - modulationDepth) + lfo.nextSample();
    }

};