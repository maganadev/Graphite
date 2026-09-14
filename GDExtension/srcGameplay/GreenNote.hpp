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

    int64_t getTimePicoseconds() const;

    void setPrefab(GreenNotePrefab* prefab);
    GreenNotePrefab* getPrefab() const;

    bool isJudged() const;
    void setJudged(NoteGradings grading, int64_t picosecondsOff);
    NoteGradings getGrading() const;
    int64_t getPicosecondsOff() const;

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);
    void getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const;

private:
    int64_t m_timePicoseconds;
    double m_bpmForScrollDouble;
    std::atomic<bool> m_judged{false};
    NoteGradings m_grading{NoteGradings::Ungraded};
    int64_t m_picosecondsOff{0};
    GreenNotePrefab* m_prefab{nullptr};
};

#endif
