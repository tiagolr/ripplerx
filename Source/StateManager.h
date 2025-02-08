#pragma once

class PluginProcessor;

#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "Globals.h"

class StateManager : public juce::ValueTree::Listener, public juce::AudioProcessorValueTreeState::Listener {
public:
    StateManager(PluginProcessor* proc);
    ~StateManager() override;

    float param_value(size_t param_id);
    juce::RangedAudioParameter* get_parameter(size_t param_id);
    void set_parameter(size_t param_id, float value);
    void set_parameter_normalized(size_t param_id, float normalized_value);
    void randomize_parameter(size_t param_id, float min = 0.0f, float max = 1.0f);
    void reset_parameter(size_t param_id);
    void init();
    void randomize_parameters();
    juce::String get_parameter_text(size_t param_id);
    bool get_parameter_modified(size_t param_id, bool exchange_value=false);

    juce::UndoManager* get_undo_manager();

    void parameterChanged (const juce::String &parameterID, float newValue) override;

    //--------------------------------------------------------------------------------
    // const identifiers used for accessing ValueTrees
    // You might be able to make these private, depends on your implementation
    // One reason they should be public: if you copy a valueTree to a separate component
    //    and you want to get the children of that tree by identifier 
    //        (but don't want to copy a bunch of strings around)
    //--------------------------------------------------------------------------------
    static inline const juce::Identifier PARAMETERS_ID{"PARAMETERS"};
    static inline const juce::Identifier PRESET_ID{"PRESET"};
    static inline const juce::Identifier PRESET_NAME_ID{"PRESET_NAME"};
    static inline const juce::Identifier PRESET_MODIFIED_ID{"PRESET_MODIFIED"};
    static inline const juce::Identifier PROPERTIES_ID{"PROPERTIES"};
    static inline const juce::Identifier STATE_ID{"STATE"};

    //--------------------------------------------------------------------------------
    // Some preset info
    // these are public for convenience if you make a preset browser component
    //--------------------------------------------------------------------------------
    const juce::File PRESETS_DIR; // initialized in initializer list
    const juce::String PRESET_EXTENSION;
    const juce::String DEFAULT_PRESET{"INIT"};

    //--------------------------------------------------------------------------------
    // any_parameter_changed is true after any parameter is changed (including preset changes)
    // preset modified is true after any parameter is changed (but not after preset changes)
    //--------------------------------------------------------------------------------
    std::atomic<bool> any_parameter_changed{false};
    std::atomic<bool> preset_modified{true};

private:
    // state
    juce::ValueTree state_tree;
    std::unique_ptr<juce::AudioProcessorValueTreeState> param_tree_ptr;
    juce::ValueTree property_tree;
    std::unordered_map<juce::String, std::atomic<float>> property_atomics;
    std::unordered_map<juce::String, std::atomic<bool>> parameter_modified_flags;

    juce::ValueTree preset_tree;

    //random number generator for randomizing parameters
    juce::Random rng;

    // Undo Manager
    juce::UndoManager undo_manager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StateManager)
};