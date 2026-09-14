#ifndef BlueNote_hpp
#define BlueNote_hpp

#include "../srcThirdParty/json.hpp"
#include <atomic>
#include <cstdint>

#include "RhythmEnums.hpp"

class BlueNotePrefab;

class BlueNote
{
public:
    static constexpr double SCROLL_SPEED_FACTOR = 2000.0 / (240.0 * 1.0e12);
    static constexpr double LANE_Y = 386.0;
    static constexpr double HITZONE_CENTER_X = 618.0;

    BlueNote(const nlohmann::json& j);
    ~BlueNote();

    int64_t getTimePicoseconds() const;

    void setPrefab(BlueNotePrefab* prefab);
    BlueNotePrefab* getPrefab() const;

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
    BlueNotePrefab* m_prefab{nullptr};
};

#endif
