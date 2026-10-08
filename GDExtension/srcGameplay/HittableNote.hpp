#ifndef HittableNote_hpp
#define HittableNote_hpp

#include "../srcThirdParty/json.hpp"

#include "RhythmEnums.hpp"
#include <atomic>
#include <cstdint>

class Chart;

class HittableNote
{
public:
    static constexpr double SCROLL_SPEED_FACTOR = 2000.0 / (240.0 * 1.0e12);
    static constexpr double LANE_Y = 386.0;
    static constexpr double HITZONE_CENTER_X = 618.0;
    static constexpr int64_t HIT_SPAM_WINDOW_PS = 250000000000;

public:
    int64_t timePicoseconds{0};
    int64_t endTimePicoseconds{0};
    double scrollBPM{240.0};
    std::atomic<bool> judged{false};
    NoteGradings grading{NoteGradings::Ungraded};
    int64_t picosecondsOff{0};

    HittableNote(const nlohmann::json& j);
    virtual ~HittableNote();

    void getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY);
    virtual void getGradingForOfftime(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) = 0;
    virtual void setJudged(NoteGradings grading, int64_t picosecondsOff) = 0;
};

#endif