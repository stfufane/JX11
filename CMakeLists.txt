cmake_minimum_required(VERSION 3.27)

project(JX11
    VERSION 0.0.1
    DESCRIPTION "A JUCE synthesizer"
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
include(get_cpm.cmake)

option(JUCE_BUILD_EXTRAS "Build JUCE Extras" ON)
option(JUCE_BUILD_EXAMPLES "Build JUCE Examples" OFF)

# Adds all the module sources so they appear correctly in the IDE
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Enable Module Source Groups" ON)

CPMAddPackage(
    NAME JUCE
    GIT_TAG master
    GITHUB_REPOSITORY juce-framework/JUCE
    SOURCE_DIR ${LIB_DIR}/juce
)

CPMAddPackage(
    NAME melatonin_perfetto
    GIT_TAG main
    GITHUB_REPOSITORY sudara/melatonin_perfetto
    SOURCE_DIR ${LIB_DIR}/melatonin_perfetto
)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

if(MSVC)
    string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REGEX REPLACE "-W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Generate the ProjectInfo struct.
set(PROJECT_COMPANY "Stephane Albanese")
set(PROJECT_VERSION_STRING "${PROJECT_VERSION}")
set(PROJECT_VERSION_NUMBER 0x000001)
configure_file(ProjectInfo.h.in ${CMAKE_CURRENT_SOURCE_DIR}/gen/ProjectInfo.h)

juce_add_plugin("${PROJECT_NAME}"
    COMPANY_NAME "${PROJECT_COMPANY}"
    BUNDLE_ID "com.stephanealbanese.jx11"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE APGM
    PLUGIN_CODE JX11
    FORMATS Standalone VST3
    PRODUCT_NAME "JX11")

target_compile_features(${PROJECT_NAME}
    PUBLIC
    cxx_std_17)

target_sources(${PROJECT_NAME} PRIVATE
    src/processor/BaseProcessor.cpp
    src/processor/BaseProcessor.h
    src/processor/PluginProcessor.cpp
    src/processor/PluginProcessor.h
    src/processor/Params.h
    src/processor/Utils.h

    src/engine/Envelope.h
    src/engine/Filter.h
    src/engine/NoiseGenerator.h
    src/engine/Oscillator.h
    src/engine/Synth.h
    src/engine/Synth.cpp
    src/engine/Voice.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})

set(BinaryDataTarget "${PROJECT_NAME}-Data")
juce_add_binary_data(${BinaryDataTarget} SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/data/img/logo.png
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${BinaryDataTarget})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/gen)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    DONT_SET_USING_JUCE_NAMESPACE=1
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    juce_dsp
    juce_audio_utils
    juce_gui_extra
    Melatonin::Perfetto
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /Wall /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
