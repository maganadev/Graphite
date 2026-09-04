#ifndef GraphiteGlobals_hpp
#define GraphiteGlobals_hpp

#include "../../RhythmAudio/RhythmAudio/LFProtectObj.hpp"
#include "../../RhythmAudio/RhythmAudio/RhythmAudioEngine.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"
#include "../srcGameplay/Chart.hpp"
#include <cstdint>
#include <optional>

class GraphiteGlobals
{
public:
    static std::optional<RhythmAudio::RhythmAudioEngine> audioEngine;
    static std::optional<RhythmInput::RhythmInputEngine> inputEngine;
    static uint64_t blueRyouHitsoundHandle;
    static uint64_t blueKaHitsoundHandle;
    static uint64_t blueFukaHitsoundHandle;
    static uint64_t blueChouHitsoundHandle;
    static uint64_t blueAdLibHitsoundHandle;
    static uint64_t redRyouHitsoundHandle;
    static uint64_t redKaHitsoundHandle;
    static uint64_t redFukaHitsoundHandle;
    static uint64_t redChouHitsoundHandle;
    static uint64_t redAdLibHitsoundHandle;
    static uint64_t sineWaveHitsoundHandle;
    static LFProtectObj<Chart> currentChart;
    static int64_t audioOffset;
    static int64_t visualOffset;
};

#endif