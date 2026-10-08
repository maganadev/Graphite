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
    void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    void handleGrading(NoteGradings grading, int64_t picosecondsOff) override;

    BigBlueNotePrefab* prefab{nullptr};
};

#endif