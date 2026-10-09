#include "BigYellowNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "BigYellowNotePrefab.hpp"
#include "Chart.hpp"

BigYellowNote::BigYellowNote()
{
}

BigYellowNote::~BigYellowNote()
{
}

void BigYellowNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    if (prefab)
    {
        int64_t timeDelta = (startTimePicoseconds - songPositionPicoseconds) + visualOffsetPicoseconds;
        double x = HITZONE_CENTER_X + static_cast<double>(timeDelta) * SCROLL_SPEED_FACTOR * scrollBPM;
        prefab->set_position(godot::Vector2(x, LANE_Y));
    }
}

void BigYellowNote::getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime)
{
    int64_t startDelta = songPositionPs - startTimePicoseconds;

    if (startDelta < -HIT_SPAM_WINDOW_PS)
    {
        offtime = startDelta;
        grading = NoteGradings::Early_OutOfRange;
        return;
    }

    if (startDelta < 0)
    {
        offtime = startDelta;
        grading = NoteGradings::Early_AboutToBeOutOfRange;
        return;
    }

    if (songPositionPs <= endTimePicoseconds)
    {
        offtime = 0;
        grading = NoteGradings::CompletelyPerfect;
        return;
    }

    int64_t endDelta = songPositionPs - endTimePicoseconds;
    offtime = endDelta;
    if (endDelta <= HIT_SPAM_WINDOW_PS)
    {
        grading = NoteGradings::Late_AboutToBeOutOfRange;
        return;
    }

    grading = NoteGradings::Late_OutOfRange;
}

bool BigYellowNote::handleStrikeAndGetCompleted(NoteGradings grading, int64_t picosecondsOff)
{
    spamHits.fetch_add(1, std::memory_order_release);
    return false;
}

void BigYellowNote::constructor2(const nlohmann::json& j)
{
    startTimePicoseconds = parseStartTimePicoseconds(j);
    scrollBPM = parseScrollBPM(j);
    endTimePicoseconds = parseStopTimePicoseconds(j);
}

int64_t BigYellowNote::getSpamHitsCount() const
{
    return spamHits.load(std::memory_order_acquire);
}

bool BigYellowNote::isSpamNote() const
{
    return true;
}