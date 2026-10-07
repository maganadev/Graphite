#include "BigRedNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "BigRedNotePrefab.hpp"

BigRedNote::BigRedNote(const nlohmann::json& j) : HittableNote(j)
{
}

BigRedNote::~BigRedNote()
{
}

void BigRedNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    if (prefab)
    {
        prefab->set_position(godot::Vector2(x, y));
    }
}