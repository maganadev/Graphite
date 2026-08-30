#include "BlueNote.hpp"
#include "BlueNotePrefab.hpp"

BlueNote::BlueNote()
{
    //
}

BlueNote::~BlueNote()
{
    //
}

void BlueNote::setNote(const Note& note)
{
    m_note = note;
}

const Note& BlueNote::getNote() const
{
    return m_note;
}

void BlueNote::setPrefab(BlueNotePrefab* prefab)
{
    m_prefab = prefab;
}

BlueNotePrefab* BlueNote::getPrefab() const
{
    return m_prefab;
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

void BlueNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    if (m_prefab)
    {
        m_prefab->set_position(godot::Vector2(x, y));
    }
}

void BlueNote::getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const
{
    const int64_t effectiveNoteTimePs = m_note.time_picoseconds - visualOffsetPicoseconds;
    const int64_t timeUntilNote = effectiveNoteTimePs - songPositionPicoseconds;
    outX = HITZONE_CENTER_X + SCROLL_SPEED * (static_cast<double>(timeUntilNote) / 1.0e12);
    outY = LANE_Y;
}