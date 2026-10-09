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
    int64_t startTimePicoseconds{0};
    double scrollBPM{240.0};
    std::atomic<bool> finishedJudging{false};
    NoteGradings grading{NoteGradings::Ungraded};

    HittableNote();
    virtual void constructor2(const nlohmann::json& j) = 0;
    virtual ~HittableNote();

    virtual void updatePosition(int64_t correctedSongPositionPs) = 0;
    virtual void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) = 0;
    virtual bool handleStrikeAndGetCompleted(NoteGradings grading, int64_t picosecondsOff) = 0;
    virtual int64_t getSpamHitsCount() const = 0;
    virtual bool isSpamNote() const
    {
        return false;
    }

protected:
    static int64_t parseStartTimePicoseconds(const nlohmann::json& j);
    static double parseScrollBPM(const nlohmann::json& j);
    static int64_t parseStopTimePicoseconds(const nlohmann::json& j);
    static int64_t parseGreenNoteHits(const nlohmann::json& j);
};

#endif