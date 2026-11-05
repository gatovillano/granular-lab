cmake_minimum_required(VERSION 3.22)

project(GranularLab VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Disable JUCE splash screen / analytics for clean professional experience
add_compile_definitions(JUCE_SILENCE_GLOBAL_VBLANK_WARNING=1)

# Include JUCE from libs/JUCE
add_subdirectory(libs/JUCE)

# Include CLAP extensions for JUCE
add_subdirectory(libs/clap-juce-extensions EXCLUDE_FROM_ALL)

# JUCE Plugin definition
juce_add_plugin(GranularLab
    COMPANY_NAME "GranularLab"
    PLUGIN_MANUFACTURER_CODE GLab
    PLUGIN_CODE GLb1
    FORMATS VST3 Standalone
    PRODUCT_NAME "Granular Lab"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

# Source files
target_sources(GranularLab PRIVATE
    Source/Common/Parameters.h
    Source/Common/LockFreeQueue.h

    Source/DSP/AudioBuffer.h
    Source/DSP/AudioBuffer.cpp
    Source/DSP/Grain.h
    Source/DSP/Grain.cpp
    Source/DSP/GranularEngine.h
    Source/DSP/GranularEngine.cpp
    Source/DSP/ModulationEngine.h
    Source/DSP/ModulationEngine.cpp
    Source/DSP/EffectsEngine.h
    Source/DSP/EffectsEngine.cpp

    Source/Presets/PresetData.h

    Source/UI/CustomLookAndFeel.h
    Source/UI/CustomLookAndFeel.cpp
    Source/UI/XYPad.h
    Source/UI/XYPad.cpp
    Source/UI/WaveformView.h
    Source/UI/WaveformView.cpp
    Source/UI/ParticleView.h
    Source/UI/ParticleView.cpp
    Source/UI/EffectsPanel.h
    Source/UI/EffectsPanel.cpp
    Source/UI/ModulationPanel.h
    Source/UI/ModulationPanel.cpp
    Source/UI/PresetBar.h
    Source/UI/PresetBar.cpp

    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
)

target_include_directories(GranularLab PRIVATE
    Source
    libs/clap-juce-extensions/include
)

target_compile_definitions(GranularLab PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    HAS_CLAP_JUCE_EXTENSIONS=1
)

target_link_libraries(GranularLab PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_audio_formats
    juce::juce_audio_basics
    juce::juce_gui_extra
    juce::juce_gui_basics
    juce::juce_graphics
    juce::juce_events
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    clap_juce_extensions
)

# Register CLAP Target
clap_juce_extensions_plugin(
    TARGET GranularLab
    CLAP_ID "com.granularlab.granularlab"
    CLAP_FEATURES audio-effect granular glitch filter delay reverb
)

# Custom installation target for Bitwig and Linux DAWs
add_custom_target(install_plugins
    COMMAND ${CMAKE_COMMAND} -E make_directory "$ENV{HOME}/.clap"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$ENV{HOME}/.vst3"
    COMMAND ${CMAKE_COMMAND} -E echo "Installing Granular Lab.clap to $ENV{HOME}/.clap/..."
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/GranularLab_artefacts/Release/CLAP/Granular Lab.clap" "$ENV{HOME}/.clap/Granular Lab.clap"
    COMMAND ${CMAKE_COMMAND} -E echo "Installing Granular Lab.vst3 to $ENV{HOME}/.vst3/..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/GranularLab_artefacts/Release/VST3/Granular Lab.vst3" "$ENV{HOME}/.vst3/Granular Lab.vst3"
    DEPENDS GranularLab_CLAP GranularLab_VST3
    COMMENT "Installing CLAP and VST3 plugins to user audio directories (~/.clap and ~/.vst3)"
)

# Standalone DSP unit test executable
add_executable(dsp_tests
    Tests/DSPTests.cpp
    Source/DSP/AudioBuffer.cpp
    Source/DSP/Grain.cpp
    Source/DSP/GranularEngine.cpp
    Source/DSP/ModulationEngine.cpp
    Source/DSP/EffectsEngine.cpp
)

target_include_directories(dsp_tests PRIVATE
    Source
    libs/JUCE/modules
)

target_compile_definitions(dsp_tests PRIVATE
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
)

target_link_libraries(dsp_tests PRIVATE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_graphics
    juce::juce_dsp
)
