#include "BigGhostNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "BigGhostNotePrefab.hpp"

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