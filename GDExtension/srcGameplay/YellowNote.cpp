#include "YellowNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "Chart.hpp"
#include "YellowNotePrefab.hpp"

YellowNote::YellowNote(const nlohmann::json& j) : HittableNote(j)
{
}

YellowNote::~YellowNote()
{
}

void YellowNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    if (prefab)
    {
        prefab->set_position(godot::Vector2(x, y));
    }
}

void YellowNote::getGradingForOfftime(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime)
{
    offtime = 0;
    grading = NoteGradings::Late_AboutToBeOutOfRange;
    return;
}

void YellowNote::setJudged(NoteGradings grading, int64_t picosecondsOff)
{
    this->grading = grading;
    this->picosecondsOff = picosecondsOff;
    this->judged.store(true, std::memory_order_release);
}