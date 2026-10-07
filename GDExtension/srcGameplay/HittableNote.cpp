#include "HittableNote.hpp"
#include "Fraction.hpp"
#include "JsonKeys.hpp"

HittableNote::HittableNote(const nlohmann::json& j)
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

HittableNote::~HittableNote()
{
}

void HittableNote::getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY)
{
    int64_t timeDelta = timePicoseconds - songPositionPicoseconds - visualOffsetPicoseconds;
    outX = HITZONE_CENTER_X + static_cast<double>(timeDelta * SCROLL_SPEED_FACTOR) / 1000000000000.0;
    outY = LANE_Y;
}