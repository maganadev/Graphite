#ifndef BigRedNote_hpp
#define BigRedNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class BigRedNotePrefab;

class BigRedNote : public HittableNote
{
public:
    BigRedNote();
    ~BigRedNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds) override;
    void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    void handleStrike(NoteGradings grading, int64_t picosecondsOff) override;
    void constructor2(const nlohmann::json& j) override;
    int64_t getSpamHitsCount() const override;

    int64_t picosecondsOff{0};

    BigRedNotePrefab* prefab{nullptr};
};

#endif