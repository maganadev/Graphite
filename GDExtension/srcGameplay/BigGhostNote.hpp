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

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds) override;
    void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    void handleGrading(NoteGradings grading, int64_t picosecondsOff) override;

    BigGhostNotePrefab* prefab{nullptr};
};

#endif