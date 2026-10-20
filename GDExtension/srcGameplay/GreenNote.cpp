#include "GreenNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "Chart.hpp"
#include "GreenNotePrefab.hpp"

GreenNote::GreenNote()
{
}

GreenNote::~GreenNote()
{
}

void GreenNote::updatePosition(int64_t correctedSongPositionPs, double userScrollModifier)
{
    if (prefab)
    {
        double x = 0;
        if (correctedSongPositionPs < startTimePicoseconds)
        {
            int64_t timeDelta = startTimePicoseconds - correctedSongPositionPs;
            x = HITZONE_CENTER_X + (static_cast<double>(timeDelta) * SCROLL_SPEED_FACTOR * scrollBPM * scrollMultiplier * userScrollModifier);
        }
        else if (correctedSongPositionPs <= endTimePicoseconds)
        {
            x = HITZONE_CENTER_X;
        }
        else
        {
            int64_t timeDelta = endTimePicoseconds - correctedSongPositionPs;
            x = HITZONE_CENTER_X + (static_cast<double>(timeDelta) * SCROLL_SPEED_FACTOR * scrollBPM * scrollMultiplier * userScrollModifier);
        }
        prefab->set_position(godot::Vector2(x, LANE_Y));
    }
}

void GreenNote::getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime)
{
    offtime = 0;

    if ((songPositionPs - startTimePicoseconds) < -HIT_SPAM_WINDOW_PS)
    {
        grading = NoteGradings::Early_OutOfRange;
        return;
    }

    if ((songPositionPs - startTimePicoseconds) < 0)
    {
        grading = NoteGradings::Early_AboutToBeOutOfRange;
        return;
    }

    if (songPositionPs <= endTimePicoseconds)
    {
        grading = NoteGradings::CompletelyPerfect;
        return;
    }

    if ((songPositionPs - endTimePicoseconds) <= HIT_SPAM_WINDOW_PS)
    {
        grading = NoteGradings::Late_AboutToBeOutOfRange;
        return;
    }

    grading = NoteGradings::Late_OutOfRange;
}

bool GreenNote::handleStrikeAndGetCompleted(NoteGradings grading, int64_t picosecondsOff)
{
    if ((remainingSpamHits.fetch_sub(1, std::memory_order_acq_rel)) <= 1)
    {
        this->grading = grading;
        this->finishedJudging.store(true, std::memory_order_release);
        GraphiteGlobals::logAudioResult(GraphiteGlobals::audioEngine.value().playAudioTrack(GraphiteGlobals::greenNoteCompleteHitsoundHandle), "playAudioTrack");
        return true;
    }
    return false;
}

int64_t GreenNote::getSpamHitsCount() const
{
    return remainingSpamHits.load(std::memory_order_acquire);
}

NoteTypes GreenNote::getNoteType() const
{
    return NoteTypes::GreenNoteSmall;
}