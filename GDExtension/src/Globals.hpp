#ifndef Globals_hpp
#define Globals_hpp

#include <optional>
#include <string>

#include "../../RhythmAudio/RhythmAudio/RhythmAudioEngine.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"
#include "../UtilsCode/json.hpp"

using json = nlohmann::json;

namespace GameActionIndices
{
    constexpr size_t DrumRimLeft = 0;
    constexpr size_t DrumRimRight = 1;
    constexpr size_t DrumCenterLeft = 2;
    constexpr size_t DrumCenterRight = 3;
    constexpr size_t Enter = 4;
    constexpr size_t Back = 5;
}

class Globals
{
public:
    static std::optional<RhythmAudio::RhythmAudioEngine> audioEngine;
    static std::optional<RhythmInput::RhythmInputEngine> inputEngine;
    static std::string songName;
    static std::string courseDifficulty;
    static json songJson;
};

#endif
