#include "BlueNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "BlueNotePrefab.hpp"
#include "Chart.hpp"

BlueNote::BlueNote(const nlohmann::json& j) : HittableNote(j)
{
}

BlueNote::~BlueNote()
{
}

void BlueNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    if (prefab)
    {
        prefab->set_position(godot::Vector2(x, y));
    }
}

void BlueNote::getGradingForOfftime(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime)
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

void BlueNote::setJudged(NoteGradings grading, int64_t picosecondsOff)
{
    this->grading = grading;
    this->picosecondsOff = picosecondsOff;
    this->judged.store(true, std::memory_order_release);
}