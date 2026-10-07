#ifndef BigRedNote_hpp
#define BigRedNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class BigRedNotePrefab;

class BigRedNote : public HittableNote
{
public:
    BigRedNote(const nlohmann::json& j);
    ~BigRedNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);
    NoteGradings getGradingForOfftime(int64_t songPositionPs, const Chart* chart) override;

    BigRedNotePrefab* prefab{nullptr};
};

#endif