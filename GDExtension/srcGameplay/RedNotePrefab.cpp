#include "RedNotePrefab.hpp"

void RedNotePrefab::_bind_methods()
{
    //
}

RedNotePrefab::RedNotePrefab()
{
    //
}

RedNotePrefab::~RedNotePrefab()
{
    //
}

void RedNotePrefab::setNote(const Note& note)
{
    m_note.setNote(note);
}

const Note& RedNotePrefab::getNote() const
{
    return m_note.getNote();
}

void RedNotePrefab::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    m_note.getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    set_position(Vector2(x, y));
}
