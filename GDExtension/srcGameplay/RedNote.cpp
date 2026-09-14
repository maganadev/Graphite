#include "RedNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "RedNotePrefab.hpp"

RedNote::RedNote(const nlohmann::json& j) : m_timePicoseconds(j["time_picoseconds"]), m_bpmForScrollDouble(j.value("bpmForScroll_double", 240.0))
{
}

RedNote::~RedNote()
{
}

int64_t RedNote::getTimePicoseconds() const
{
    return m_timePicoseconds;
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
