#ifndef BigBlueNote_hpp
#define BigBlueNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class BigBlueNotePrefab;

class BigBlueNote : public HittableNote
{
public:
    BigBlueNote(const nlohmann::json& j);
    ~BigBlueNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

    BigBlueNotePrefab* prefab{nullptr};
};

#endif