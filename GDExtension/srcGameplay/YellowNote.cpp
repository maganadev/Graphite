#include "YellowNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "YellowNotePrefab.hpp"

YellowNote::YellowNote(const nlohmann::json& j) : m_timePicoseconds(j["time_picoseconds"]), m_bpmForScrollDouble(j.value("bpmForScroll_double", 240.0))
{
}

YellowNote::~YellowNote()
{
}

int64_t YellowNote::getTimePicoseconds() const
{
    return m_timePicoseconds;
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

void YellowNote::setJudged(NoteGradings grading, int64_t picosecondsOff)
{
    m_grading = grading;
    m_picosecondsOff = picosecondsOff;
    m_judged.store(true, std::memory_order_release);
}

NoteGradings YellowNote::getGrading() const
{
    return m_grading;
}

int64_t YellowNote::getPicosecondsOff() const
{
    return m_picosecondsOff;
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
