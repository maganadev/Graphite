#include "GhostNote.hpp"
#include "GhostNotePrefab.hpp"
#include "../src/GraphiteGlobals.hpp"

GhostNote::GhostNote()
{
    //
}

GhostNote::~GhostNote()
{
    //
}

void GhostNote::setNote(const Note& note)
{
    m_note = note;
}

const Note& GhostNote::getNote() const
{
    return m_note;
}

void GhostNote::setPrefab(GhostNotePrefab* prefab)
{
    m_prefab = prefab;
}

GhostNotePrefab* GhostNote::getPrefab() const
{
    return m_prefab;
}

void GhostNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    if (m_prefab)
    {
        m_prefab->set_position(godot::Vector2(x, y));
    }
}

void GhostNote::getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const
{
    const int64_t effectiveNoteTimePs = m_note.time_picoseconds + visualOffsetPicoseconds;
    const int64_t timeUntilNote = effectiveNoteTimePs - songPositionPicoseconds;
    double scrollXOffset = (SCROLL_SPEED_FACTOR * m_note.bpmForScroll_double * static_cast<double>(timeUntilNote));
    if (GraphiteGlobals::modAudioOffsetCalibration)
    {
        scrollXOffset *= 0.125;
    }
    outX = scrollXOffset + HITZONE_CENTER_X;
    outY = LANE_Y;
}