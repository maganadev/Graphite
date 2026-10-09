#include "RedNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "Chart.hpp"
#include "RedNotePrefab.hpp"
#include <exception>

RedNote::RedNote()
{
}

RedNote::~RedNote()
{
}

void RedNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    if (prefab)
    {
        int64_t timeDelta = (startTimePicoseconds - songPositionPicoseconds) + visualOffsetPicoseconds;
        double x = HITZONE_CENTER_X + static_cast<double>(timeDelta) * SCROLL_SPEED_FACTOR * scrollBPM;
        prefab->set_position(godot::Vector2(x, LANE_Y));
    }
}

void RedNote::getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime)
{
    int64_t timeDelta = songPositionPs - startTimePicoseconds;
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

bool RedNote::handleStrikeAndGetCompleted(NoteGradings grading, int64_t picosecondsOff)
{
    this->grading = grading;
    this->picosecondsOff = picosecondsOff;
    this->finishedJudging.store(true, std::memory_order_release);
    return true;
}

void RedNote::constructor2(const nlohmann::json& j)
{
    startTimePicoseconds = parseStartTimePicoseconds(j);
    scrollBPM = parseScrollBPM(j);
}

int64_t RedNote::getSpamHitsCount() const
{
    std::terminate();
}