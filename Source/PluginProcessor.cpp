/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Globals.h"

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
        std::make_unique<juce::AudioParameterFloat>("mallet_stiff", "Mallet Stifness",juce::NormalisableRange<float>(400.0f, 5000.0f, 0.1f, 0.3f) , 1500.0f),

        std::make_unique<juce::AudioParameterBool>("a_on", "A ON", true),
        std::make_unique<juce::AudioParameterChoice>("a_model", "A Model", StringArray { "String", "Beam", "Squared", "Membrane", "Plate", "Drumhead", "Marimba", "Open Tube", "Closed Tube" }, 0),
        std::make_unique<juce::AudioParameterChoice>("a_partials", "A Partials", StringArray { "4", "8", "16", "32", "64" }, 3),
        std::make_unique<juce::AudioParameterFloat>("a_decay", "A Decay",juce::NormalisableRange<float>(0.01f, 100.0f, 0.01f, 0.2f) , 1.0f),
        std::make_unique<juce::AudioParameterFloat>("a_damp", "A Material", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("a_tone", "A Tone", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("a_hit", "A HitPos", 0.02f, 0.5f, 0.26f),
        std::make_unique<juce::AudioParameterFloat>("a_rel", "A Release", 0.0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("a_inharm", "A Inharmonic",juce::NormalisableRange<float>(0.0001f, 1.0f, 0.001f, 0.3f), 0.0001f),
        std::make_unique<juce::AudioParameterFloat>("a_ratio", "A Ratio",juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.3f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>("a_cut", "A LowCut",juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 20.0f),
        std::make_unique<juce::AudioParameterFloat>("a_radius", "A Tube Radius", 0.0f, 100.0f, 50.0f),

        std::make_unique<juce::AudioParameterBool>("b_on", "B ON", true),
        std::make_unique<juce::AudioParameterChoice>("b_model", "B Model", StringArray { "String", "Beam", "Squared", "Membrane", "Plate", "Drumhead", "Marimba", "Open Tube", "Closed Tube" }, 0),
        std::make_unique<juce::AudioParameterChoice>("b_partials", "B Partials", StringArray { "4", "8", "16", "32", "64" }, 3),
        std::make_unique<juce::AudioParameterFloat>("b_decay", "B Decay",juce::NormalisableRange<float>(0.01f, 100.0f, 0.01f, 0.2f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>("b_damp", "B Material", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("b_tone", "B Tone", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("b_hit", "B HitPos", 0.02f, 0.5f, 0.26f),
        std::make_unique<juce::AudioParameterFloat>("b_rel", "B Release", 0.0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("b_inharm", "B Inharmonic",juce::NormalisableRange<float>(0.0001f, 1.0f, 0.001f, 0.3f), 0.0001f),
        std::make_unique<juce::AudioParameterFloat>("b_ratio", "B Ratio",juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.3f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>("b_cut", "B LowCut",juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 20.0f),
        std::make_unique<juce::AudioParameterFloat>("b_radius", "B Tube Radius", 0.0f, 100.0f, 50.0f),
        
        std::make_unique<juce::AudioParameterFloat>("noise_mix", "Noise Mix", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_res", "Noise Resonance", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterChoice>("noise_filter_mode", "Noise Filter Mode", StringArray {"LP", "BP", "HP"}, 2),
        std::make_unique<juce::AudioParameterFloat>("noise_filter_freq", "Noise Filter Freq",juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 20.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_filter_q", "Noise Filter Q", 0.707f, 4.0f, 0.707f),
        std::make_unique<juce::AudioParameterFloat>("noise_att", "Noise Attack",juce::NormalisableRange<float>(1.0f, 5000.0f, 1.0f, 0.3f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_dec", "Noise Decay",juce::NormalisableRange<float>(1.0f, 5000.0f, 1.0f, 0.3f), 500.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_sus", "Noise Sustain", 0.0f, 100.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_rel", "Noise Release",juce::NormalisableRange<float>(1.0f, 5000.0f, 1.0f, 0.3f), 500.0f),

        std::make_unique<juce::AudioParameterFloat>("vel_mallet_mix", "Vel Mallet Mix", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_mallet_res", "Vel Mallet Resonance", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_mallet_stiff", "Vel Mallet Stiffness", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_noise_mix", "Vel Noise Mix", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_noise_res", "Vel Noise Resonance", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_a_decay", "Vel A Decay", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_a_hit", "Vel A Hit", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_a_inharm", "Vel A Inharmonic", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_b_decay", "Vel B Decay", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_b_hit", "Vel B Hit", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_b_inharm", "Vel B Inharmonic", 0.0f, 1.0f, 0.0f),

        std::make_unique<juce::AudioParameterChoice>("couple", "Coupling", StringArray {"A+B", "A>B"}, 1),
        std::make_unique<juce::AudioParameterFloat>("ab_mix", "A+B Mix", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("ab_split", "A>B Split", juce::NormalisableRange<float>(1.0f, 100.0f, 0.01f, 0.3f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>("gain", "Res Gain", -24.0f, 24.0f, 0.0f),
    })
#endif
{
    juce::PropertiesFile::Options options{};
    options.applicationName = ProjectInfo::projectName;
    options.filenameSuffix = ".settings";
    options.osxLibrarySubFolder = "Application Support";
    options.storageFormat = PropertiesFile::storeAsXML;
    settings.setStorageParameters(options);

    for (auto* param : getParameters()) {
        param->addListener(this);
    }

    for (int i = 0; i < globals::MAX_POLYPHONY; ++i) {
        voices.push_back(Voice());
    }

    loadSettings();
    onSlider();
}

void RipplerXAudioProcessor::parameterValueChanged (int parameterIndex, float newValue)
{
    (void)parameterIndex; // suppress unused warnings
    (void)newValue;
    paramChanged = true;
}

void RipplerXAudioProcessor::parameterGestureChanged (int parameterIndex, bool gestureIsStarting)
{
    (void)parameterIndex;
    (void)gestureIsStarting;
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
    // clear all voices
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
    (void)index;
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
    (void)index;
    (void)newName;
}

//==============================================================================
void RipplerXAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    (void)sampleRate;
    (void)samplesPerBlock;
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

float RipplerXAudioProcessor::normalizeVolSlider(float val) 
{ 
    return val * 60.0f / 100.0f - 60.0f;
}

void RipplerXAudioProcessor::onNote(MIDIMsg msg)
{
    auto srate = getSampleRate();
    Voice& voice = voices[nvoice];
    nvoice = (nvoice + 1) % polyphony;

    voice.trigger(srate, msg.note, msg.vel / 127.0);

    //PolyMsg note;
    //note.note = msg.note;
    //note.vel = msg.vel / 127.0;
    //note.freq = note2freq(msg.note) / getSampleRate();
    //note.impulse = 1.0;
    //note.elapsed = (int)(getSampleRate()/10.0); // countdown (100ms)
    //note.nvoice = nvoice;
    //note.release = false;
    //notes.push_back(note);

    //nvoice = (nvoice + 1) % polyphony;
    /*
    * click_f = min(5000, exp(log(click_freq) + vel / 127 * vel_click_freq * (log(5000) - log(40))));
    printf("nstring == %d ? (\n", i-1);
    printf("  s%02d.vel = ptr[1];\n", i); -> avoices[note.nvoice].vel = note.vel 
    printf("  s%02d.string_init(freq, 0, 1);\n", i); -> avoices[note.nvoice].init()
    printf("  s%02d.active = 1; s%02d.silence = 0;\n", i, i); -> avoices[note.nvoice].active = 1; avoices[note.nvoice].silence = 0;
    printf("  click_filter%02d.rbj_bp(click_f, 0.707);\n",i); -> malletFilters[note.nvoice].bp(click_f, 0.707);
    printf("  b_s%02d.string_init(freq, 0, 0);\n", i);
    printf("  b_s%02d.active = 1; b_s%02d.silence = 0;\n", i, i);
    printf("  noise_env%02d.env_a(1);\n", i); -> noiseEnvs[note.nstring].attack(1)
    printf("  noise_env%02d.vel = ptr[1];\n", i); -> noiseEnvs[note.nstring].vel = note.vel
    printf(");\n");
    */
}

void RipplerXAudioProcessor::offNote(MIDIMsg msg)
{
    for (int i = 0; i < polyphony; ++i) {
        Voice& voice = voices[i];
        if (voice.note == msg.note) {
            voice.release();
        }
    }
    /*
    for (auto& note : notes) {
        if (note.note == msg.note) {
            //s%02d.string_init(s%02d.f0, 1, 1);  -> avoices[note.nstring].string_init()
            //b_s%02d.string_init(b_s%02d.f0, 1, 0);\n", i-1, i, i, i, i); -> bvoices[note.nstring].string_init()
            //noise_env%02d.env_r();\n", i-1, i); -> noiseEnvs[note.nstring].init()
            note.release = true;
            break;
        }
    }
    */
}

void RipplerXAudioProcessor::onSlider()
{
    auto srate = getSampleRate();
    auto mallet_stiff = (double)params.getRawParameterValue("mallet_stiff")->load();
    auto noise_filter_freq = (double)params.getRawParameterValue("noise_filter_freq")->load();
    auto noise_filter_mode = (int)params.getRawParameterValue("noise_filter_mode")->load();
    auto noise_filter_q = (double)params.getRawParameterValue("noise_filter_q")->load();
    auto noise_att = (double)params.getRawParameterValue("noise_att")->load();
    auto noise_dec = (double)params.getRawParameterValue("noise_dec")->load();
    auto noise_sus = (double)normalizeVolSlider(params.getRawParameterValue("noise_att")->load());
    auto noise_rel = (double)params.getRawParameterValue("noise_rel")->load();

    for (int i = 0; i < polyphony; i++) {
        Voice& voice = voices[i];
        voice.noise.init(srate, noise_filter_mode, noise_filter_freq, noise_filter_q, noise_att, noise_dec, noise_sus, noise_rel);
        voice.mallet.setFreq(srate, mallet_stiff);
    }
}

bool RipplerXAudioProcessor::supportsDoublePrecisionProcessing() const
{
    return true;
}

void RipplerXAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    processBlockByType(buffer, midiMessages);
}

void RipplerXAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    processBlockByType(buffer, midiMessages);
}

template <typename FloatType>
void RipplerXAudioProcessor::processBlockByType (AudioBuffer<FloatType>& buffer, juce::MidiBuffer& midiMessages)
{
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    auto mallet_mix = (double)params.getRawParameterValue("mallet_mix")->load();
    auto vel_mallet_mix = (double)params.getRawParameterValue("vel_mallet_mix")->load();

    // remove midi messages that have been processed
    midi.erase(std::remove_if(midi.begin(), midi.end(), [](const MIDIMsg& msg) {
        return msg.offset < 0;
    }), midi.end());

    if (paramChanged) {
        onSlider();
        paramChanged = false;
    }

    // Process new MIDI messages
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    for (const auto metadata : midiMessages)
    {
        juce::MidiMessage message = metadata.getMessage();
        if (message.isNoteOn() || message.isNoteOff()) {
            midi.push_back({ // queue midi message
                metadata.samplePosition, 
                message.isNoteOn(),
                message.getNoteNumber(),
                message.getVelocity()
            });
        }
        else if (message.isAllNotesOff()) {
            // TODO
        }
        else if (message.isAllSoundOff()) {
            // TODO clear all voices
        }
    }
    
    for (int sample = 0; sample < numSamples; ++sample) {
        // process midi queue
        for (auto& msg : midi) {
            if (msg.offset == 0) {
                if (msg.isNoteon) {
                    onNote(msg);
                }
                else {
                    offNote(msg);
                }
            }
            msg.offset -= 1;
        }

        //double resOut[16] = {}; // output to resonators, per voice
        double dirOut = 0.0; // direct output

        for (int i = 0; i < polyphony; ++i) {
            Voice& voice = voices[i];
            auto msample = voice.mallet.process();
            if (msample) {
                dirOut = msample * fmin(1.0, mallet_mix + vel_mallet_mix * voice.vel);
            }
        }

        // process notes being played
        //for (auto& note : notes) {
        //    if (note.elapsed > 0) {
        //        note.elapsed -= 1;
        //        double noteOut = 0.0; // malletFilters[note.nvoice].df2(note.impulse) * 2.0;
                //malletDirOut += noteOut * min(1, mallet_mix + vel_mallet_mix * note.vel);
                //malletResOut[note.nvoice] = noteOut * min(1, mallet_res + vel_mallet_res * note.vel);
        //    }
       // }

        //double aOut = 0.0; // resonator A out
        //double bOut = 0.0; // resonator B out

        // process noise
        // loop(i=1;npolyphony,
        // printf("noise_env%02d.state > 0 ? (\n", i);
        // printf("  noise_env%02d.env_process();\n", i);
        // printf("  n = noise_gen%02d.process_noise();\n", i);
        // printf("  n *= noise_env%02d.env;\n", i);
        // printf("  out_noise += n * min(1, noise_mix + vel_noise_mix * noise_env%02d.vel);\n", i);
        // printf("  outm_%02d += n * min(1, noise_res + vel_noise_res * noise_env%02d.vel);\n", i, i);
        // printf(");\n");



        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            buffer.setSample(channel, sample, static_cast<FloatType>(dirOut));
        }
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
