#include "RedNote.hpp"
#include "RedNotePrefab.hpp"

RedNote::RedNote()
{
    //
}

RedNote::~RedNote()
{
    //
}

void RedNote::setNote(const Note& note)
{
    m_note = note;
}

const Note& RedNote::getNote() const
{
    return m_note;
}

void RedNote::setPrefab(RedNotePrefab* prefab)
{
    m_prefab = prefab;
}

RedNotePrefab* RedNote::getPrefab() const
{
    return m_prefab;
}

bool RedNote::isJudged() const
{
    return m_judged.load(std::memory_order_acquire);
}

void RedNote::setJudged(NoteGradings grading, int64_t picosecondsOff)
{
    m_grading = grading;
    m_picosecondsOff = picosecondsOff;
    m_judged.store(true, std::memory_order_release);
}

NoteGradings RedNote::getGrading() const
{
    return m_grading;
}

int64_t RedNote::getPicosecondsOff() const
{
    return m_picosecondsOff;
}

void RedNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    if (m_prefab)
    {
        m_prefab->set_position(godot::Vector2(x, y));
    }
}

void RedNote::getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const
{
    const int64_t effectiveNoteTimePs = m_note.time_picoseconds + visualOffsetPicoseconds;
    const int64_t timeUntilNote = effectiveNoteTimePs - songPositionPicoseconds;
    outX = HITZONE_CENTER_X + SCROLL_SPEED * (static_cast<double>(timeUntilNote) / 1.0e12);
    outY = LANE_Y;
}