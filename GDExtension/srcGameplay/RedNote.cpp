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

void RedNote::setEvent(const TJAEvent& event)
{
    m_event = event;
}

const TJAEvent& RedNote::getEvent() const
{
    return m_event;
}

void RedNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    const int64_t effectiveNoteTimePs = m_event.time_picoseconds - visualOffsetPicoseconds;
    const int64_t timeUntilNote = effectiveNoteTimePs - songPositionPicoseconds;
    set_position(Vector2(HITZONE_CENTER_X + SCROLL_SPEED * (static_cast<double>(timeUntilNote) / 1.0e12), LANE_Y));
}