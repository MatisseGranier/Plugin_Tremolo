/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils.h"

//==============================================================================
Tremolo_test2AudioProcessor::Tremolo_test2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                     #endif
                       )
#endif
{
    castParameter(aptvs, ParameterID::mainOscFreq, mainOscFreqParam);
    castParameter(aptvs, ParameterID::mainOscAmp, mainOscAmpParam);
    castParameter(aptvs, ParameterID::tremFreq, modFreqParam);
    castParameter(aptvs, ParameterID::tremDepth, modDepthParam);
    castParameter(aptvs, "waveform", waveformParam);

    aptvs.state.addListener(this);
}

Tremolo_test2AudioProcessor::~Tremolo_test2AudioProcessor()
{
    aptvs.state.removeListener(this);

}

//==============================================================================
const juce::String Tremolo_test2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Tremolo_test2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Tremolo_test2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Tremolo_test2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Tremolo_test2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Tremolo_test2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Tremolo_test2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Tremolo_test2AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Tremolo_test2AudioProcessor::getProgramName (int index)
{
    return {};
}

void Tremolo_test2AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Tremolo_test2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    this->sampleRate = sampleRate;

    main_osc.reset();
    trem.reset();

    //1st version : Non atomic
    //main_osc.inc = freq / this->sampleRate;
    //main_osc.amplitude = 0.5;
    //trem.lfo.inc = mod_freq / this->sampleRate;
    //trem.modulationDepth = mod_amp;

    //2nd version : Atomic
    main_osc.inc = mainOscFreqParam->get() / this->sampleRate;
    main_osc.amplitude = mainOscAmpParam->get();
    trem.lfo.inc = modFreqParam->get() / sampleRate;
    trem.modulationDepth = modDepthParam->get();
    trem.lfo.waveform = waveformParam->getIndex();
}

void Tremolo_test2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Tremolo_test2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Tremolo_test2AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    bool expected = true;
    if (parametersChanged.compare_exchange_strong(expected, false))
    {
        main_osc.inc = mainOscFreqParam->get() / this->sampleRate;
        main_osc.amplitude = mainOscAmpParam->get();
        trem.lfo.inc = modFreqParam->get() / sampleRate;
        trem.modulationDepth = modDepthParam->get();
        trem.lfo.waveform = waveformParam->getIndex();
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        
        auto* output = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            //*output++ = std::sin( juce::MathConstants<float>::twoPi * 
            //    sample_index *freq / sampleRate + phase ) * 0.075;
            //sample_index++;
            *output++ = main_osc.nextSample() * trem.nextSample();
        }
    }
}

//==============================================================================
bool Tremolo_test2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Tremolo_test2AudioProcessor::createEditor()
{
    auto editor = new juce::GenericAudioProcessorEditor(*this);
    editor->setSize(500, 300);
    return editor;
    //return new Tremolo_test2AudioProcessorEditor (*this);
}

//==============================================================================
void Tremolo_test2AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Tremolo_test2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Tremolo_test2AudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout Tremolo_test2AudioProcessor::createParameterLayout()
{

    juce::AudioProcessorValueTreeState::ParameterLayout layout;


    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::mainOscFreq,
        "Main Osc freq",
        juce::NormalisableRange<float>(40.0f, 2000.0f, 0.001f, 0.3f, false ),
        660.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::mainOscAmp,
        "Main Osc amp",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f
        //juce::AudioParameterFloatAttributes().withLabel("Hz")
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::tremFreq,
        "Modulation Frequency",
        juce::NormalisableRange<float>(0.001f, 20.0f, 0.001f, 0.3f, false ),
        2.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::tremDepth,
        "Modulation Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f
        //juce::AudioParameterFloatAttributes().withLabel( "Hz" )
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "waveform", "Waveform",
        juce::StringArray{ "Sine", "Square", "Saw", "Triangle", "Random" }, 0
    ));


    return layout;
}

void Tremolo_test2AudioProcessor::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&)
{
    // parametersChanged est atomic
    parametersChanged.store(true);
};

