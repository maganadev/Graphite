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

void GreenNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    if (prefab)
    {
        int64_t timeDelta = (startTimePicoseconds - songPositionPicoseconds) + visualOffsetPicoseconds;
        double x = HITZONE_CENTER_X + static_cast<double>(timeDelta) * SCROLL_SPEED_FACTOR * scrollBPM;
        prefab->set_position(godot::Vector2(x, LANE_Y));
    }
}

void GreenNote::getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime)
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

void GreenNote::handleGrading(NoteGradings grading, int64_t picosecondsOff)
{
    remainingSpamHits.fetch_add(-1, std::memory_order_release);
}

void GreenNote::constructor2(const nlohmann::json& j)
{
    startTimePicoseconds = parseStartTimePicoseconds(j);
    scrollBPM = parseScrollBPM(j);
    endTimePicoseconds = parseStopTimePicoseconds(j);
    remainingSpamHits = parseGreenNoteHits(j);
}

int64_t GreenNote::getSpamHitsCount() const
{
    return remainingSpamHits.load(std::memory_order_acquire);
}

bool GreenNote::isSpamNote() const
{
    return true;
}