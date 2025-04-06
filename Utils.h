/*
  ==============================================================================

    Utils.h
    Created: 26 Feb 2025 11:00:24pm
    Author:  matis

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

template<typename T>
inline static void castParameter(juce::AudioProcessorValueTreeState&
    aptvs, const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(aptvs.getParameter(id.getParamID())
        );
    jassert(destination);
}
