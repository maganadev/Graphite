#include "Barline.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "BarlinePrefab.hpp"
#include "Chart.hpp"
#include <exception>

Barline::Barline()
{
}

Barline::~Barline()
{
}

void Barline::updatePosition(int64_t correctedSongPositionPs, double userScrollModifier)
{
    if (prefab)
    {
        int64_t timeDelta = startTimePicoseconds - correctedSongPositionPs;
        double x = HITZONE_CENTER_X + (static_cast<double>(timeDelta) * SCROLL_SPEED_FACTOR * scrollBPM * scrollMultiplier * userScrollModifier);
        prefab->set_position(godot::Vector2(x, LANE_Y));
    }
}

void Barline::getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime)
{
    offtime = 0;
    grading = NoteGradings::Late_AboutToBeOutOfRange;
    return;
}

bool Barline::handleStrikeAndGetCompleted(NoteGradings grading, int64_t picosecondsOff)
{
    this->grading = grading;
    this->picosecondsOff = picosecondsOff;
    this->finishedJudging.store(true, std::memory_order_release);
    return true;
}

int64_t Barline::getSpamHitsCount() const
{
    std::terminate();
}

NoteTypes Barline::getNoteType() const
{
    return NoteTypes::Barline;
}