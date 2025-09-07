// Copyright 2025 tilr

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Globals.h"

RipplerXAudioProcessor::RipplerXAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
         .withInput("Sidechain", juce::AudioChannelSet::stereo(), true)
         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
     )
    , settings{}
    , params(*this, &undoManager, "PARAMETERS", {
        std::make_unique<juce::AudioParameterChoice>("mallet_type", "Mallet Type", StringArray { "Impulse","Reserved","Reserved","Reserved","Reserved","Reserved","Reserved","Reserved","Reserved","Reserved","Reserved","User File","Click 1","Click 2","Click 3","Click 4","Click 5","Click 6" }, 0),
        std::make_unique<juce::AudioParameterFloat>("mallet_pitch", "Mallet Pitch", -24.0f, 24.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("mallet_filter", "Mallet Filter", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("mallet_mix", "Mallet Mix", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("mallet_res", "Mallet Resonance", 0.0f, 1.0f, 0.8f),
        std::make_unique<juce::AudioParameterFloat>("mallet_stiff", "Mallet Stifness",juce::NormalisableRange<float>(100.0f, 5000.0f, 1.0f, 0.3f) , 600.0f),

        std::make_unique<juce::AudioParameterBool>("a_on", "A ON", true),
        std::make_unique<juce::AudioParameterChoice>("a_model", "A Model", StringArray { "String", "Beam", "Squared", "Membrane", "Plate", "Drumhead", "Marimba", "Open Tube", "Closed Tube", "Marimba2", "Bell", "Djembe" }, 0),
        std::make_unique<juce::AudioParameterChoice>("a_partials", "A Partials", StringArray { "4", "8", "16", "32", "64", "1", "2"}, 3),
        std::make_unique<juce::AudioParameterFloat>("a_decay", "A Decay",juce::NormalisableRange<float>(0.01f, 100.0f, 0.01f, 0.2f) , 1.0f),
        std::make_unique<juce::AudioParameterFloat>("a_damp", "A Material", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("a_tone", "A Tone", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("a_hit", "A HitPos", 0.02f, 0.5f, 0.26f),
        std::make_unique<juce::AudioParameterFloat>("a_rel", "A Release", 0.001f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("a_inharm", "A Inharmonic",juce::NormalisableRange<float>(0.0001f, 1.0f, 0.0001f, 0.3f), 0.0001f),
        std::make_unique<juce::AudioParameterFloat>("a_ratio", "A Ratio",juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.3f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>("a_cut", "A LowCut", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("a_radius", "A Tube Radius", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("a_coarse", "A coarse pitch", juce::NormalisableRange<float>(-48.0f, 48.0f, 1.0f, 1.0f), 0.0f),
        std::make_unique<juce::AudioParameterFloat>("a_fine", "A fine pitch", juce::NormalisableRange<float>(-99.0f, 99.0f, 1.0f, 1.0f), 0.0f),

        std::make_unique<juce::AudioParameterBool>("b_on", "B ON", false),
        std::make_unique<juce::AudioParameterChoice>("b_model", "B Model", StringArray { "String", "Beam", "Squared", "Membrane", "Plate", "Drumhead", "Marimba", "Open Tube", "Closed Tube", "Marimba2", "Bell", "Djembe" }, 0),
        std::make_unique<juce::AudioParameterChoice>("b_partials", "B Partials", StringArray { "4", "8", "16", "32", "64", "1", "2"}, 3),
        std::make_unique<juce::AudioParameterFloat>("b_decay", "B Decay",juce::NormalisableRange<float>(0.01f, 100.0f, 0.01f, 0.2f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>("b_damp", "B Material", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("b_tone", "B Tone", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("b_hit", "B HitPos", 0.02f, 0.5f, 0.26f),
        std::make_unique<juce::AudioParameterFloat>("b_rel", "B Release", 0.001f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("b_inharm", "B Inharmonic",juce::NormalisableRange<float>(0.0001f, 1.0f, 0.0001f, 0.3f), 0.0001f),
        std::make_unique<juce::AudioParameterFloat>("b_ratio", "B Ratio",juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.3f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>("b_cut", "B LowCut", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("b_radius", "B Tube Radius", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("b_coarse", "B coarse pitch", juce::NormalisableRange<float>(-48.0f, 48.0f, 1.0f, 1.0f), 0.0f),
        std::make_unique<juce::AudioParameterFloat>("b_fine", "B fine pitch", juce::NormalisableRange<float>(-99.0f, 99.0f, 1.0f, 1.0f), 0.0f),

        std::make_unique<juce::AudioParameterFloat>("noise_osc", "Noise DC", juce::NormalisableRange<float>(0.0f, 1.0f, 0.0001f, 1.0f), 0.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_mix", "Noise Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.0001f, 0.3f), 0.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_res", "Noise Resonance", juce::NormalisableRange<float>(0.0f, 1.0f, 0.0001f, 0.3f), 0.0f),
        std::make_unique<juce::AudioParameterChoice>("noise_filter_mode", "Noise Filter Mode", StringArray {"LP", "BP", "HP"}, 2),
        std::make_unique<juce::AudioParameterFloat>("noise_filter_freq", "Noise Filter Freq",juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 20.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_filter_q", "Noise Filter Q", 0.707f, 4.0f, 0.707f),
        std::make_unique<juce::AudioParameterFloat>("noise_att", "Noise Attack",juce::NormalisableRange<float>(1.0f, 20000.0f, 1.0f, 0.3f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_dec", "Noise Decay",juce::NormalisableRange<float>(1.0f, 20000.0f, 1.0f, 0.3f), 500.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_sus", "Noise Sustain", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_rel", "Noise Release",juce::NormalisableRange<float>(1.0f, 20000.0f, 1.0f, 0.3f), 500.0f),
        std::make_unique<juce::AudioParameterFloat>("noise_att_ten", "Noise Attack Tension", -1.f, 1.f, 0.4f),
        std::make_unique<juce::AudioParameterFloat>("noise_dec_ten", "Noise Decay Tension", -1.f, 1.f, 0.4f),
        std::make_unique<juce::AudioParameterFloat>("noise_rel_ten", "Noise Release Tension", -1.f, 1.f, 0.4f),

        std::make_unique<juce::AudioParameterFloat>("vel_mallet_mix", "Vel Mallet Mix", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_mallet_res", "Vel Mallet Resonance", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_mallet_stiff", "Vel Mallet Stiffness", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_noise_mix", "Vel Noise Mix", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_noise_res", "Vel Noise Resonance", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_noise_freq", "Vel Noise Frequency", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_noise_att", "Vel Noise Attack", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_noise_dec", "Vel Noise Decay", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_noise_sus", "Vel Noise Sustain", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_noise_rel", "Vel Noise Release", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_noise_q", "Vel Noise Q", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_a_decay", "Vel A Decay", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_a_hit", "Vel A Hit", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_a_inharm", "Vel A Inharmonic", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_a_damp", "Vel A Material", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_a_tone", "Vel A Tone", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_b_decay", "Vel B Decay", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_b_hit", "Vel B Hit", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_b_inharm", "Vel B Inharmonic", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_b_damp", "Vel B Material", -1.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("vel_b_tone", "Vel B Tone", -1.0f, 1.0f, 0.0f),

        std::make_unique<juce::AudioParameterChoice>("couple", "Coupling", StringArray {"A+B", "A>B"}, 0),
        std::make_unique<juce::AudioParameterFloat>("ab_mix", "A+B Mix", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("ab_split", "A>B Split", juce::NormalisableRange<float>(0.01f, 1.0f, 0.001f, 0.5f), 0.01f),
        std::make_unique<juce::AudioParameterFloat>("gain", "Res Gain", -24.0f, 24.0f, 0.0f),
        std::make_unique<juce::AudioParameterInt>("bend_range", "PitchBend Range", 1, 24, 2),
        std::make_unique<juce::AudioParameterBool>("stereoizer", "Stereoizer", true),
        std::make_unique<juce::AudioParameterBool>("reuse_voices", "Reuse Voices", false),
    }),
    mtsClientPtr{nullptr}
#endif
{
    juce::PropertiesFile::Options options{};
    options.applicationName = ProjectInfo::projectName;
    options.filenameSuffix = ".settings";
#if defined(JUCE_LINUX) || defined(JUCE_BSD)
    options.folderName = "~/.config/RipplerX";
#endif
    options.osxLibrarySubFolder = "Application Support";
    options.storageFormat = PropertiesFile::storeAsXML;
    settings.setStorageParameters(options);

    for (auto* param : getParameters()) {
        param->addListener(this);
    }

    models = std::make_unique<Models>();
    malletSampler = std::make_unique<Sampler>();

    for (int i = 0; i < globals::MAX_POLYPHONY; ++i) {
        voices.push_back(std::make_unique<Voice>(*models, *malletSampler));
    }
    
    mtsClientPtr = MTS_RegisterClient();
    
    loadSettings();
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
    MTS_DeregisterClient(mtsClientPtr);
}

void RipplerXAudioProcessor::loadSettings ()
{
    if (auto* file = settings.getUserSettings()) {
        scale = (float)file->getDoubleValue("scale", 1.0f);
        polyphony = file->getIntValue("polyphony", 8);
        darkTheme = file->getBoolValue("dark-theme", false);
    }
}

void RipplerXAudioProcessor::saveSettings ()
{
    if (auto* file = settings.getUserSettings()) {
        file->setValue("scale", scale);
        file->setValue("polyphony", polyphony);
        file->setValue("dark-theme", darkTheme);
    }
    settings.saveIfNeeded();
}

void RipplerXAudioProcessor::setPolyphony(int value)
{
    polyphony = value;
    saveSettings();
    clearVoices();
    onSlider();
}

// Set UI scale factor
void RipplerXAudioProcessor::setScale(float value)
{
    scale = value;
    saveSettings();
}

void RipplerXAudioProcessor::toggleTheme()
{
    darkTheme = !darkTheme;
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
    return 28;
}

int RipplerXAudioProcessor::getCurrentProgram()
{
    return currentProgram;
}

void RipplerXAudioProcessor::setCurrentProgram (int index)
{
    if (currentProgram == index) return;
    currentProgram = index;
    auto data = BinaryData::Init_xml;
    auto size = BinaryData::Init_xmlSize;
    if (currentProgram == -1) return;

    if (index == 1) { data = BinaryData::Harpsi_xml; size = BinaryData::Harpsi_xmlSize; }
    else if (index == 2) { data = BinaryData::Harp_xml; size = BinaryData::Harp_xmlSize; }
    else if (index == 3) { data = BinaryData::Sankyo_xml; size = BinaryData::Sankyo_xmlSize; }
    else if (index == 4) { data = BinaryData::Tubes_xml; size = BinaryData::Tubes_xmlSize; }
    else if (index == 5) { data = BinaryData::Stars_xml; size = BinaryData::Stars_xmlSize; }
    else if (index == 6) { data = BinaryData::DoorBell_xml; size = BinaryData::DoorBell_xmlSize; }
    else if (index == 7) { data = BinaryData::Bells_xml; size = BinaryData::Bells_xmlSize; }
    else if (index == 8) { data = BinaryData::Bells2_xml; size = BinaryData::Bells2_xmlSize; }
    else if (index == 9) { data = BinaryData::KeyRing_xml; size = BinaryData::KeyRing_xmlSize; }
    else if (index == 10) { data = BinaryData::Sink_xml; size = BinaryData::Sink_xmlSize; }
    else if (index == 11) { data = BinaryData::Cans_xml; size = BinaryData::Cans_xmlSize; }
    else if (index == 12) { data = BinaryData::Gong_xml; size = BinaryData::Gong_xmlSize; }
    else if (index == 13) { data = BinaryData::Bong_xml; size = BinaryData::Bong_xmlSize; }
    else if (index == 14) { data = BinaryData::Marimba_xml; size = BinaryData::Marimba_xmlSize; }
    else if (index == 15) { data = BinaryData::Fight_xml; size = BinaryData::Fight_xmlSize; }
    else if (index == 16) { data = BinaryData::Tabla_xml; size = BinaryData::Tabla_xmlSize; }
    else if (index == 17) { data = BinaryData::Tabla2_xml; size = BinaryData::Tabla2_xmlSize; }
    else if (index == 18) { data = BinaryData::Strings_xml; size = BinaryData::Strings_xmlSize; }
    else if (index == 19) { data = BinaryData::OldClock_xml; size = BinaryData::OldClock_xmlSize; }
    else if (index == 20) { data = BinaryData::Crystal_xml; size = BinaryData::Crystal_xmlSize; }
    else if (index == 21) { data = BinaryData::Ride_xml; size = BinaryData::Ride_xmlSize; }
    else if (index == 22) { data = BinaryData::Ride2_xml; size = BinaryData::Ride2_xmlSize; }
    else if (index == 23) { data = BinaryData::Crash_xml; size = BinaryData::Crash_xmlSize; }
    else if (index == 24) { data = BinaryData::Vibes_xml; size = BinaryData::Vibes_xmlSize; }
    else if (index == 25) { data = BinaryData::Flute_xml; size = BinaryData::Flute_xmlSize; }
    else if (index == 26) { data = BinaryData::Fifths_xml; size = BinaryData::Fifths_xmlSize; }
    else if (index == 27) { data = BinaryData::Kalimba_xml; size = BinaryData::Kalimba_xmlSize; }

    auto xmlState = XmlDocument::parse (juce::String (data, size));
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(params.state.getType())) {
            clearVoices();
            auto state = juce::ValueTree::fromXml(*xmlState);

            // migrate a_cut and b_cut to new normalized range
            auto a_cut = state.getChildWithProperty("id", "a_cut");
            if (a_cut.isValid()) {
                float val = (float)a_cut.getProperty("value");
                if (val >= 20.0f) {
                    val = std::log(val / 20.0f) / std::log(20000.0f / 20.0f);
                    a_cut.setProperty("value", val, nullptr);
                }
            }
            auto b_cut = state.getChildWithProperty("id", "b_cut");
            if (b_cut.isValid()) {
                float val = (float)b_cut.getProperty("value");
                if (val >= 20.0f) {
                    val = std::log(val / 20.0f) / std::log(20000.0f / 20.0f);
                    b_cut.setProperty("value", val, nullptr);
                }
            }

            params.replaceState(state);
            resetLastModels();
        }
    }
}

const juce::String RipplerXAudioProcessor::getProgramName (int index)
{
    if (index == 0) return "Init";
    if (index == 1) return "Harpsi";
    if (index == 2) return "Harp";
    if (index == 3) return "Sankyo";
    if (index == 4) return "Tubes";
    if (index == 5) return "Stars";
    if (index == 6) return "DoorBell";
    if (index == 7) return "Bells";
    if (index == 8) return "Bells2";
    if (index == 9) return "KeyRing";
    if (index == 10) return "Sink";
    if (index == 11) return "Cans";
    if (index == 12) return "Gong";
    if (index == 13) return "Bong";
    if (index == 14) return "Marimba";
    if (index == 15) return "Fight";
    if (index == 16) return "Tabla";
    if (index == 17) return "Tabla2";
    if (index == 18) return "Strings";
    if (index == 19) return "OldClock";
    if (index == 20) return "Crystal";
    if (index == 21) return "Ride";
    if (index == 22) return "Ride2";
    if (index == 23) return "Crash";
    if (index == 24) return "Vibes";
    if (index == 25) return "Flute";
    if (index == 26) return "Fifths";
    if (index == 27) return "Kalimba";
    return "";
}

void RipplerXAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    (void)index;
    (void)newName;
}

//==============================================================================
void RipplerXAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    (void)samplesPerBlock;
    totalSamplesBend = (int)(globals::BEND_GLIDE_MS * 0.001 * sampleRate);
    Partial::initA1LUT(sampleRate);
    comb.init(sampleRate);
    limiter.init(sampleRate);
    resetLastModels(); // FIX - ableton initial load causes async value reset that overrides loaded patch value for a_model and b_model
    clearVoices();
    onSlider();
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

int RipplerXAudioProcessor::pickVoice(int note) {
    int pick = 0;
    bool reuseVoices = (bool)params.getRawParameterValue("reuse_voices")->load();

    for (int i = 1; i < polyphony; ++i) {
        const auto& v1 = voices[i];
        const auto& v2 = voices[pick];

        if (v1->note == note && reuseVoices) {
            pick = i;
        }
        else if (!v1->isPressed && !v2->isPressed) {
            if (v1->release_ts < v2->release_ts) pick = i;
        }
        else if (v1->isPressed && v2->isPressed) {
            if (v1->pressed_ts < v2->pressed_ts) pick = i;
        }
        else if (!v1->isPressed && v2->isPressed) {
            pick = i;
        }
    }

    return pick;
}

void RipplerXAudioProcessor::onNote(MIDIMsg msg)
{
    auto srate = getSampleRate();

    int nvoice = pickVoice(msg.note);
    Voice& voice = *voices[nvoice];

    auto mallet_type = (MalletType)params.getRawParameterValue("mallet_type")->load();
    auto mallet_stiff = (double)params.getRawParameterValue("mallet_stiff")->load();
    auto vel_mallet_stiff = (double)params.getRawParameterValue("vel_mallet_stiff")->load();
    auto malletFreq = fmax(100.0, fmin(5000.0, exp(log(mallet_stiff) + msg.vel / 127.0 * vel_mallet_stiff * 2.0 * (log(5000.0) - log(100.0)))));

    voice.trigger(srate, msg.note, msg.vel / 127.0, mallet_type, malletFreq, mtsClientPtr);
}

void RipplerXAudioProcessor::offNote(MIDIMsg msg)
{
    for (int i = 0; i < polyphony; ++i) {
        Voice& voice = *voices[i];
        if (voice.note == msg.note && !voice.isRelease) {
            voice.release();
        }
    }
}

void RipplerXAudioProcessor::onSlider()
{
    auto srate = getSampleRate();

    auto mallet_type = (MalletType)params.getRawParameterValue("mallet_type")->load();
    auto mallet_pitch = (double)params.getRawParameterValue("mallet_pitch")->load();
    auto mallet_filter = (double)params.getRawParameterValue("mallet_filter")->load();

    auto noise_filter_freq = (double)params.getRawParameterValue("noise_filter_freq")->load();
    auto noise_filter_mode = (int)params.getRawParameterValue("noise_filter_mode")->load();
    auto noise_filter_q = (double)params.getRawParameterValue("noise_filter_q")->load();
    auto noise_att = (double)params.getRawParameterValue("noise_att")->load();
    auto noise_dec = (double)params.getRawParameterValue("noise_dec")->load();
    auto noise_sus = (double)params.getRawParameterValue("noise_sus")->load();
    auto noise_rel = (double)params.getRawParameterValue("noise_rel")->load();
    auto noise_att_ten = (double)params.getRawParameterValue("noise_att_ten")->load();
    auto noise_dec_ten = (double)params.getRawParameterValue("noise_dec_ten")->load();
    auto noise_rel_ten = (double)params.getRawParameterValue("noise_rel_ten")->load();
    auto vel_noise_freq = params.getRawParameterValue("vel_noise_freq")->load();
    auto vel_noise_q = params.getRawParameterValue("vel_noise_q")->load();
    auto vel_noise_att = (double)params.getRawParameterValue("vel_noise_att")->load();
    auto vel_noise_dec = (double)params.getRawParameterValue("vel_noise_dec")->load();
    auto vel_noise_sus = (double)params.getRawParameterValue("vel_noise_sus")->load();
    auto vel_noise_rel = (double)params.getRawParameterValue("vel_noise_rel")->load();

    auto a_on = (bool)params.getRawParameterValue("a_on")->load();
    auto a_model = (int)params.getRawParameterValue("a_model")->load();
    auto a_partials = (int)params.getRawParameterValue("a_partials")->load();
    auto a_decay = (double)params.getRawParameterValue("a_decay")->load();
    auto a_damp = (double)params.getRawParameterValue("a_damp")->load();
    auto a_tone = (double)params.getRawParameterValue("a_tone")->load();
    auto a_hit = (double)params.getRawParameterValue("a_hit")->load();
    auto a_rel = (double)params.getRawParameterValue("a_rel")->load();
    auto a_inharm = (double)params.getRawParameterValue("a_inharm")->load();
    auto a_ratio = (double)params.getRawParameterValue("a_ratio")->load();
    auto a_cut = (double)params.getRawParameterValue("a_cut")->load();
    auto a_radius = (double)params.getRawParameterValue("a_radius")->load();

    auto b_on = (bool)params.getRawParameterValue("b_on")->load();
    auto b_model = (int)params.getRawParameterValue("b_model")->load();
    auto b_partials = (int)params.getRawParameterValue("b_partials")->load();
    auto b_decay = (double)params.getRawParameterValue("b_decay")->load();
    auto b_damp = (double)params.getRawParameterValue("b_damp")->load();
    auto b_tone = (double)params.getRawParameterValue("b_tone")->load();
    auto b_hit = (double)params.getRawParameterValue("b_hit")->load();
    auto b_rel = (double)params.getRawParameterValue("b_rel")->load();
    auto b_inharm = (double)params.getRawParameterValue("b_inharm")->load();
    auto b_ratio = (double)params.getRawParameterValue("b_ratio")->load();
    auto b_cut = (double)params.getRawParameterValue("b_cut")->load();
    auto b_radius = (double)params.getRawParameterValue("b_radius")->load();

    auto vel_a_decay = (double)params.getRawParameterValue("vel_a_decay")->load();
    auto vel_a_hit = (double)params.getRawParameterValue("vel_a_hit")->load();
    auto vel_a_inharm = (double)params.getRawParameterValue("vel_a_inharm")->load();
    auto vel_a_damp = (double)params.getRawParameterValue("vel_a_damp")->load();
    auto vel_a_tone = (double)params.getRawParameterValue("vel_a_tone")->load();
    auto vel_b_decay = (double)params.getRawParameterValue("vel_b_decay")->load();
    auto vel_b_hit = (double)params.getRawParameterValue("vel_b_hit")->load();
    auto vel_b_inharm = (double)params.getRawParameterValue("vel_b_inharm")->load();
    auto vel_b_damp = (double)params.getRawParameterValue("vel_b_damp")->load();
    auto vel_b_tone = (double)params.getRawParameterValue("vel_b_tone")->load();

    auto a_coarse = (double)params.getRawParameterValue("a_coarse")->load();
    auto a_fine = (double)params.getRawParameterValue("a_fine")->load();
    auto b_coarse = (double)params.getRawParameterValue("b_coarse")->load();
    auto b_fine = (double)params.getRawParameterValue("b_fine")->load();

    auto couple = (bool)params.getRawParameterValue("couple")->load();
    auto split = (double)params.getRawParameterValue("ab_split")->load() * 100.0;

    if (a_model != last_a_model) {
        auto param = params.getParameter("a_ratio");
        a_ratio = a_model == Beam ? 2.0 : a_model == Djembe ? 1.0 : 0.78;
        auto value = param->convertTo0to1(float(a_ratio));
        MessageManager::callAsync([param, value] {
            param->beginChangeGesture();
            param->setValueNotifyingHost(value);
            param->endChangeGesture();
        });
        clearVoices();
        last_a_model = a_model;
    }
    if (b_model != last_b_model) {
        auto param = params.getParameter("b_ratio");
        b_ratio = b_model == Beam ? 2.0 : b_model == Djembe ? 1.0 : 0.78;
        auto value = param->convertTo0to1((float)b_ratio);
        MessageManager::callAsync([param, value] {
            param->beginChangeGesture();
            param->setValueNotifyingHost(value);
            param->endChangeGesture();
        });
        clearVoices();
        last_b_model = b_model;
    }
    if (last_a_partials != a_partials) {
        clearVoices();
        last_a_partials = a_partials;
    }
    if (last_b_partials != b_partials) {
        clearVoices();
        last_b_partials = b_partials;
    }

    // convert choice to partials num
    if (a_partials == 0) a_partials = 4;
    else if (a_partials == 1) a_partials = 8;
    else if (a_partials == 2) a_partials = 16;
    else if (a_partials == 3) a_partials = 32;
    else if (a_partials == 4) a_partials = 64;
    else if (a_partials == 5) a_partials = 1;
    else if (a_partials == 6) a_partials = 2;
    // convert choice to partials num
    if (b_partials == 0) b_partials = 4;
    else if (b_partials == 1) b_partials = 8;
    else if (b_partials == 2) b_partials = 16;
    else if (b_partials == 3) b_partials = 32;
    else if (b_partials == 4) b_partials = 64;
    else if (b_partials == 5) b_partials = 1;
    else if (b_partials == 6) b_partials = 2;

    if (a_model == ModalModels::Beam) models->recalcBeam(true, a_ratio);
    else if (a_model == ModalModels::Membrane) models->recalcMembrane(true, a_ratio);
    else if (a_model == ModalModels::Plate) models->recalcPlate(true, a_ratio);
    if (b_model == ModalModels::Beam) models->recalcBeam(false, b_ratio);
    else if (b_model == ModalModels::Membrane) models->recalcMembrane(false, b_ratio);
    else if (b_model == ModalModels::Plate) models->recalcPlate(false, b_ratio);

    if (mallet_type != l_mallet_type) {
        l_mallet_type = mallet_type;
        if (mallet_type > MalletType::kUserFile) {
            malletSampler->loadInternalSample(mallet_type);
        }
        clearVoices();
    }

    malletSampler->setPitch(mallet_pitch);

    for (int i = 0; i < polyphony; i++) {
        Voice& voice = *voices[i];
        voice.noise.init(srate, noise_filter_mode, noise_filter_freq, noise_filter_q, noise_att, 
            noise_dec, noise_sus, noise_rel, vel_noise_freq, vel_noise_q, noise_att_ten, noise_dec_ten, noise_rel_ten,
            vel_noise_att, vel_noise_dec, vel_noise_sus, vel_noise_rel
        );
        voice.setPitch(a_coarse, b_coarse, a_fine, b_fine, curBend);
        voice.setRatio(a_ratio, b_ratio);
        voice.resA.setParams(srate, a_on, a_model, a_partials, a_decay, a_damp, a_tone, a_hit, a_rel, 
            a_inharm, a_cut, a_radius, vel_a_decay, vel_a_hit, vel_a_inharm, vel_a_damp, vel_a_tone);
        voice.resB.setParams(srate, b_on, b_model, b_partials, b_decay, b_damp, b_tone, b_hit, b_rel, 
            b_inharm, b_cut, b_radius, vel_b_decay, vel_b_hit, vel_b_inharm, vel_b_damp, vel_b_tone);
        voice.setCoupling(couple, split);
        voice.updateResonators();
        if (mallet_type >= MalletType::kUserFile) {
            voice.mallet.setFilter(mallet_filter);
        }
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
    juce::ScopedNoDenormals disableDenormals;
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto numSamples = buffer.getNumSamples();

    auto a_on = (bool)params.getRawParameterValue("a_on")->load();
    auto b_on = (bool)params.getRawParameterValue("b_on")->load();
    auto mallet_mix = (double)params.getRawParameterValue("mallet_mix")->load();
    auto mallet_res = (double)params.getRawParameterValue("mallet_res")->load();
    auto vel_mallet_mix = (double)params.getRawParameterValue("vel_mallet_mix")->load();
    auto vel_mallet_res = (double)params.getRawParameterValue("vel_mallet_res")->load();
    auto noise_osc = (double)params.getRawParameterValue("noise_osc")->load();
    auto noise_mix = params.getRawParameterValue("noise_mix")->load();
    auto noise_mix_range = params.getParameter("noise_mix")->getNormalisableRange();
    noise_mix = noise_mix_range.convertTo0to1(noise_mix);
    auto noise_res = params.getRawParameterValue("noise_res")->load();
    auto noise_res_range = params.getParameter("noise_res")->getNormalisableRange();
    noise_res = noise_res_range.convertTo0to1(noise_res);
    auto vel_noise_mix = params.getRawParameterValue("vel_noise_mix")->load();
    auto vel_noise_res = params.getRawParameterValue("vel_noise_res")->load();
    auto serial = (bool)params.getRawParameterValue("couple")->load();
    auto ab_mix = (double)params.getRawParameterValue("ab_mix")->load();
    auto gain = (double)params.getRawParameterValue("gain")->load();
    auto couple = (bool)params.getRawParameterValue("couple")->load();
    gain = pow(10.0, gain / 20.0);
    auto bend_range = (double)params.getRawParameterValue("bend_range")->load();
    auto stereoizer = (bool)params.getRawParameterValue("stereoizer")->load();

    auto setBendTarget = [this, bend_range](double pitchWheel)
        {
            double normalized = (pitchWheel - 8192) / 8191.0;
            startBend = curBend;
            targetBend = std::pow(2.0, normalized * bend_range / 12.0);
            remainingSamplesBend = totalSamplesBend;
            bendStep = (targetBend - startBend) / (double)totalSamplesBend;
        };

    auto interpolatePitchBend = [this]()
        {
            if (remainingSamplesBend > 0) {
                curBend += bendStep;

                --remainingSamplesBend;

                if (remainingSamplesBend == 0) {
                    curBend = targetBend;
                }
            }
        };

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
    for (const auto metadata : midiMessages) {
        juce::MidiMessage message = metadata.getMessage();
        if (message.isNoteOn() || message.isNoteOff() || message.isSustainPedalOn() || message.isSustainPedalOff())
            midi.push_back({ // queue midi message
                metadata.samplePosition,
                message.isNoteOn() ? MIDIMsgType::NoteOn
                : message.isNoteOff() ? MIDIMsgType::NoteOff
                : message.isSustainPedalOn() ? MIDIMsgType::SustainPedalOn
                : MIDIMsgType::SustainPedalOff,
                message.getNoteNumber(),
                message.getVelocity(),
                });
        else if (message.isAllNotesOff())
            clearVoices();
        else if (message.isAllSoundOff())
            clearVoices();
        else if (message.isPitchWheel())
            midi.push_back({
                metadata.samplePosition,
                MIDIMsgType::PitchWheel,
                0,
                message.getPitchWheelValue(),
            });
    }

    for (int sample = 0; sample < numSamples; ++sample) {
        interpolatePitchBend();

        // process midi queue
        for (auto& msg : midi) {
            if (msg.offset == 0) {
                if (msg.type == MIDIMsgType::NoteOn) {
                    onNote(msg);

                    // remove new notes from sustain pedal notes, 
                    // fixes notes pressed twice and held should not be released with the pedal
                    auto it = std::remove_if(
                        sustainPedalNotes.begin(),
                        sustainPedalNotes.end(),
                        [msg](const MIDIMsg& m) {
                            return m.note == msg.note;
                        }
                    );
                    sustainPedalNotes.erase(it, sustainPedalNotes.end());
                }
                else if (msg.type == MIDIMsgType::NoteOff) {
                    if (!sustainPedal) {
                        offNote(msg);
                    }
                    else {
                        sustainPedalNotes.push_back(msg);
                    }
                }
                else if (msg.type == MIDIMsgType::SustainPedalOn) {
                    sustainPedal = true;
                }
                else if (msg.type == MIDIMsgType::SustainPedalOff) {
                    sustainPedal = false;
                    for (auto& note : sustainPedalNotes) {
                        offNote(note);
                    }
                    sustainPedalNotes.clear();
                }
                else if (msg.type == MIDIMsgType::PitchWheel) {
                    setBendTarget((double)msg.vel);
                }
            }
            msg.offset -= 1;
        }

        double dirOut = 0.0; // direct output
        double aOut = 0.0; // resonator A output
        double bOut = 0.0; // resonator B output

        auto audioIn = 0.0;
        for (int i = 0; i < totalNumInputChannels; ++i)
            audioIn += (double)buffer.getSample(i, sample);
        if (totalNumInputChannels) 
            audioIn /= (double)totalNumInputChannels;

        for (int i = 0; i < polyphony; ++i) {
            Voice& voice = *voices[i];
            double resOut = 0.0;

            if (remainingSamplesBend >= 0) {
                voice.applyPitchBend(curBend);
                if (remainingSamplesBend == 0)
                    remainingSamplesBend = -1;
            }

            // voice fade out used to declick on voice note repeat
            double voiceFadeOutEnv = voice.isFading ? voice.fadeOut() : 1.0;

            // process mallet
            auto msample = voice.mallet.process(); 
            if (msample) {
                dirOut += msample * fmax(0.0, fmin(1.0, mallet_mix + vel_mallet_mix * voice.vel)) * voiceFadeOutEnv;
                resOut += msample * fmax(0.0, fmin(1.0, mallet_res + vel_mallet_res * voice.vel));
            }

            // process audio in
            if (audioIn && voice.isPressed)
                resOut += audioIn;

            // process noise
            auto noise = voice.noise.process(); 
            if (voice.noise.env.state != 0) {
                auto osc = noise_osc > 0.0 && (noise_res > 0.0 || vel_noise_res > 0.0)
                    ? voice.noise.processOSC(voice.processOscillators(false) + voice.processOscillators(true)) * noise_osc
                    : 0.0;
                dirOut += noise * (double)noise_mix_range.convertFrom0to1(fmax(0.f, fmin(1.f, noise_mix + vel_noise_mix * (float)voice.vel))) * voiceFadeOutEnv;
                resOut += (noise * (1.0 - noise_osc) + osc) * (double)noise_res_range.convertFrom0to1(fmax(0.f, fmin(1.f, noise_res + vel_noise_res * (float)voice.vel)));
            }

            auto out_from_a = 0.0; // output from voice A into B in case of resonator serial coupling
            if (a_on) {
                auto out = voice.resA.process(resOut);
                if (voice.resA.cut != 0.0)
                    out = voice.resA.filter.df1(out);
                aOut += out * voiceFadeOutEnv;
                out_from_a = out;
            }

            if (b_on) {
                auto out = voice.resB.process(a_on && couple ? out_from_a : resOut);
                if (voice.resB.cut != 0.0)
                    out = voice.resB.filter.df1(out);
                bOut += out * voiceFadeOutEnv;
            }
        }

        double resOut = 0.0;
        if (a_on && b_on)
            resOut = serial ? bOut : aOut * (1-ab_mix) + bOut * ab_mix;
        else
            resOut = aOut + bOut; // one of them is turned off, just sum the two

        double totalOut = dirOut + resOut * gain;

        double spl0, spl1;
        if (stereoizer) {
            std::tie(spl0, spl1) = comb.process(totalOut);
        }
        else {
            spl0 = totalOut;
            spl1 = totalOut;
        }
        auto [left, right] = limiter.process(spl0, spl1);

        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            buffer.setSample(channel, sample, static_cast<FloatType>(!channel ? left : right));
        }
    }

    float rms = (float)buffer.getRMSLevel(0, 0, buffer.getNumSamples());
    rmsValue.store(rms, std::memory_order_release);
    midiMessages.clear(); // attempt fix rare crash when clicking the piano keys
}

void RipplerXAudioProcessor::clearVoices()
{
    for (int i = 0; i < globals::MAX_POLYPHONY; ++i) {
        Voice& voice = *voices[i];
        voice.clear();
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
    state.setProperty("currentProgram", currentProgram, nullptr);
    std::unique_ptr<juce::XmlElement>xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void RipplerXAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement>xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(params.state.getType())) {
            auto state = juce::ValueTree::fromXml (*xmlState);
            if (state.hasProperty("currentProgram")) {
                currentProgram = static_cast<int>(state.getProperty("currentProgram"));
            }
            // migrate a_cut and b_cut to new normalized range
            auto a_cut = state.getChildWithProperty("id", "a_cut");
            if (a_cut.isValid()) {
                float val = (float)a_cut.getProperty("value");
                if (val >= 20.0f) {
                    val = std::log(val / 20.0f) / std::log(20000.0f / 20.0f);
                    a_cut.setProperty("value", val, nullptr);
                }
            }
            auto b_cut = state.getChildWithProperty("id", "b_cut");
            if (b_cut.isValid()) {
                float val = (float)b_cut.getProperty("value");
                if (val >= 20.0f) {
                    val = std::log(val / 20.0f) / std::log(20000.0f / 20.0f);
                    b_cut.setProperty("value", val, nullptr);
                }
            }

            params.replaceState(state);
        }
    }

    resetLastModels();
    clearVoices();
}

// Reset last models and last partials so they don't trigger changes onSlider()
void RipplerXAudioProcessor::resetLastModels()
{
    last_a_model = (int)params.getRawParameterValue("a_model")->load();
    last_a_partials = (int)params.getRawParameterValue("a_partials")->load();
    last_b_model = (int)params.getRawParameterValue("b_model")->load();
    last_b_partials = (int)params.getRawParameterValue("b_partials")->load();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RipplerXAudioProcessor();
}
