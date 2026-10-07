#include "GhostNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "GhostNotePrefab.hpp"

GhostNote::GhostNote(const nlohmann::json& j) : HittableNote(j)
{
}

GhostNote::~GhostNote()
{
}

void GhostNote::updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(songPositionPicoseconds, visualOffsetPicoseconds, x, y);
    if (prefab)
    {
        prefab->set_position(godot::Vector2(x, y));
    }
}