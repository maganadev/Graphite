#ifndef BigGreenNote_hpp
#define BigGreenNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class BigGreenNotePrefab;

class BigGreenNote : public HittableNote
{
public:
    BigGreenNote(const nlohmann::json& j);
    ~BigGreenNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

    BigGreenNotePrefab* prefab{nullptr};
};

#endif