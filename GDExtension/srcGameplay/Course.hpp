#ifndef Course_hpp
#define Course_hpp

#include <cstdint>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../srcThirdParty/json.hpp"
#include "BlueNote.hpp"
#include "CompletionList.hpp"
#include "GhostNote.hpp"
#include "GreenNote.hpp"
#include "RedNote.hpp"
#include "YellowNote.hpp"

void courseLogError(const std::string& message);

class Course
{
public:
    ////////////////////////////////////////////////////////////
    //
    // Information stored in the chart
    //
    ////////////////////////////////////////////////////////////
    int courseNumber;
    int level;
    std::vector<RedNote*> redNotes;
    std::vector<BlueNote*> blueNotes;
    std::vector<YellowNote*> yellowNotes;
    std::vector<GreenNote*> greenNotes;
    std::vector<GhostNote*> ghostNotes;

    ////////////////////////////////////////////////////////////
    //
    // Tacked-on information for the game to use during play
    //
    ////////////////////////////////////////////////////////////
    CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>> laneRed;
    CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>> laneBlue;

    static Course FromJson(const nlohmann::json& j)
    {
        static const std::unordered_map<std::string, std::string> typeToName = {{"1", "red"}, {"2", "blue"}, {"3", "redBig"}, {"4", "blueBig"}, {"5", "yellow"}, {"7", "green"}, {"8", "green"}, {"G", "ghost"}};

        Course course;
        course.courseNumber = j["course"];
        course.level = j["level"];

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
                const std::string& noteTypeName = it->second;

                if (noteTypeName == "red" || noteTypeName == "redBig")
                {
                    RedNote* note = new RedNote(e);
                    course.redNotes.push_back(note);
                }
                else if (noteTypeName == "blue" || noteTypeName == "blueBig")
                {
                    BlueNote* note = new BlueNote(e);
                    course.blueNotes.push_back(note);
                }
                else if (noteTypeName == "yellow")
                {
                    YellowNote* note = new YellowNote(e);
                    course.yellowNotes.push_back(note);
                }
                else if (noteTypeName == "green")
                {
                    GreenNote* note = new GreenNote(e);
                    course.greenNotes.push_back(note);
                }
                else if (noteTypeName == "ghost")
                {
                    GhostNote* note = new GhostNote(e);
                    course.ghostNotes.push_back(note);
                }
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
