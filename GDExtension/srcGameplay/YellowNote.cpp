#include "YellowNote.hpp"
#include "YellowNotePrefab.hpp"

YellowNote::YellowNote()
{
    //
}

YellowNote::~YellowNote()
{
    //
}

void YellowNote::setNote(const Note& note)
{
    m_note = note;
}

const Note& YellowNote::getNote() const
{
    return m_note;
}

void YellowNote::setPrefab(YellowNotePrefab* prefab)
{
    m_prefab = prefab;
}

YellowNotePrefab* YellowNote::getPrefab() const
{
    return m_prefab;
}

bool YellowNote::isJudged() const
{
    return m_judged.load(std::memory_order_acquire);
}

void YellowNote::setJudged(NoteGradings grading)
{
    m_grading = grading;
    m_judged.store(true, std::memory_order_release);
}

NoteGradings YellowNote::getGrading() const
{
    return m_grading;
}

void YellowNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    if (m_prefab)
    {
        m_prefab->set_position(godot::Vector2(x, y));
    }
}

void YellowNote::getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const
{
    const int64_t effectiveNoteTimePs = m_note.time_picoseconds - visualOffsetPicoseconds;
    const int64_t timeUntilNote = effectiveNoteTimePs - songPositionPicoseconds;
    outX = HITZONE_CENTER_X + SCROLL_SPEED * (static_cast<double>(timeUntilNote) / 1.0e12);
    outY = LANE_Y;
}