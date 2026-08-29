#include "BlueNote.hpp"

void BlueNote::_bind_methods()
{
    //
}

BlueNote::BlueNote()
{
    //
}

BlueNote::~BlueNote()
{
    //
}

void BlueNote::setEvent(const TJAEvent& event)
{
    m_event = event;
}

const TJAEvent& BlueNote::getEvent() const
{
    return m_event;
}

void BlueNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    if (m_judged.load(std::memory_order_acquire))
    {
        set_position(Vector2(-1000, -1000));
        return;
    }

    const int64_t effectiveNoteTimePs = m_event.time_picoseconds - visualOffsetPicoseconds;
    const int64_t timeUntilNote = effectiveNoteTimePs - songPositionPicoseconds;
    set_position(Vector2(HITZONE_CENTER_X + SCROLL_SPEED * (static_cast<double>(timeUntilNote) / 1.0e12), LANE_Y));
}

bool BlueNote::isJudged() const
{
    return m_judged.load(std::memory_order_acquire);
}

void BlueNote::setJudged(NoteGradings grading)
{
    m_grading = grading;
    m_judged.store(true, std::memory_order_release);
}

NoteGradings BlueNote::getGrading() const
{
    return m_grading;
}