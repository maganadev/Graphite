#ifndef BlueNote_hpp
#define BlueNote_hpp

#include <atomic>
#include <cstdint>

#include <godot_cpp/classes/sprite2d.hpp>

#include "Course.hpp"
#include "RhythmEnums.hpp"

class BlueNote : public godot::Sprite2D
{
public:
    GDCLASS(BlueNote, godot::Sprite2D)

public:
    static void _bind_methods();

    static constexpr double SCROLL_SPEED = 1600.0;
    static constexpr double LANE_Y = 386.0;
    static constexpr double HITZONE_CENTER_X = 618.0;

    BlueNote();
    ~BlueNote();

    void setNote(const Note& note);
    const Note& getNote() const;

    bool isJudged() const;
    void setJudged(NoteGradings grading);
    NoteGradings getGrading() const;

public:
    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

    void getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const;

private:
    Note m_note;
    std::atomic<bool> m_judged{false};
    NoteGradings m_grading{NoteGradings::Ungraded};
};

#endif
