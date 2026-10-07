#ifndef BigGhostNote_hpp
#define BigGhostNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class BigGhostNotePrefab;

class BigGhostNote : public HittableNote
{
public:
    BigGhostNote(const nlohmann::json& j);
    ~BigGhostNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);
    NoteGradings getGradingForOfftime(int64_t songPositionPs, const Chart* chart) override;

    BigGhostNotePrefab* prefab{nullptr};
};

#endif