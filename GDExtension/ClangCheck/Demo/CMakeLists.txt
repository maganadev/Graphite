# In MSYS2 UCRT64 shell
# pacman -S mingw-w64-ucrt-x86_64-gcc
# pacman -S mingw-w64-ucrt-x86_64-clang-tools-extra
# pacman -S mingw-w64-ucrt-x86_64-libvorbis
# pacman -S mingw-w64-ucrt-x86_64-libsamplerate
# pacman -S mingw-w64-ucrt-x86_64-portaudio
# pacman -S mingw-w64-ucrt-x86_64-cmake
#
# In MSYS2 UCRT64 shell
# cmake --preset Release -DCMAKE_PREFIX_PATH=/ucrt64
# cmake --build build/Release/

cmake_minimum_required(VERSION 3.21)
project(GraphiteClangCheck CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wfatal-errors)

# Debug symbols configuration
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DRELEASE")

find_package(PkgConfig REQUIRED)
pkg_check_modules(Vorbis REQUIRED IMPORTED_TARGET vorbisfile)
pkg_check_modules(SampleRate REQUIRED IMPORTED_TARGET samplerate)
pkg_check_modules(PortAudio REQUIRED IMPORTED_TARGET portaudio-2.0)

set(GraphiteClangCheck_SOURCES
    main.cpp
)

add_executable(GraphiteClangCheck
    ${GraphiteClangCheck_SOURCES}
)

# Run clang-format on the sources listed above. Already-formatted files are
# left untouched so their modification times (and incremental builds) survive.
add_custom_target(format
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/../FormatSources.sh ${GraphiteClangCheck_SOURCES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Formatting sources with clang-format"
    VERBATIM
)
