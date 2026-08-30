#ifndef RedNote_hpp
#define RedNote_hpp

#include <atomic>
#include <cstdint>

#include <godot_cpp/classes/sprite2d.hpp>

#include "Course.hpp"
#include "RhythmEnums.hpp"

class RedNote : public godot::Sprite2D
{
public:
    GDCLASS(RedNote, godot::Sprite2D)

public:
    static void _bind_methods();

    RedNote();
    ~RedNote();

    void setNote(const Note& note);
    const Note& getNote() const;

    bool isJudged() const;
    void setJudged(NoteGradings grading);
    NoteGradings getGrading() const;

public:
    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

private:
    Note m_note;
    std::atomic<bool> m_judged{false};
    NoteGradings m_grading{NoteGradings::Ungraded};
};

#endif
