#include "YellowNote.hpp"
#include "../src/GraphiteGlobals.hpp"
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