#include "GhostNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "Chart.hpp"
#include "GhostNotePrefab.hpp"
#include <exception>

GhostNote::GhostNote()
{
}

GhostNote::~GhostNote()
{
}

void GhostNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    if (prefab)
    {
        int64_t timeDelta = (startTimePicoseconds - songPositionPicoseconds) + visualOffsetPicoseconds;
        double x = HITZONE_CENTER_X + static_cast<double>(timeDelta) * SCROLL_SPEED_FACTOR * scrollBPM;
        prefab->set_position(godot::Vector2(x, LANE_Y));
    }
}

void GhostNote::getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime)
{
    offtime = 0;
    grading = NoteGradings::Late_AboutToBeOutOfRange;
    return;
}

void GhostNote::handleGrading(NoteGradings grading, int64_t picosecondsOff)
{
    this->grading = grading;
    this->picosecondsOff = picosecondsOff;
    this->finishedJudging.store(true, std::memory_order_release);
}

void GhostNote::constructor2(const nlohmann::json& j)
{
    startTimePicoseconds = parseStartTimePicoseconds(j);
    scrollBPM = parseScrollBPM(j);
}

int64_t GhostNote::getSpamHitsCount() const
{
    std::terminate();
}