#ifndef HittableNote_hpp
#define HittableNote_hpp

#include "../srcThirdParty/json.hpp"

#include "RhythmEnums.hpp"
#include <atomic>
#include <cstdint>

class HittableNote
{
public:
    static constexpr int64_t SCROLL_SPEED_FACTOR = 3;
    static constexpr double LANE_Y = 420.0;
    static constexpr double HITZONE_CENTER_X = 475.0;

    int64_t timePicoseconds{0};
    double bpmForScrollDouble{240.0};
    std::atomic<bool> judged{false};
    NoteGradings grading{NoteGradings::Ungraded};
    int64_t picosecondsOff{0};

    HittableNote(const nlohmann::json& j);
    virtual ~HittableNote();

    void getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY);
};

#endif