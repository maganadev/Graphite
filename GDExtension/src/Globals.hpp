#ifndef Globals_hpp
#define Globals_hpp

#include <optional>
#include <string>

#include "../../RhythmAudio/RhythmAudio/RhythmAudioEngine.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"
#include "../UtilsCode/json.hpp"

using json = nlohmann::json;

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
