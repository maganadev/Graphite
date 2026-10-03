#include "RedNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "RedNotePrefab.hpp"

RedNote::RedNote(const nlohmann::json& j) : timePicoseconds(j[JC_STARTTIME_PS]), bpmForScrollDouble(j.value(JC_BPMSCROLL_D, 240.0))
{
}

RedNote::~RedNote()
{
}

void RedNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    if (prefab)
    {
        prefab->set_position(godot::Vector2(x, y));
    }
}

void RedNote::getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const
{
    const int64_t effectiveNoteTimePs = timePicoseconds + visualOffsetPicoseconds;
    const int64_t timeUntilNote = effectiveNoteTimePs - songPositionPicoseconds;
    double scrollXOffset = (SCROLL_SPEED_FACTOR * bpmForScrollDouble * static_cast<double>(timeUntilNote));
    if (GraphiteGlobals::modAudioOffsetCalibration)
    {
        scrollXOffset *= 0.125;
    }
    outX = scrollXOffset + HITZONE_CENTER_X;
    outY = LANE_Y;
}
