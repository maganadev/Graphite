#ifndef Course_hpp
#define Course_hpp

#include <cstdint>
#include <cstdlib>
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
class GhostNote;

struct Note
{
    std::string type;
    std::string time_fractional;
    int64_t time_picoseconds;
    std::string bpmForScroll_fractional;
    double bpmForScroll_double{0.0};
    double scroll;
    int measure;
    bool gogo;
    bool big;
    bool visible;
};

void courseLogError(const std::string& message);

class Course
{
public:
    int courseNumber;
    int level;
    std::vector<Note> notes;
    std::vector<RedNote*> redNotes;
    std::vector<BlueNote*> blueNotes;
    std::vector<YellowNote*> yellowNotes;
    std::vector<GreenNote*> greenNotes;
    std::vector<GhostNote*> ghostNotes;
    CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>> laneRed;
    CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>> laneBlue;

    static Course FromJson(const nlohmann::json& j)
    {
        static const std::unordered_map<std::string, std::string> typeToName = {
            {"1", "red"}, {"2", "blue"}, {"3", "redBig"}, {"4", "blueBig"}, {"5", "yellow"}, {"7", "green"}, {"8", "green"}, {"G", "ghost"}};

        Course course;
        course.courseNumber = j["course"];
        course.level = j["level"];

        auto parseNote = [&course](const std::string& type, const std::string& timeFrac, int64_t timePs, const std::string& bpmFrac, double bpmDouble, double scroll, int measure, bool gogo, bool big, bool visible)
        {
            Note note;
            note.type = type;
            note.time_fractional = timeFrac;
            note.time_picoseconds = timePs;
            note.bpmForScroll_fractional = bpmFrac;
            note.bpmForScroll_double = bpmDouble;
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
                std::string typeStr = e["type"];
                auto it = typeToName.find(typeStr);
                if (it == typeToName.end())
                {
                    courseLogError("Unknown note type string \"" + typeStr + "\" at time " + e["time"].get<std::string>());
                    continue;
                }
                bool big = (typeStr == "3" || typeStr == "4");
                parseNote(it->second, e["time"], e["time_picoseconds"], e.value("bpmForScroll_fractional", std::string("240/1")), e.value("bpmForScroll_double", 240.0), 1.0, 0, false, big, true);
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
            laneRed.push_back(note);
        laneRed.resetCompletionStates();
        laneBlue.resetCompletionStates();
    }
};

#endif
