#ifndef Course_hpp
#define Course_hpp

#include "../srcThirdParty/json.hpp"
#include "BlueNote.hpp"
#include "CompletionList.hpp"
#include "GhostNote.hpp"
#include "GreenNote.hpp"
#include "JsonKeys.hpp"
#include "RedNote.hpp"
#include "YellowNote.hpp"
#include <cstdint>
#include <cstdlib>
#include <string>
#include <variant>
#include <vector>

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
        Course course;
        course.courseNumber = j[JC_COURSE];
        course.level = j[JC_LEVEL];

        auto readNotes = [&course, &j](const std::string& key, const std::string& noteType)
        {
            if (!j.contains(key))
                return;
            for (const auto& e : j[key])
            {
                if (noteType == NT_RED)
                {
                    RedNote* note = new RedNote(e);
                    course.redNotes.push_back(note);
                }
                else if (noteType == NT_BLUE)
                {
                    BlueNote* note = new BlueNote(e);
                    course.blueNotes.push_back(note);
                }
                else if (noteType == NT_YELLOW)
                {
                    YellowNote* note = new YellowNote(e);
                    course.yellowNotes.push_back(note);
                }
                else if (noteType == NT_GREEN)
                {
                    GreenNote* note = new GreenNote(e);
                    course.greenNotes.push_back(note);
                }
            }
        };

        readNotes(JC_REDNOTES, NT_RED);
        readNotes(JC_BLUENOTES, NT_BLUE);
        readNotes(JC_BIGREDNOTES, NT_RED);
        readNotes(JC_BIGBLUENOTES, NT_BLUE);
        readNotes(JC_YELLOWNOTES, NT_YELLOW);
        readNotes(JC_BIGYELLOWNOTES, NT_YELLOW);
        readNotes(JC_GREENNOTES, NT_GREEN);

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
