#ifndef TJACourse_hpp
#define TJACourse_hpp

#include <cstdint>
#include <string>
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
        Course course;
        course.name = j["name"];
        course.level = j["level"];
        course.bpm = j["bpm"];
        course.offset = j.value("offset", "0/1");
        course.offset_picoseconds = j.value("offset_picoseconds", static_cast<int64_t>(0));
        if (j.contains("events"))
        {
            for (const auto& e : j["events"])
            {
                Note note;
                note.type = e["type"];
                note.time_fractional = e["time_fractional"];
                note.time_picoseconds = e["time_picoseconds"];
                note.scroll = e.value("scroll", 1.0);
                note.measure = e["measure"];
                note.gogo = e.value("gogo", false);
                note.big = e.value("big", false);
                note.visible = e.value("visible", false);
                course.notes.push_back(note);
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
