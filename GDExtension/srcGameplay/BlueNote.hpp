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
    void getGradingForOfftime(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    void setJudged(NoteGradings grading, int64_t picosecondsOff) override;

    BlueNotePrefab* prefab{nullptr};
};

#endif