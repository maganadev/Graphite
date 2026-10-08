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

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds) override;
    void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    void handleGrading(NoteGradings grading, int64_t picosecondsOff) override;

    BigGreenNotePrefab* prefab{nullptr};
};

#endif