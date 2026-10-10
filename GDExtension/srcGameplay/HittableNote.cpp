#include "HittableNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "Fraction.hpp"
#include "JsonKeys.hpp"

HittableNote::HittableNote()
{
}

HittableNote::~HittableNote()
{
}

int64_t HittableNote::parseStartTimePicoseconds(const nlohmann::json& j)
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
        int64_t result = f.toInt(intOk);
        if (GraphiteGlobals::playbackRate != 1.0)
            return static_cast<int64_t>(result / GraphiteGlobals::playbackRate);
        return result;
    }
    return 0;
}

double HittableNote::parseScrollBPM(const nlohmann::json& j)
{
    std::string bpmStr = j.value(JC_BPMSCROLL_F, "240/1");
    bool ok = false;
    Fraction bpmFrac;
    bpmFrac.assignFromString(bpmStr, ok);
    if (ok)
    {
        bool dblOk;
        double result = bpmFrac.toDouble(dblOk);
        if (GraphiteGlobals::playbackRate != 1.0)
            return result * GraphiteGlobals::playbackRate;
        return result;
    }
    return 240.0;
}

int64_t HittableNote::parseStopTimePicoseconds(const nlohmann::json& j)
{
    std::string endStr = j.value(JC_STOPTIME_F, "0/1");
    if (endStr.empty())
    {
        return 0;
    }
    bool ok = false;
    Fraction ef;
    ef.assignFromString(endStr, ok);
    if (ok)
    {
        Fraction ps;
        ps.assignFromUInt64(1000000000000ULL);
        ef.multiply(ps);
        bool intOk;
        int64_t result = ef.toInt(intOk);
        if (GraphiteGlobals::playbackRate != 1.0)
            return static_cast<int64_t>(result / GraphiteGlobals::playbackRate);
        return result;
    }
    return 0;
}

int64_t HittableNote::parseGreenNoteHits(const nlohmann::json& j)
{
    if (!j.contains(JC_GREENNOTEHITS))
    {
        return 0;
    }

    const auto& value = j[JC_GREENNOTEHITS];
    if (value.is_number())
    {
        return value.get<int64_t>();
    }

    return 0;
}