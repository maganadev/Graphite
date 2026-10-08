#include "BigRedNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "BigRedNotePrefab.hpp"
#include "Chart.hpp"

BigRedNote::BigRedNote(const nlohmann::json& j) : HittableNote(j)
{
}

BigRedNote::~BigRedNote()
{
}

void BigRedNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    if (prefab)
    {
        int64_t timeDelta = (timePicoseconds - songPositionPicoseconds) + visualOffsetPicoseconds;
        double x = HITZONE_CENTER_X + static_cast<double>(timeDelta) * SCROLL_SPEED_FACTOR * scrollBPM;
        prefab->set_position(godot::Vector2(x, LANE_Y));
    }
}

void BigRedNote::getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime)
{
    int64_t timeDelta = songPositionPs - timePicoseconds;
    offtime = timeDelta;
    const int64_t absDelta = std::abs(timeDelta);
    const bool isEarly = timeDelta < 0;

    if (absDelta > chart->hitWindowAboutToBeOutOfRange)
    {
        if (isEarly)
            grading = NoteGradings::Early_OutOfRange;
        else
            grading = NoteGradings::Late_OutOfRange;
        return;
    }

    if (absDelta > chart->hitWindowFuka && absDelta <= chart->hitWindowAboutToBeOutOfRange)
    {
        if (isEarly)
            grading = NoteGradings::Early_AboutToBeOutOfRange;
        else
            grading = NoteGradings::Late_AboutToBeOutOfRange;
        return;
    }

    if (absDelta > chart->hitWindowKa && absDelta <= chart->hitWindowFuka)
    {
        if (isEarly)
            grading = NoteGradings::Early_Fuka;
        else
            grading = NoteGradings::Late_Fuka;
        return;
    }

    if (absDelta > chart->hitWindowRyou && absDelta <= chart->hitWindowKa)
    {
        if (isEarly)
            grading = NoteGradings::Early_Ka;
        else
            grading = NoteGradings::Late_Ka;
        return;
    }

    if (absDelta > chart->hitWindowChou && absDelta <= chart->hitWindowRyou)
    {
        if (isEarly)
            grading = NoteGradings::Early_Ryou;
        else
            grading = NoteGradings::Late_Ryou;
        return;
    }

    if (absDelta <= chart->hitWindowChou)
    {
        if (isEarly)
            grading = NoteGradings::Early_Chou;
        else if (timeDelta == 0)
            grading = NoteGradings::CompletelyPerfect;
        else
            grading = NoteGradings::Late_Chou;
        return;
    }

    grading = NoteGradings::Ungraded;
}

void BigRedNote::handleGrading(NoteGradings grading, int64_t picosecondsOff)
{
    this->grading = grading;
    this->picosecondsOff = picosecondsOff;
    this->judged.store(true, std::memory_order_release);
}