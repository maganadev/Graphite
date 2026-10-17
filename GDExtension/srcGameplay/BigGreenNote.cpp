#include "BigGreenNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "BigGreenNotePrefab.hpp"
#include "Chart.hpp"

BigGreenNote::BigGreenNote()
{
}

BigGreenNote::~BigGreenNote()
{
}

void BigGreenNote::updatePosition(int64_t correctedSongPositionPs, double userScrollModifier)
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
            int64_t timeDelta = startTimePicoseconds - endTimePicoseconds;
            x = HITZONE_CENTER_X + (static_cast<double>(timeDelta) * SCROLL_SPEED_FACTOR * scrollBPM * scrollMultiplier * userScrollModifier);
        }
        prefab->set_position(godot::Vector2(x, LANE_Y));
    }
}

void BigGreenNote::getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime)
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

bool BigGreenNote::handleStrikeAndGetCompleted(NoteGradings grading, int64_t picosecondsOff)
{
    if (remainingSpamHits.fetch_sub(1, std::memory_order_acq_rel) <= 1)
    {
        this->grading = grading;
        this->finishedJudging.store(true, std::memory_order_release);
        GraphiteGlobals::audioEngine.value().playAudioTrack(GraphiteGlobals::greenNoteCompleteHitsoundHandle);
        return true;
    }
    return false;
}

int64_t BigGreenNote::getSpamHitsCount() const
{
    return remainingSpamHits.load(std::memory_order_acquire);
}

NoteTypes BigGreenNote::getNoteType() const
{
    return NoteTypes::GreenNoteLarge;
}