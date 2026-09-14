#include "BlueNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "BlueNotePrefab.hpp"

BlueNote::BlueNote(const nlohmann::json& j) : m_timePicoseconds(j["time_picoseconds"]), m_bpmForScrollDouble(j.value("bpmForScroll_double", 240.0))
{
}

BlueNote::~BlueNote()
{
}

int64_t BlueNote::getTimePicoseconds() const
{
    return m_timePicoseconds;
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

void BlueNote::setJudged(NoteGradings grading, int64_t picosecondsOff)
{
    m_grading = grading;
    m_picosecondsOff = picosecondsOff;
    m_judged.store(true, std::memory_order_release);
}

NoteGradings BlueNote::getGrading() const
{
    return m_grading;
}

int64_t BlueNote::getPicosecondsOff() const
{
    return m_picosecondsOff;
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
