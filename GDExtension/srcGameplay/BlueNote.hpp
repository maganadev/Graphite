#ifndef BlueNote_hpp
#define BlueNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class BlueNotePrefab;

class BlueNote : public HittableNote
{
public:
    BlueNote(const nlohmann::json& j);
    ~BlueNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);
    NoteGradings getGradingForOfftime(int64_t songPositionPs, const Chart* chart) override;

    BlueNotePrefab* prefab{nullptr};
};

#endif