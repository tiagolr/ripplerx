cmake_minimum_required(VERSION 3.24)

# Change to your project name
project(RipplerX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_XCODE_GENERATE_SCHEME OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "Runtime Library")
endif()

add_subdirectory(libs/JUCE) # this is however you load juce

# Make sure you include any new source files here
set(SourceFiles
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
)

set(plugin_formats
    VST3
)
if(APPLE)
    list(APPEND plugin_formats AU)
elseif(UNIX AND NOT APPLE)
    list(APPEND plugin_formats LV2)
endif()

juce_add_plugin(${PROJECT_NAME}
        VERSION "1.0.0"
        COMPANY_NAME Tilr
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        JUCE_VST3_CAN_REPLACE_VST2 FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE Tilr
        PLUGIN_CODE Ripx
        FORMATS ${plugin_formats}
        PRODUCT_NAME "RipplerX"
        BUNDLE_ID "com.tilr.ripplerx"
        LV2URI "https://github.com/tiagolr/ripplerx"
)

# How we want our SourceFiles to appear in our IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})

# Make the SourceFiles buildable
target_sources(${PROJECT_NAME} PRIVATE ${SourceFiles})

# These are some toggleable options from the JUCE CMake API
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        #JUCE_DISPLAY_SPLASH_SCREEN=0
)

# JUCE libraries to bring into our project
juce_generate_juce_header(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        juce::juce_analytics
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_core
        juce::juce_data_structures
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

#file(GLOB_RECURSE res "${CMAKE_CURRENT_SOURCE_DIR}/Resources/*.*")
#juce_add_binary_data(${PROJECT_NAME}_res SOURCES ${res})
#target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_res)

if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC JUCE_AU=1)

    # This option is relevant to x86_64 target architecture,
    # but I do not see how to detect the target architecture
    # during CMake generation time. So this will cause warnings when
    # compiling for arm64.
    target_compile_options(${PROJECT_NAME} PRIVATE "-mfma")
endif()