#ifndef TJACourse_hpp
#define TJACourse_hpp

#include <cstdint>
#include <string>
#include <vector>

#include "../srcThirdParty/json.hpp"

class RedNote;

using json = nlohmann::json;

struct TJAEvent
{
    std::string type;
    std::string time_fractional;
    int64_t time_picoseconds;
    double scroll;
    int measure;
    bool gogo;
    bool big;
    bool visible;
};

class TJACourse
{
public:
    std::string name;
    int level;
    std::string bpm;
    std::string offset;
    int64_t offset_picoseconds{0};
    std::vector<TJAEvent> events;
    std::vector<RedNote*> spawnedNotes;

    static TJACourse FromJson(const json& j)
    {
        TJACourse course;
        course.name = j["name"];
        course.level = j["level"];
        course.bpm = j["bpm"];
        course.offset = j.value("offset", "0/1");
        course.offset_picoseconds = j.value("offset_picoseconds", static_cast<int64_t>(0));
        if (j.contains("events"))
        {
            for (const auto& e : j["events"])
            {
                TJAEvent ev;
                ev.type = e["type"];
                ev.time_fractional = e["time_fractional"];
                ev.time_picoseconds = e["time_picoseconds"];
                ev.scroll = e.value("scroll", 1.0);
                ev.measure = e["measure"];
                ev.gogo = e.value("gogo", false);
                ev.big = e.value("big", false);
                ev.visible = e.value("visible", false);
                course.events.push_back(ev);
            }
        }
        return course;
    }
};

#endif
