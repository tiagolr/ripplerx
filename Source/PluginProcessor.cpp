/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RipplerXAudioProcessor::RipplerXAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    , settings{}
    , params(*this, &undoManager, "PARAMETERS", {
        std::make_unique<juce::AudioParameterFloat>("mallet_mix", "Mallet Mix", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("mallet_res", "Mallet Resonance", 0.0f, 1.0f, 0.8f),
        std::make_unique<juce::AudioParameterFloat>("mallet_stiff", "Mallet Stifness",juce::NormalisableRange<float>(400.0f, 5000.0f, 0.5f, 1.0f) , 1500.0f),

        std::make_unique<juce::AudioParameterFloat>("vel_mallet_res", "Vel Mallet Resonance", 0.0f, 1.0f, 0.0f),
    })
#endif
{
    juce::PropertiesFile::Options options{};
    options.applicationName = ProjectInfo::projectName;
    options.filenameSuffix = ".settings";
    options.osxLibrarySubFolder = "Application Support";
    options.storageFormat = PropertiesFile::storeAsXML;
    settings.setStorageParameters(options);

    loadSettings();
}

RipplerXAudioProcessor::~RipplerXAudioProcessor()
{
}

void RipplerXAudioProcessor::loadSettings () 
{
    if (auto* file = settings.getUserSettings()) {
        scale = (float)file->getDoubleValue("scale", 1.0f);
        polyphony = file->getIntValue("polyphony", 8);
    }
}

void RipplerXAudioProcessor::saveSettings ()
{
    if (auto* file = settings.getUserSettings()) {
        file->setValue("scale", scale);
        file->setValue("polyphony", polyphony);
    }
    settings.saveIfNeeded();
}

void RipplerXAudioProcessor::setPolyphony(int value)
{
    polyphony = value;
    saveSettings();
}

void RipplerXAudioProcessor::setScale(float value)
{
    scale = value;
    saveSettings();
}

//==============================================================================
const juce::String RipplerXAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RipplerXAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RipplerXAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RipplerXAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RipplerXAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RipplerXAudioProcessor::getNumPrograms()
{
    return 3;
}

int RipplerXAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RipplerXAudioProcessor::setCurrentProgram (int index)
{
    DBG("SET CURRENT PROGRAM");
}

const juce::String RipplerXAudioProcessor::getProgramName (int index)
{
    return index == 0 ? "Init"
        : index == 1 ? "Test"
        : "Other";
}

void RipplerXAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RipplerXAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void RipplerXAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RipplerXAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void RipplerXAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Process MIDI
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    for (const auto metadata : midiMessages)
    {
        juce::MidiMessage message = metadata.getMessage();
        if (message.isNoteOn()) {
            DBG("NOTE!!!");
        }
    }

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
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool RipplerXAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RipplerXAudioProcessor::createEditor()
{
    return new RipplerXAudioProcessorEditor (*this);
}

//==============================================================================
void RipplerXAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = params.copyState();
    std::unique_ptr<juce::XmlElement>xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void RipplerXAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement>xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(params.state.getType()))
            params.replaceState(juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RipplerXAudioProcessor();
}
