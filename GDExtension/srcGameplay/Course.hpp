#ifndef Course_hpp
#define Course_hpp

#include "../srcThirdParty/json.hpp"
#include "Barline.hpp"
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
#include "RedNote.hpp"
#include "YellowNote.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

typedef HittableNote* HittableNoteVariant;

struct Measure
{
    int measureIndex{0};
    double lastScrollMultiplier{1.0};
    double lastBpm{0.0};
};

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

    std::vector<Barline*> barlineNotes;

    std::vector<Measure> measures;

    ////////////////////////////////////////////////////////////
    //
    // Tacked-on information for the game to use during play
    //
    ////////////////////////////////////////////////////////////
    CompletionList<HittableNoteVariant> laneRed;
    CompletionList<HittableNoteVariant> laneBlue;
    CompletionList<HittableNoteVariant> hitSpamNoteList;

    void populateLanes()
    {
        laneRed = CompletionList<HittableNoteVariant>();
        laneBlue = CompletionList<HittableNoteVariant>();
        hitSpamNoteList = CompletionList<HittableNoteVariant>();

        std::vector<HittableNoteVariant> redLaneNotes;
        std::vector<HittableNoteVariant> blueLaneNotes;
        std::vector<HittableNoteVariant> hitSpamNotes;

        for (auto* note : redNotes)
        {
            redLaneNotes.push_back(note);
        }
        for (auto* note : blueNotes)
        {
            blueLaneNotes.push_back(note);
        }
        for (auto* note : yellowNotes)
        {
            redLaneNotes.push_back(note);
        }
        for (auto* note : greenNotes)
        {
            redLaneNotes.push_back(note);
        }
        for (auto* note : bigRedNotes)
        {
            redLaneNotes.push_back(note);
        }
        for (auto* note : bigBlueNotes)
        {
            blueLaneNotes.push_back(note);
        }
        for (auto* note : bigYellowNotes)
        {
            redLaneNotes.push_back(note);
        }
        for (auto* note : bigGreenNotes)
        {
            redLaneNotes.push_back(note);
        }
        for (auto* note : yellowNotes)
        {
            hitSpamNotes.push_back(note);
        }
        for (auto* note : greenNotes)
        {
            hitSpamNotes.push_back(note);
        }
        for (auto* note : bigYellowNotes)
        {
            hitSpamNotes.push_back(note);
        }
        for (auto* note : bigGreenNotes)
        {
            hitSpamNotes.push_back(note);
        }

        std::sort(redLaneNotes.begin(), redLaneNotes.end(), [](const HittableNoteVariant& a, const HittableNoteVariant& b) { return a->startTimePicoseconds < b->startTimePicoseconds; });
        std::sort(blueLaneNotes.begin(), blueLaneNotes.end(), [](const HittableNoteVariant& a, const HittableNoteVariant& b) { return a->startTimePicoseconds < b->startTimePicoseconds; });
        std::sort(hitSpamNotes.begin(), hitSpamNotes.end(), [](const HittableNoteVariant& a, const HittableNoteVariant& b) { return a->startTimePicoseconds < b->startTimePicoseconds; });

        for (auto* note : redLaneNotes)
        {
            laneRed.push_back(note);
        }
        for (auto* note : blueLaneNotes)
        {
            laneBlue.push_back(note);
        }
        for (auto* note : hitSpamNotes)
        {
            hitSpamNoteList.push_back(note);
        }

        laneRed.resetCompletionStates();
        laneBlue.resetCompletionStates();
        hitSpamNoteList.resetCompletionStates();
    }
};

#endif