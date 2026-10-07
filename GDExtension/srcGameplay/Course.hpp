#ifndef Course_hpp
#define Course_hpp

#include "../srcThirdParty/json.hpp"
#include "BigBlueNote.hpp"
#include "BigGhostNote.hpp"
#include "BigGreenNote.hpp"
#include "BigRedNote.hpp"
#include "BigYellowNote.hpp"
#include "BlueNote.hpp"
#include "CompletionList.hpp"
#include "GhostNote.hpp"
#include "GreenNote.hpp"
#include "HittableNote.hpp"
#include "JsonKeys.hpp"
#include "RedNote.hpp"
#include "YellowNote.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

void courseLogError(const std::string& message);

typedef HittableNote* HittableNoteVariant;

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
    std::vector<BigRedNote*> bigRedNotes;
    std::vector<BigBlueNote*> bigBlueNotes;
    std::vector<BigYellowNote*> bigYellowNotes;
    std::vector<BigGreenNote*> bigGreenNotes;
    std::vector<BigGhostNote*> bigGhostNotes;

    ////////////////////////////////////////////////////////////
    //
    // Tacked-on information for the game to use during play
    //
    ////////////////////////////////////////////////////////////
    CompletionList<HittableNoteVariant> laneRed;
    CompletionList<HittableNoteVariant> laneBlue;

    static Course FromJson(const nlohmann::json& j)
    {
        Course course;
        course.courseNumber = j[JC_COURSE];
        course.level = j[JC_LEVEL];

        auto readNotesFn = [&course, &j](const std::string& key, auto target)
        {
            if (!j.contains(key))
                return;
            for (const auto& e : j[key])
            {
                target(e);
            }
        };

        readNotesFn(JC_REDNOTES, [&course](const nlohmann::json& e) { course.redNotes.push_back(new RedNote(e)); });
        readNotesFn(JC_BLUENOTES, [&course](const nlohmann::json& e) { course.blueNotes.push_back(new BlueNote(e)); });
        readNotesFn(JC_BIGREDNOTES, [&course](const nlohmann::json& e) { course.bigRedNotes.push_back(new BigRedNote(e)); });
        readNotesFn(JC_BIGBLUENOTES, [&course](const nlohmann::json& e) { course.bigBlueNotes.push_back(new BigBlueNote(e)); });
        readNotesFn(JC_YELLOWNOTES, [&course](const nlohmann::json& e) { course.yellowNotes.push_back(new YellowNote(e)); });
        readNotesFn(JC_BIGYELLOWNOTES, [&course](const nlohmann::json& e) { course.bigYellowNotes.push_back(new BigYellowNote(e)); });
        readNotesFn(JC_GREENNOTES, [&course](const nlohmann::json& e) { course.greenNotes.push_back(new GreenNote(e)); });
        readNotesFn(JC_BIGGREENNOTES, [&course](const nlohmann::json& e) { course.bigGreenNotes.push_back(new BigGreenNote(e)); });
        readNotesFn(JC_GHOSTNOTES, [&course](const nlohmann::json& e) { course.ghostNotes.push_back(new GhostNote(e)); });
        readNotesFn(JC_BIGGHOSTNOTES, [&course](const nlohmann::json& e) { course.bigGhostNotes.push_back(new BigGhostNote(e)); });

        return course;
    }

    void populateLanes()
    {
        laneRed = CompletionList<HittableNoteVariant>();
        laneBlue = CompletionList<HittableNoteVariant>();

        std::vector<HittableNoteVariant> redLaneNotes;
        std::vector<HittableNoteVariant> blueLaneNotes;

        for (auto* note : redNotes)
            redLaneNotes.push_back(note);
        for (auto* note : blueNotes)
            blueLaneNotes.push_back(note);
        for (auto* note : yellowNotes)
            redLaneNotes.push_back(note);
        for (auto* note : greenNotes)
            redLaneNotes.push_back(note);
        for (auto* note : bigRedNotes)
            redLaneNotes.push_back(note);
        for (auto* note : bigBlueNotes)
            blueLaneNotes.push_back(note);
        for (auto* note : bigYellowNotes)
            redLaneNotes.push_back(note);
        for (auto* note : bigGreenNotes)
            redLaneNotes.push_back(note);

        std::sort(redLaneNotes.begin(), redLaneNotes.end(), [](const HittableNoteVariant& a, const HittableNoteVariant& b) { return a->timePicoseconds < b->timePicoseconds; });
        std::sort(blueLaneNotes.begin(), blueLaneNotes.end(), [](const HittableNoteVariant& a, const HittableNoteVariant& b) { return a->timePicoseconds < b->timePicoseconds; });

        for (auto* note : redLaneNotes)
            laneRed.push_back(note);
        for (auto* note : blueLaneNotes)
            laneBlue.push_back(note);

        laneRed.resetCompletionStates();
        laneBlue.resetCompletionStates();
    }
};

#endif