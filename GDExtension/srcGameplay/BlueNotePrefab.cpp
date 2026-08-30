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

bool BlueNotePrefab::isJudged() const
{
    return m_note.isJudged();
}

void BlueNotePrefab::setJudged(NoteGradings grading)
{
    m_note.setJudged(grading);
}

NoteGradings BlueNotePrefab::getGrading() const
{
    return m_note.getGrading();
}

void BlueNotePrefab::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    m_note.getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    set_position(Vector2(x, y));
}

BlueNote* BlueNotePrefab::getNote()
{
    return &m_note;
}
