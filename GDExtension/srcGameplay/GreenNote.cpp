#include "GreenNote.hpp"

GreenNote::GreenNote()
{
    //
}

GreenNote::~GreenNote()
{
    //
}

void GreenNote::setNote(const Note& note)
{
    m_note = note;
}

const Note& GreenNote::getNote() const
{
    return m_note;
}

bool GreenNote::isJudged() const
{
    return m_judged.load(std::memory_order_acquire);
}

void GreenNote::setJudged(NoteGradings grading)
{
    m_grading = grading;
    m_judged.store(true, std::memory_order_release);
}

NoteGradings GreenNote::getGrading() const
{
    return m_grading;
}

void GreenNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    (void)songPositionPicoseconds;
    (void)visualOffsetPicoseconds;
}

void GreenNote::getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const
{
    const int64_t effectiveNoteTimePs = m_note.time_picoseconds - visualOffsetPicoseconds;
    const int64_t timeUntilNote = effectiveNoteTimePs - songPositionPicoseconds;
    outX = HITZONE_CENTER_X + SCROLL_SPEED * (static_cast<double>(timeUntilNote) / 1.0e12);
    outY = LANE_Y;
}
