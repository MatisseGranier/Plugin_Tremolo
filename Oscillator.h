/*
  ==============================================================================

    oscillateur.h
    Created: 4 Feb 2025 4:51:14pm
    Author:  matis

  ==============================================================================
*/

#pragma once

//#include "PluginProcessor.h"
//#include "PluginEditor.h"
//#include "tremolo.h"

class Oscillator
{
private:
    float previous_modulo = 0.0f;
    float randomValue = 0.0f;

public:
    float inc;
    float modulo_counter;
    float amplitude;
    int waveform = 0;


    void reset()
    {
        modulo_counter = 0.0f;
    }

    void setAmplitude(float amplitude)
    {
        this->amplitude = amplitude;
    }

    float nextSample()
    {
        float output = 0.0f;

        modulo_counter += inc;
        if (modulo_counter >= 1.0f)
            modulo_counter -= 1.0f;

        if (waveform == 0) {
            output = std::sin(juce::MathConstants<float>::twoPi * modulo_counter);
        }
        else if (waveform == 1) {
            output = (std::sin(juce::MathConstants<float>::twoPi * modulo_counter) > 0.0f) ? 1.0f : -1.0f;
        }
        else if (waveform == 2) {
            output = 2.0f * (modulo_counter - std::floor(modulo_counter + 0.5f));
        }
        else if (waveform == 3) {
            output = 2.0f * std::abs(2.0f * (modulo_counter - std::floor(modulo_counter + 0.5f))) - 1.0f;
        }
        else if (waveform == 4) {
            // LFO aléatoire : nouvelle valeur à chaque cycle
            if (modulo_counter < previous_modulo)
                randomValue = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;

            output = randomValue;

            previous_modulo = modulo_counter;
        }

        return output * amplitude;

    }
};
