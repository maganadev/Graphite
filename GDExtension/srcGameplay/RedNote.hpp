#ifndef RedNote_hpp
#define RedNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class RedNotePrefab;

class RedNote : public HittableNote
{
public:
    RedNote(const nlohmann::json& j);
    ~RedNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

    RedNotePrefab* prefab{nullptr};
};

#endif