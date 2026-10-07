#ifndef YellowNote_hpp
#define YellowNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class YellowNotePrefab;

class YellowNote : public HittableNote
{
public:
    YellowNote(const nlohmann::json& j);
    ~YellowNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

    YellowNotePrefab* prefab{nullptr};
};

#endif