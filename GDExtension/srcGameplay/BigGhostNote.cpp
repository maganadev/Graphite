#include "BigGhostNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "BigGhostNotePrefab.hpp"
#include "Chart.hpp"

BigGhostNote::BigGhostNote(const nlohmann::json& j) : HittableNote(j)
{
}

BigGhostNote::~BigGhostNote()
{
}

void BigGhostNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    if (prefab)
    {
        prefab->set_position(godot::Vector2(x, y));
    }
}

NoteGradings BigGhostNote::getGradingForOfftime(int64_t songPositionPs, const Chart* chart)
{
    int64_t timeDelta = songPositionPs - timePicoseconds;
    const int64_t absDelta = std::abs(timeDelta);
    const bool isEarly = timeDelta < 0;

    if (absDelta > chart->hitWindowAboutToBeOutOfRange)
    {
        if (isEarly)
            return NoteGradings::Early_OutOfRange;
        return NoteGradings::Late_OutOfRange;
    }

    if (absDelta > chart->hitWindowFuka && absDelta <= chart->hitWindowAboutToBeOutOfRange)
    {
        if (isEarly)
            return NoteGradings::Early_AboutToBeOutOfRange;
        return NoteGradings::Late_AboutToBeOutOfRange;
    }

    if (absDelta > chart->hitWindowKa && absDelta <= chart->hitWindowFuka)
    {
        if (isEarly)
            return NoteGradings::Early_Fuka;
        return NoteGradings::Late_Fuka;
    }

    if (absDelta > chart->hitWindowRyou && absDelta <= chart->hitWindowKa)
    {
        if (isEarly)
            return NoteGradings::Early_Ka;
        return NoteGradings::Late_Ka;
    }

    if (absDelta > chart->hitWindowChou && absDelta <= chart->hitWindowRyou)
    {
        if (isEarly)
            return NoteGradings::Early_Ryou;
        return NoteGradings::Late_Ryou;
    }

    if (absDelta <= chart->hitWindowChou)
    {
        if (isEarly)
            return NoteGradings::Early_Chou;
        if (timeDelta == 0)
            return NoteGradings::CompletelyPerfect;
        return NoteGradings::Late_Chou;
    }

    return NoteGradings::Ungraded;
}