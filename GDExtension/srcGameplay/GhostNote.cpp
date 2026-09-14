#include "GhostNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "GhostNotePrefab.hpp"

GhostNote::GhostNote(const nlohmann::json& j) : m_timePicoseconds(j["time_picoseconds"]), m_bpmForScrollDouble(j.value("bpmForScroll_double", 240.0))
{
}

GhostNote::~GhostNote()
{
}

int64_t GhostNote::getTimePicoseconds() const
{
    return m_timePicoseconds;
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
    const int64_t effectiveNoteTimePs = m_timePicoseconds + visualOffsetPicoseconds;
    const int64_t timeUntilNote = effectiveNoteTimePs - songPositionPicoseconds;
    double scrollXOffset = (SCROLL_SPEED_FACTOR * m_bpmForScrollDouble * static_cast<double>(timeUntilNote));
    if (GraphiteGlobals::modAudioOffsetCalibration)
    {
        scrollXOffset *= 0.125;
    }
    outX = scrollXOffset + HITZONE_CENTER_X;
    outY = LANE_Y;
}
