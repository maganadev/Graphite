#ifndef BigGhostNote_hpp
#define BigGhostNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class BigGhostNotePrefab;

class BigGhostNote : public HittableNote
{
public:
    BigGhostNote();
    ~BigGhostNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds) override;
    void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    bool handleStrikeAndGetCompleted(NoteGradings grading, int64_t picosecondsOff) override;
    void constructor2(const nlohmann::json& j) override;
    int64_t getSpamHitsCount() const override;

    int64_t picosecondsOff{0};

    BigGhostNotePrefab* prefab{nullptr};
};

#endif