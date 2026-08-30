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

bool GreenNotePrefab::isJudged() const
{
    return m_note.isJudged();
}

void GreenNotePrefab::setJudged(NoteGradings grading)
{
    m_note.setJudged(grading);
}

NoteGradings GreenNotePrefab::getGrading() const
{
    return m_note.getGrading();
}

void GreenNotePrefab::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    m_note.getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    set_position(Vector2(x, y));
}

GreenNote* GreenNotePrefab::getNote()
{
    return &m_note;
}
