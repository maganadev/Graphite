#ifndef GhostNote_hpp
#define GhostNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class GhostNotePrefab;

class GhostNote : public HittableNote
{
public:
    GhostNote(const nlohmann::json& j);
    ~GhostNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

    GhostNotePrefab* prefab{nullptr};
};

#endif