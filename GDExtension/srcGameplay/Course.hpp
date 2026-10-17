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
    CompletionList<HittableNoteVariant> laneCombined;
    CompletionList<HittableNoteVariant> hitSpamNoteList;

    void populateLanes()
    {
        laneCombined = CompletionList<HittableNoteVariant>();
        hitSpamNoteList = CompletionList<HittableNoteVariant>();

        std::vector<HittableNoteVariant> laneNotes;
        std::vector<HittableNoteVariant> hitSpamNotes;

        auto addNote = [&laneNotes, &hitSpamNotes](HittableNoteVariant note)
        {
            laneNotes.push_back(note);
            if (HittableNote::isSpamNote(note->getNoteType()))
            {
                hitSpamNotes.push_back(note);
            }
        };

        for (auto* note : redNotes)
        {
            addNote(note);
        }
        for (auto* note : blueNotes)
        {
            addNote(note);
        }
        for (auto* note : yellowNotes)
        {
            addNote(note);
        }
        for (auto* note : greenNotes)
        {
            addNote(note);
        }
        for (auto* note : bigRedNotes)
        {
            addNote(note);
        }
        for (auto* note : bigBlueNotes)
        {
            addNote(note);
        }
        for (auto* note : bigYellowNotes)
        {
            addNote(note);
        }
        for (auto* note : bigGreenNotes)
        {
            addNote(note);
        }

        std::sort(laneNotes.begin(), laneNotes.end(), [](const HittableNoteVariant& a, const HittableNoteVariant& b) { return a->startTimePicoseconds < b->startTimePicoseconds; });
        std::sort(hitSpamNotes.begin(), hitSpamNotes.end(), [](const HittableNoteVariant& a, const HittableNoteVariant& b) { return a->startTimePicoseconds < b->startTimePicoseconds; });

        for (auto* note : laneNotes)
        {
            laneCombined.push_back(note);
        }
        for (auto* note : hitSpamNotes)
        {
            hitSpamNoteList.push_back(note);
        }

        laneCombined.resetCompletionStates();
        hitSpamNoteList.resetCompletionStates();
    }
};

#endif