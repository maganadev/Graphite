#ifndef RedNote_hpp
#define RedNote_hpp

#include <atomic>
#include <cstdint>

#include "Course.hpp"
#include "RhythmEnums.hpp"

class RedNotePrefab;

class RedNote
{
public:
    static constexpr double SCROLL_SPEED = 1600.0;
    static constexpr double LANE_Y = 386.0;
    static constexpr double HITZONE_CENTER_X = 618.0;

    RedNote();
    ~RedNote();

    void setNote(const Note& note);
    const Note& getNote() const;

    void setPrefab(RedNotePrefab* prefab);
    RedNotePrefab* getPrefab() const;

    bool isJudged() const;
    void setJudged(NoteGradings grading);
    NoteGradings getGrading() const;

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);
    void getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const;

private:
    Note m_note;
    std::atomic<bool> m_judged{false};
    NoteGradings m_grading{NoteGradings::Ungraded};
    RedNotePrefab* m_prefab{nullptr};
};

#endif