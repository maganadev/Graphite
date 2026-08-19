#ifndef Globals_hpp
#define Globals_hpp

class Globals
{
public:
    static std::string songName;
    static std::string courseDifficulty;
    static std::optional<RhythmInput::RhythmInputEngine> inputEngine;
    static json songJson;
};

#endif
