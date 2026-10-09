#ifndef BigGreenNote_hpp
#define BigGreenNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class BigGreenNotePrefab;

class BigGreenNote : public HittableNote
{
public:
    BigGreenNote();
    ~BigGreenNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds) override;
    void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    void handleStrike(NoteGradings grading, int64_t picosecondsOff) override;
    void constructor2(const nlohmann::json& j) override;
    int64_t getSpamHitsCount() const override;
    bool isSpamNote() const override;

    int64_t endTimePicoseconds{0};
    std::atomic<int64_t> remainingSpamHits{0};

    BigGreenNotePrefab* prefab{nullptr};
};

#endif