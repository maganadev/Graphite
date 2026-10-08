#include "BigGreenNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "BigGreenNotePrefab.hpp"
#include "Chart.hpp"

BigGreenNote::BigGreenNote(const nlohmann::json& j) : HittableNote(j)
{
}

BigGreenNote::~BigGreenNote()
{
}

void BigGreenNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    if (prefab)
    {
        prefab->set_position(godot::Vector2(x, y));
    }
}

void BigGreenNote::getGradingForOfftime(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime)
{
    int64_t startDelta = songPositionPs - timePicoseconds;

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

void BigGreenNote::setJudged(NoteGradings grading, int64_t picosecondsOff)
{
    this->grading = grading;
    this->picosecondsOff = picosecondsOff;
    this->judged.store(true, std::memory_order_release);
}