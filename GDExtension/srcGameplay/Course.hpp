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
    CompletionList<HittableNoteVariant> hitSpamNoteList;

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

        readNotesFn(JC_REDNOTES,
                    [&course](const nlohmann::json& e)
                    {
                        RedNote* note = new RedNote();
                        note->constructor2(e);
                        course.redNotes.push_back(note);
                    });
        readNotesFn(JC_BLUENOTES,
                    [&course](const nlohmann::json& e)
                    {
                        BlueNote* note = new BlueNote();
                        note->constructor2(e);
                        course.blueNotes.push_back(note);
                    });
        readNotesFn(JC_BIGREDNOTES,
                    [&course](const nlohmann::json& e)
                    {
                        BigRedNote* note = new BigRedNote();
                        note->constructor2(e);
                        course.bigRedNotes.push_back(note);
                    });
        readNotesFn(JC_BIGBLUENOTES,
                    [&course](const nlohmann::json& e)
                    {
                        BigBlueNote* note = new BigBlueNote();
                        note->constructor2(e);
                        course.bigBlueNotes.push_back(note);
                    });
        readNotesFn(JC_YELLOWNOTES,
                    [&course](const nlohmann::json& e)
                    {
                        YellowNote* note = new YellowNote();
                        note->constructor2(e);
                        course.yellowNotes.push_back(note);
                    });
        readNotesFn(JC_BIGYELLOWNOTES,
                    [&course](const nlohmann::json& e)
                    {
                        BigYellowNote* note = new BigYellowNote();
                        note->constructor2(e);
                        course.bigYellowNotes.push_back(note);
                    });
        readNotesFn(JC_GREENNOTES,
                    [&course](const nlohmann::json& e)
                    {
                        GreenNote* note = new GreenNote();
                        note->constructor2(e);
                        course.greenNotes.push_back(note);
                    });
        readNotesFn(JC_BIGGREENNOTES,
                    [&course](const nlohmann::json& e)
                    {
                        BigGreenNote* note = new BigGreenNote();
                        note->constructor2(e);
                        course.bigGreenNotes.push_back(note);
                    });
        readNotesFn(JC_GHOSTNOTES,
                    [&course](const nlohmann::json& e)
                    {
                        GhostNote* note = new GhostNote();
                        note->constructor2(e);
                        course.ghostNotes.push_back(note);
                    });
        readNotesFn(JC_BIGGHOSTNOTES,
                    [&course](const nlohmann::json& e)
                    {
                        BigGhostNote* note = new BigGhostNote();
                        note->constructor2(e);
                        course.bigGhostNotes.push_back(note);
                    });

        return course;
    }

    void populateLanes()
    {
        laneRed = CompletionList<HittableNoteVariant>();
        laneBlue = CompletionList<HittableNoteVariant>();
        hitSpamNoteList = CompletionList<HittableNoteVariant>();

        std::vector<HittableNoteVariant> redLaneNotes;
        std::vector<HittableNoteVariant> blueLaneNotes;
        std::vector<HittableNoteVariant> hitSpamNotes;

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
        for (auto* note : yellowNotes)
            hitSpamNotes.push_back(note);
        for (auto* note : greenNotes)
            hitSpamNotes.push_back(note);
        for (auto* note : bigYellowNotes)
            hitSpamNotes.push_back(note);
        for (auto* note : bigGreenNotes)
            hitSpamNotes.push_back(note);

        std::sort(redLaneNotes.begin(), redLaneNotes.end(), [](const HittableNoteVariant& a, const HittableNoteVariant& b) { return a->startTimePicoseconds < b->startTimePicoseconds; });
        std::sort(blueLaneNotes.begin(), blueLaneNotes.end(), [](const HittableNoteVariant& a, const HittableNoteVariant& b) { return a->startTimePicoseconds < b->startTimePicoseconds; });
        std::sort(hitSpamNotes.begin(), hitSpamNotes.end(), [](const HittableNoteVariant& a, const HittableNoteVariant& b) { return a->startTimePicoseconds < b->startTimePicoseconds; });

        for (auto* note : redLaneNotes)
            laneRed.push_back(note);
        for (auto* note : blueLaneNotes)
            laneBlue.push_back(note);
        for (auto* note : hitSpamNotes)
            hitSpamNoteList.push_back(note);

        laneRed.resetCompletionStates();
        laneBlue.resetCompletionStates();
        hitSpamNoteList.resetCompletionStates();
    }
};

#endif