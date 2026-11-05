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

# ==============================================================================
# 1. GRANULAR LAB (Audio Effect Plugin)
# ==============================================================================
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

clap_juce_extensions_plugin(
    TARGET GranularLab
    CLAP_ID "com.granularlab.granularlab"
    CLAP_FEATURES audio-effect granular glitch filter delay reverb
)

# ==============================================================================
# 2. GRANULAR SYNTH LAB (Polyphonic Granular Synthesizer Plugin)
# ==============================================================================
juce_add_plugin(GranularSynthLab
    COMPANY_NAME "GranularLab"
    PLUGIN_MANUFACTURER_CODE GLab
    PLUGIN_CODE GLs1
    FORMATS VST3 Standalone
    PRODUCT_NAME "Granular Synth Lab"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(GranularSynthLab PRIVATE
    Source/Common/Parameters.h
    Source/Common/LockFreeQueue.h

    Source/DSP/ModulationEngine.h
    Source/DSP/ModulationEngine.cpp
    Source/DSP/EffectsEngine.h
    Source/DSP/EffectsEngine.cpp

    Source/UI/CustomLookAndFeel.h
    Source/UI/CustomLookAndFeel.cpp
    Source/UI/XYPad.h
    Source/UI/XYPad.cpp
    Source/UI/EffectsPanel.h
    Source/UI/EffectsPanel.cpp
    Source/UI/ModulationPanel.h
    Source/UI/ModulationPanel.cpp

    Source/Synth/DSP/SampleBuffer.h
    Source/Synth/DSP/SampleBuffer.cpp
    Source/Synth/DSP/SynthGrain.h
    Source/Synth/DSP/SynthGrain.cpp
    Source/Synth/DSP/SynthVoice.h
    Source/Synth/DSP/SynthVoice.cpp
    Source/Synth/DSP/SynthEngine.h
    Source/Synth/DSP/SynthEngine.cpp

    Source/Synth/Presets/SynthPresetData.h
    Source/Synth/SynthParameters.h

    Source/Synth/UI/SynthPresetBar.h
    Source/Synth/UI/SynthPresetBar.cpp
    Source/Synth/UI/SynthSourcePanel.h
    Source/Synth/UI/SynthSourcePanel.cpp
    Source/Synth/UI/SynthWaveformView.h
    Source/Synth/UI/SynthWaveformView.cpp
    Source/Synth/UI/SynthParticleView.h
    Source/Synth/UI/SynthParticleView.cpp
    Source/Synth/UI/SynthADSRPanel.h
    Source/Synth/UI/SynthADSRPanel.cpp

    Source/Synth/SynthProcessor.h
    Source/Synth/SynthProcessor.cpp
    Source/Synth/SynthEditor.h
    Source/Synth/SynthEditor.cpp
)

target_include_directories(GranularSynthLab PRIVATE
    Source
    libs/clap-juce-extensions/include
)

target_compile_definitions(GranularSynthLab PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    HAS_CLAP_JUCE_EXTENSIONS=1
)

target_link_libraries(GranularSynthLab PRIVATE
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

clap_juce_extensions_plugin(
    TARGET GranularSynthLab
    CLAP_ID "com.granularlab.granularsynthlab"
    CLAP_FEATURES instrument synthesizer granular sampler polyphonic
)

# ==============================================================================
# Installation target for Bitwig and Linux DAWs
# ==============================================================================
add_custom_target(install_plugins
    COMMAND ${CMAKE_COMMAND} -E make_directory "$ENV{HOME}/.clap"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$ENV{HOME}/.vst3"
    COMMAND ${CMAKE_COMMAND} -E echo "Installing Granular Lab.clap to $ENV{HOME}/.clap/..."
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/GranularLab_artefacts/Release/CLAP/Granular Lab.clap" "$ENV{HOME}/.clap/Granular Lab.clap"
    COMMAND ${CMAKE_COMMAND} -E echo "Installing Granular Lab.vst3 to $ENV{HOME}/.vst3/..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/GranularLab_artefacts/Release/VST3/Granular Lab.vst3" "$ENV{HOME}/.vst3/Granular Lab.vst3"
    COMMAND ${CMAKE_COMMAND} -E echo "Installing Granular Synth Lab.clap to $ENV{HOME}/.clap/..."
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/GranularSynthLab_artefacts/Release/CLAP/Granular Synth Lab.clap" "$ENV{HOME}/.clap/Granular Synth Lab.clap"
    COMMAND ${CMAKE_COMMAND} -E echo "Installing Granular Synth Lab.vst3 to $ENV{HOME}/.vst3/..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/GranularSynthLab_artefacts/Release/VST3/Granular Synth Lab.vst3" "$ENV{HOME}/.vst3/Granular Synth Lab.vst3"
    DEPENDS GranularLab_CLAP GranularLab_VST3 GranularSynthLab_CLAP GranularSynthLab_VST3
    COMMENT "Installing CLAP and VST3 plugins to user audio directories (~/.clap and ~/.vst3)"
)

# ==============================================================================
# DSP Unit Tests
# ==============================================================================
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

add_executable(synth_dsp_tests
    Tests/SynthDSPTests.cpp
    Source/Synth/DSP/SampleBuffer.cpp
    Source/Synth/DSP/SynthGrain.cpp
    Source/Synth/DSP/SynthVoice.cpp
    Source/Synth/DSP/SynthEngine.cpp
    Source/DSP/EffectsEngine.cpp
)

target_include_directories(synth_dsp_tests PRIVATE
    Source
    libs/JUCE/modules
)

target_compile_definitions(synth_dsp_tests PRIVATE
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
)

target_link_libraries(synth_dsp_tests PRIVATE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_audio_formats
    juce::juce_graphics
    juce::juce_dsp
)
