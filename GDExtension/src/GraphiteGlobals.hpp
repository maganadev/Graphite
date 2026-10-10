#ifndef GraphiteGlobals_hpp
#define GraphiteGlobals_hpp

#include "../../RhythmAudio/RhythmAudio/LFProtectObj.hpp"
#include "../../RhythmAudio/RhythmAudio/RhythmAudioEngine.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"
#include "../srcGameplay/Chart.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

class GraphiteGlobals
{
public:
    static std::optional<RhythmAudio::RhythmAudioEngine> audioEngine;
    static std::optional<RhythmInput::RhythmInputEngine> inputEngine;

public:
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
    static uint64_t greenNoteCompleteHitsoundHandle;

public:
    static LFProtectObj<Chart> currentChart;

public:
    // Audio track handle for the currently playing song
    static uint64_t audioTrackHandle;

public:
    static int64_t audioOffset;
    static int64_t visualOffset;

public:
    // Loading into gameplay state
    static int32_t difficulty;
    static std::string currentSongFileName;

public:
    // Mods
    static double playbackRate;
    static bool modVisualOffsetCalibration;
    static bool modAudioOffsetCalibration;

public:
    // Menu navigation state (preserved across scene transitions)
    static int32_t menuCurrentNodeId;
    static int32_t menuCurrentItemIndex;
    static int32_t menuScrollOffset;
    static std::vector<int32_t> menuNavNodeIds;
    static std::vector<int32_t> menuNavItemIndices;
    static std::vector<int32_t> menuNavScrollOffsets;
};

#endif