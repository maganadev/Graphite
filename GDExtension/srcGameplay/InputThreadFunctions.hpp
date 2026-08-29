#ifndef InputThreadFunctions_hpp
#define InputThreadFunctions_hpp

#include <cstdint>

#include "RhythmEnums.hpp"

class InputThreadFunctions
{
public:
    static void DrumRimLeft(uint64_t);
    static void DrumCenterLeft(uint64_t);
    static void DrumCenterRight(uint64_t);
    static void DrumRimRight(uint64_t);
};

#endif
