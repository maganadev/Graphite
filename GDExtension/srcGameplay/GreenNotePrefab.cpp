#include "GreenNotePrefab.hpp"

void GreenNotePrefab::_bind_methods()
{
    //
}

GreenNotePrefab::GreenNotePrefab()
{
    //
}

GreenNotePrefab::~GreenNotePrefab()
{
    //
}

void GreenNotePrefab::setNote(const Note& note)
{
    m_note.setNote(note);
}

const Note& GreenNotePrefab::getNote() const
{
    return m_note.getNote();
}

void GreenNotePrefab::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    m_note.getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    set_position(Vector2(x, y));
}
