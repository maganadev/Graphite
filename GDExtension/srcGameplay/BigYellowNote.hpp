#ifndef BigYellowNote_hpp
#define BigYellowNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class BigYellowNotePrefab;

class BigYellowNote : public HittableNote
{
public:
    BigYellowNote(const nlohmann::json& j);
    ~BigYellowNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

    BigYellowNotePrefab* prefab{nullptr};
};

#endif