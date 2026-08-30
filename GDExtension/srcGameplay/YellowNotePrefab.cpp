#include "YellowNotePrefab.hpp"

void YellowNotePrefab::_bind_methods()
{
    //
}

YellowNotePrefab::YellowNotePrefab()
{
    //
}

YellowNotePrefab::~YellowNotePrefab()
{
    //
}

void YellowNotePrefab::setNote(const Note& note)
{
    m_note.setNote(note);
}

const Note& YellowNotePrefab::getNote() const
{
    return m_note.getNote();
}

void YellowNotePrefab::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    m_note.getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    set_position(Vector2(x, y));
}
