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
    NoteGradings getGradingForOfftime(int64_t songPositionPs, const Chart* chart) override;

    BigYellowNotePrefab* prefab{nullptr};
};

#endif