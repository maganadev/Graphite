#ifndef GreenNote_hpp
#define GreenNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class GreenNotePrefab;

class GreenNote : public HittableNote
{
public:
    GreenNote(const nlohmann::json& j);
    ~GreenNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

    GreenNotePrefab* prefab{nullptr};
};

#endif