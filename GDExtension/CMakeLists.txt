# Using the same minimum as the godot-cpp project
cmake_minimum_required(VERSION 3.17)

# Silence unused variable warning when specified from toolchain
if(CMAKE_C_COMPILER)
endif()

set(LIBNAME "GraphiteGDExtension") # "The name of the library"
set(GODOT_PROJECT_DIR "project" CACHE STRING "The directory of a Godot project folder")

# Make sure all the dependencies are satisfied
find_package(Python3 3.4 REQUIRED)
find_program(GIT git REQUIRED)

# Ensure godot-cpp submodule has been updated
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/godot-cpp/src")
    message(NOTICE "godot-cpp bindings source not found")
    message(NOTICE "initializing/updating the godot-cpp submodule...")

    # update the c++ bindings submodule to populate it with the necessary source for the library
    execute_process(
        COMMAND git submodule update --init godot-cpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()
add_subdirectory(godot-cpp SYSTEM)

# When compiling for emscripten, this toolchainhack needs to be included.
# set(CMAKE_PROJECT_<project-name>_INCLUDE ${godot-cpp_SOURCE_DIR}/cmake/emsdkHack.cmake)

# Create the gen directory at configure time so IDEs can see generated headers immediately after generate_bindings
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/godot-cpp/gen/include")

# Add godot-cpp's module path and include the exported functions.
# This is made available for documentation generation
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${godot-cpp_SOURCE_DIR}/cmake")
include(GodotCPPModule)

# The godot-cpp target has some of useful properties attached that can be retrieved like so.
get_target_property(GODOTCPP_SUFFIX godot::cpp GODOTCPP_SUFFIX)
get_target_property(GODOTCPP_PLATFORM godot::cpp GODOTCPP_PLATFORM)

# Now we can specify our own project which will inherit any global cmake properties or variables that have been defined.
project(godot-cpp-template
    VERSION 1.0
    DESCRIPTION "This repository serves as a quickstart template for GDExtension development with Godot 4.0+."
    HOMEPAGE_URL "https://github.com/enetheru/godot-cpp-template/tree/main"
    LANGUAGES CXX
)

add_library(${LIBNAME} SHARED)

target_sources(${LIBNAME}
    PRIVATE
    src/RegisterTypes.cpp
    src/RegisterTypes.hpp
    src/GameManager.cpp
    src/GameManager.hpp
    src/GameplaySceneManager.cpp
    src/GameplaySceneManager.hpp
    srcGameplay/RedNote.cpp
    srcGameplay/RedNote.hpp
    srcGameplay/SettingsFile.cpp
    srcGameplay/SettingsFile.hpp
    srcGameplay/TJACourse.cpp
    srcGameplay/TJACourse.hpp
    srcGameplay/InputThreadFunctions.cpp
    srcGameplay/InputThreadFunctions.hpp
    srcGameplay/CompletionList.cpp
    srcGameplay/CompletionList.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/AudioProcessing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/LFProtectObj.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/NumericLimits.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/QueueMPMCMutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/QueueMPSCMutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/QueueSPMCMutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/QueueSPSC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/QueueStorage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/RhythmAudioEngine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/RhythmAudioEnumsAndData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/RhythmAudioTimingAssistant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/ScopeGuard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/SlotMap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/Snapshots.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/TimeConversion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/TimeProviderInterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/TimeProviderOS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/TimeProviderPuppet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio/TimingOSSingletons.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmInput/RhythmInput/DeferredPrint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmInput/RhythmInput/QueueSPSC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmInput/RhythmInput/RhythmInputEngine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmInput/RhythmInput/RhythmInputEnumsAndData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmInput/RhythmInput/Snapshots.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmInput/RhythmInput/TimeConversion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmInput/RhythmInput/TimeProviderInterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmInput/RhythmInput/TimeProviderOS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmInput/RhythmInput/TimeProviderPuppet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmInput/RhythmInput/TimingOSSingletons.cpp
)

# Fetch a list of the xml files to use for documentation and add to our target
file(GLOB_RECURSE DOC_XML LIST_DIRECTORIES NO CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/doc_classes/*.xml")

# conditionally add doc data to compile output
if(DOC_XML)
    if(GODOTCPP_TARGET MATCHES "editor|template_debug")
        target_doc_sources(${LIBNAME} "${DOC_XML}")
    endif()
endif()

target_link_libraries(${LIBNAME} PRIVATE godot-cpp)

# Require at least C++20 for this target
set_property(TARGET ${LIBNAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${LIBNAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmAudio/RhythmAudio
    ${CMAKE_CURRENT_SOURCE_DIR}/../RhythmInput/RhythmInput
)

# Find and link portaudio, vorbis, samplerate
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(PortAudio IMPORTED_TARGET portaudio-2.0)
    if(PortAudio_FOUND)
        target_link_libraries(${LIBNAME} PRIVATE PkgConfig::PortAudio)
    endif()
endif()

set_target_properties(${LIBNAME}
    PROPERTIES
    # The generator expression here prevents msvc from adding a Debug or Release subdir.
    RUNTIME_OUTPUT_DIRECTORY "$<1:${PROJECT_SOURCE_DIR}/bin/${GODOTCPP_PLATFORM}>"

    PREFIX ""
    OUTPUT_NAME "${LIBNAME}${GODOTCPP_SUFFIX}"
)

set(GODOT_PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/${GODOT_PROJECT_DIR}/bin/${GODOTCPP_PLATFORM}")

add_custom_command(TARGET ${LIBNAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${LIBNAME}>" "${GODOT_PROJECT_BINARY_DIR}/$<TARGET_FILE_NAME:${LIBNAME}>"
)
