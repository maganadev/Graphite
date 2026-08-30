#include "RedNote.hpp"

void RedNote::_bind_methods()
{
    //
}

RedNote::RedNote()
{
    //
}

RedNote::~RedNote()
{
    //
}

void RedNote::setNote(const Note& note)
{
    m_note = note;
}

const Note& RedNote::getNote() const
{
    return m_note;
}

bool RedNote::isJudged() const
{
    return m_judged.load(std::memory_order_acquire);
}

void RedNote::setJudged(NoteGradings grading)
{
    m_grading = grading;
    m_judged.store(true, std::memory_order_release);
}

NoteGradings RedNote::getGrading() const
{
    return m_grading;
}

void RedNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    const int64_t effectiveNoteTimePs = m_note.time_picoseconds - visualOffsetPicoseconds;
    const int64_t timeUntilNote = effectiveNoteTimePs - songPositionPicoseconds;
    set_position(godot::Vector2(x, y));
}
