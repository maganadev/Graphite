#ifndef Course_hpp
#define Course_hpp

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../srcThirdParty/json.hpp"
#include "CompletionList.hpp"

class RedNote;
class BlueNote;
class YellowNote;
class GreenNote;

struct Note
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

class Course
{
public:
    std::string name;
    int level;
    std::string bpm;
    std::string offset;
    int64_t offset_picoseconds{0};
    std::vector<Note> notes;
    std::vector<RedNote*> redNotes;
    std::vector<BlueNote*> blueNotes;
    std::vector<YellowNote*> yellowNotes;
    std::vector<GreenNote*> greenNotes;
    CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>> laneRed;
    CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>> laneBlue;

    static Course FromJson(const nlohmann::json& j)
    {
        static const std::unordered_map<int, std::string> valueToType = {
            {1, "red"}, {2, "blue"}, {3, "redBig"}, {4, "blueBig"}};

        Course course;
        course.name = j["name"];
        course.level = j["level"];
        course.bpm = j["bpm"];
        course.offset = j.value("offset", "0/1");
        course.offset_picoseconds = j.value("offset_picoseconds", static_cast<int64_t>(0));

        auto parseNote = [&course](const std::string& type, const std::string& timeFrac, int64_t timePs, double scroll, int measure, bool gogo, bool big, bool visible)
        {
            Note note;
            note.type = type;
            note.time_fractional = timeFrac;
            note.time_picoseconds = timePs;
            note.scroll = scroll;
            note.measure = measure;
            note.gogo = gogo;
            note.big = big;
            note.visible = visible;
            course.notes.push_back(note);
        };

        if (j.contains("notes"))
        {
            for (const auto& e : j["notes"])
            {
                int val = e["value"];
                auto it = valueToType.find(val);
                if (it == valueToType.end())
                    continue;
                std::string type = it->second;
                bool big = (val == 3 || val == 4);
                parseNote(type, e["time"], e["time_picoseconds"], 1.0, 0, false, big, true);
            }
        }
        return course;
    }

    void populateLanes()
    {
        laneRed = CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>>();
        laneBlue = CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>>();
        for (auto* note : redNotes)
            laneRed.push_back(note);
        for (auto* note : blueNotes)
            laneBlue.push_back(note);
        for (auto* note : yellowNotes)
            laneRed.push_back(note);
        for (auto* note : greenNotes)
            laneBlue.push_back(note);
        laneRed.resetCompletionStates();
        laneBlue.resetCompletionStates();
    }
};

#endif
