#ifndef GreenNote_hpp
#define GreenNote_hpp

#include "../srcThirdParty/json.hpp"
#include <atomic>
#include <cstdint>

#include "RhythmEnums.hpp"

class GreenNotePrefab;

class GreenNote
{
public:
    static constexpr double SCROLL_SPEED_FACTOR = 2000.0 / (240.0 * 1.0e12);
    static constexpr double LANE_Y = 386.0;
    static constexpr double HITZONE_CENTER_X = 618.0;

    GreenNote(const nlohmann::json& j);
    ~GreenNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);
    void getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const;

    int64_t timePicoseconds;
    double bpmForScrollDouble;
    std::atomic<bool> judged{false};
    NoteGradings grading{NoteGradings::Ungraded};
    int64_t picosecondsOff{0};
    GreenNotePrefab* prefab{nullptr};
};

#endif
