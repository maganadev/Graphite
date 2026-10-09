#include "BigGhostNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "BigGhostNotePrefab.hpp"
#include "Chart.hpp"
#include <exception>

BigGhostNote::BigGhostNote()
{
}

BigGhostNote::~BigGhostNote()
{
}

void BigGhostNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    if (prefab)
    {
        int64_t timeDelta = (startTimePicoseconds - songPositionPicoseconds) + visualOffsetPicoseconds;
        double x = HITZONE_CENTER_X + static_cast<double>(timeDelta) * SCROLL_SPEED_FACTOR * scrollBPM;
        prefab->set_position(godot::Vector2(x, LANE_Y));
    }
}

void BigGhostNote::getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime)
{
    offtime = 0;
    grading = NoteGradings::Late_AboutToBeOutOfRange;
    return;
}

bool BigGhostNote::handleStrikeAndGetCompleted(NoteGradings grading, int64_t picosecondsOff)
{
    this->grading = grading;
    this->picosecondsOff = picosecondsOff;
    this->finishedJudging.store(true, std::memory_order_release);
    return true;
}

void BigGhostNote::constructor2(const nlohmann::json& j)
{
    startTimePicoseconds = parseStartTimePicoseconds(j);
    scrollBPM = parseScrollBPM(j);
}

int64_t BigGhostNote::getSpamHitsCount() const
{
    std::terminate();
}