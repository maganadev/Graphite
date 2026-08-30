#include "BlueNotePrefab.hpp"

void BlueNotePrefab::_bind_methods()
{
    //
}

BlueNotePrefab::BlueNotePrefab()
{
    //
}

BlueNotePrefab::~BlueNotePrefab()
{
    //
}

void BlueNotePrefab::setNote(const Note& note)
{
    m_note.setNote(note);
}

const Note& BlueNotePrefab::getNote() const
{
    return m_note.getNote();
}

void BlueNotePrefab::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    m_note.getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    set_position(Vector2(x, y));
}
