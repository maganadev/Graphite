#ifndef GreenNote_hpp
#define GreenNote_hpp

#include <atomic>
#include <cstdint>

#include "Course.hpp"
#include "RhythmEnums.hpp"

class GreenNotePrefab;

class GreenNote
{
public:
    static constexpr double SCROLL_SPEED = 1600.0;
    static constexpr double LANE_Y = 386.0;
    static constexpr double HITZONE_CENTER_X = 618.0;

    GreenNote();
    ~GreenNote();

    void setNote(const Note& note);
    const Note& getNote() const;

    void setPrefab(GreenNotePrefab* prefab);
    GreenNotePrefab* getPrefab() const;

    bool isJudged() const;
    void setJudged(NoteGradings grading);
    NoteGradings getGrading() const;

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);
    void getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const;

private:
    Note m_note;
    std::atomic<bool> m_judged{false};
    NoteGradings m_grading{NoteGradings::Ungraded};
    GreenNotePrefab* m_prefab{nullptr};
};

#endif