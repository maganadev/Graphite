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
    void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    void handleGrading(NoteGradings grading, int64_t picosecondsOff) override;

    BigRedNotePrefab* prefab{nullptr};
};

#endif