#include "BlueNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "BlueNotePrefab.hpp"
#include "Fraction.hpp"
#include "JsonKeys.hpp"

BlueNote::BlueNote(const nlohmann::json& j) : timePicoseconds(0), bpmForScrollDouble(240.0)
{
    std::string startStr = j.value(JC_STARTTIME_F, "0/1");
    bool ok;
    Fraction f;
    f.assignFromString(startStr, ok);
    if (ok)
    {
        Fraction ps;
        ps.assignFromUInt64(1000000000000ULL);
        f.multiply(ps);
        bool intOk;
        timePicoseconds = f.toInt(intOk);
    }
    std::string bpmStr = j.value(JC_BPMSCROLL_F, "120/1");
    ok = false;
    Fraction bpmFrac;
    bpmFrac.assignFromString(bpmStr, ok);
    if (ok)
    {
        bool dblOk;
        bpmForScrollDouble = bpmFrac.toDouble(dblOk);
    }
}

BlueNote::~BlueNote()
{
}

void BlueNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    if (prefab)
    {
        prefab->set_position(godot::Vector2(x, y));
    }
}

void BlueNote::getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const
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
