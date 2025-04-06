/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Oscillator.h"
#include "Tremolo.h"

namespace ParameterID
{
        #define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);

        PARAMETER_ID(mainOscFreq)
        PARAMETER_ID(mainOscAmp)
        PARAMETER_ID(tremFreq)
        PARAMETER_ID(tremDepth)

        #undef PARAMETER_ID
}

//==============================================================================
/**
*/
class Tremolo_test2AudioProcessor  : public juce::AudioProcessor, private juce::ValueTree::Listener
{
public:
    //==============================================================================
    Tremolo_test2AudioProcessor();
    ~Tremolo_test2AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState aptvs{ *this, nullptr, "Parameters", createParameterLayout()};

    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tremolo_test2AudioProcessor)

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();


    int sample_index = 0;
    float freq = 440.f;
    float phase = 0.f;
    double sampleRate;

    float mod_amp = 0.75;
    //float mod_freq = 2.f;

    Oscillator main_osc;
    Tremolo trem;

    juce::AudioParameterFloat* mainOscFreqParam; // float freq = 440.f;
    juce::AudioParameterFloat* mainOscAmpParam;
    juce::AudioParameterFloat* modFreqParam; // float mod_freq = 2.f;
    juce::AudioParameterFloat* modDepthParam; // was float mod_amp = 0.75;
    juce::AudioParameterChoice* waveformParam;

    std::atomic<bool> parametersChanged{ false };
};