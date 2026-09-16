#ifndef YellowNote_hpp
#define YellowNote_hpp

#include "../srcThirdParty/json.hpp"
#include <atomic>
#include <cstdint>

#include "RhythmEnums.hpp"

class YellowNotePrefab;

class YellowNote
{
public:
    static constexpr double SCROLL_SPEED_FACTOR = 2000.0 / (240.0 * 1.0e12);
    static constexpr double LANE_Y = 386.0;
    static constexpr double HITZONE_CENTER_X = 618.0;

    YellowNote(const nlohmann::json& j);
    ~YellowNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);
    void getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const;

    int64_t timePicoseconds;
    double bpmForScrollDouble;
    std::atomic<bool> judged{false};
    NoteGradings grading{NoteGradings::Ungraded};
    int64_t picosecondsOff{0};
    YellowNotePrefab* prefab{nullptr};
};

#endif
