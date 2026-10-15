# In MSYS2 UCRT64 shell
# pacman -S mingw-w64-ucrt-x86_64-gcc
# pacman -S mingw-w64-ucrt-x86_64-clang-tools-extra
# pacman -S mingw-w64-ucrt-x86_64-libvorbis
# pacman -S mingw-w64-ucrt-x86_64-libsamplerate
# pacman -S mingw-w64-ucrt-x86_64-cmake
#
# In MSYS2 UCRT64 shell
# cmake --preset Release -DCMAKE_PREFIX_PATH=/ucrt64
# cmake --build build/Release/

cmake_minimum_required(VERSION 3.21)
project(GraphiteClangCheck CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wfatal-errors)

# Emit compile_commands.json so clang-tidy / clangd can reuse the exact flags.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# clang-tidy discovers its configuration by walking up from the directory of the
# source file. The sources checked here live under GDExtension/, RhythmAudio/ and
# RhythmInput/, so the .clang-tidy kept next to this project would never be
# found and clang-tidy would silently fall back to its default checks. Point it
# at the project config explicitly.
if(CMAKE_CXX_CLANG_TIDY)
    list(APPEND CMAKE_CXX_CLANG_TIDY
        "--config-file=${CMAKE_CURRENT_SOURCE_DIR}/../.clang-tidy"
    )
endif()

# Debug symbols configuration
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DRELEASE")

find_package(PkgConfig REQUIRED)
pkg_check_modules(Vorbis REQUIRED IMPORTED_TARGET vorbisfile)
pkg_check_modules(SampleRate REQUIRED IMPORTED_TARGET samplerate)

# --- Source locations -------------------------------------------------------
# This project lives in GDExtension/ClangCheck/Demo, so the Graphite sources we
# want to check are reached through the parent directories.
set(GDEXTENSION_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(RHYTHM_AUDIO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../RhythmAudio/RhythmAudio")
set(RHYTHM_INPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../RhythmInput/RhythmInput")
set(GODOTCPP_DIR "${GDEXTENSION_DIR}/godot-cpp")

# --- Godot engine bindings (compiled, but NOT checked by clang-tidy) --------
# Use the godot-cpp static library produced by the main build so we do not
# recompile the engine bindings here, and so clang-tidy never runs on their
# sources (we do not control that code).
file(GLOB GODOTCPP_STATIC_LIBS
    "${GODOTCPP_DIR}/bin/libgodot-cpp*.a"
    "${GODOTCPP_DIR}/bin/godot-cpp*.lib"
)
if(NOT GODOTCPP_STATIC_LIBS)
    message(FATAL_ERROR
        "Could not find a prebuilt godot-cpp static library in ${GODOTCPP_DIR}/bin. "
        "Build godot-cpp first (e.g. with the main GDExtension build).")
endif()
list(GET GODOTCPP_STATIC_LIBS 0 GODOTCPP_STATIC_LIB)

add_library(godot-cpp STATIC IMPORTED)
set_target_properties(godot-cpp PROPERTIES
    IMPORTED_LOCATION "${GODOTCPP_STATIC_LIB}"
)
target_include_directories(godot-cpp INTERFACE
    "${GODOTCPP_DIR}/include"
    "${GODOTCPP_DIR}/gen/include"
    "${GODOTCPP_DIR}/gdextension"
)
target_compile_definitions(godot-cpp INTERFACE
    GDEXTENSION
    THREADS_ENABLED
)
if(WIN32)
    target_compile_definitions(godot-cpp INTERFACE WINDOWS_ENABLED)
elseif(UNIX)
    target_compile_definitions(godot-cpp INTERFACE LINUX_ENABLED UNIX_ENABLED)
endif()

# --- Graphite GDExtension sources (checked by clang-tidy) -------------------
file(GLOB GDEXTENSION_SOURCES CONFIGURE_DEPENDS
    "${GDEXTENSION_DIR}/src/*.cpp"
    "${GDEXTENSION_DIR}/srcGameplay/*.cpp"
)
file(GLOB RHYTHM_AUDIO_SOURCES CONFIGURE_DEPENDS
    "${RHYTHM_AUDIO_DIR}/*.cpp"
)
file(GLOB RHYTHM_INPUT_SOURCES CONFIGURE_DEPENDS
    "${RHYTHM_INPUT_DIR}/*.cpp"
)

# The RhythmAudio and RhythmInput projects share some file names. Keep the
# RhythmAudio copy and only add RhythmInput files that are not already present.
set(RHYTHM_AUDIO_BASENAMES "")
foreach(source ${RHYTHM_AUDIO_SOURCES})
    get_filename_component(basename "${source}" NAME)
    list(APPEND RHYTHM_AUDIO_BASENAMES "${basename}")
endforeach()
set(RHYTHM_INPUT_SOURCES_FILTERED "")
foreach(source ${RHYTHM_INPUT_SOURCES})
    get_filename_component(basename "${source}" NAME)
    if(NOT basename IN_LIST RHYTHM_AUDIO_BASENAMES)
        list(APPEND RHYTHM_INPUT_SOURCES_FILTERED "${source}")
    endif()
endforeach()

set(GraphiteClangCheck_SOURCES
    main.cpp
    ${GDEXTENSION_SOURCES}
    ${RHYTHM_AUDIO_SOURCES}
    ${RHYTHM_INPUT_SOURCES_FILTERED}
)

add_executable(GraphiteClangCheck
    ${GraphiteClangCheck_SOURCES}
)

# The ASIO SDK is third party code: compile it into its own target so clang-tidy
# does not check it.
if(WIN32)
    set(ASIO_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../RhythmAudio/ThirdParty/ASIOSDK")
    add_library(graphite_asio_sdk STATIC
        "${ASIO_SDK_DIR}/common/asio.cpp"
        "${ASIO_SDK_DIR}/host/asiodrivers.cpp"
        "${ASIO_SDK_DIR}/host/pc/asiolist.cpp"
    )
    set_target_properties(graphite_asio_sdk PROPERTIES CXX_CLANG_TIDY "")
    target_include_directories(graphite_asio_sdk PUBLIC
        "${ASIO_SDK_DIR}/common"
        "${ASIO_SDK_DIR}/host"
        "${ASIO_SDK_DIR}/host/pc"
    )
endif()

target_link_libraries(GraphiteClangCheck PRIVATE
    godot-cpp
    PkgConfig::Vorbis
    PkgConfig::SampleRate
)

if(WIN32)
    target_link_libraries(GraphiteClangCheck PRIVATE graphite_asio_sdk)
endif()

target_include_directories(GraphiteClangCheck PRIVATE
    "${GDEXTENSION_DIR}/src"
    "${GDEXTENSION_DIR}/srcGameplay"
    "${RHYTHM_AUDIO_DIR}"
    "${RHYTHM_INPUT_DIR}"
)

if(WIN32)
    target_link_libraries(GraphiteClangCheck PRIVATE winmm ole32 uuid advapi32 setupapi)
endif()

# Run clang-format on the sources listed above. Already-formatted files are
# left untouched so their modification times (and incremental builds) survive.
add_custom_target(format
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/../FormatSources.sh ${GraphiteClangCheck_SOURCES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Formatting sources with clang-format"
    VERBATIM
)