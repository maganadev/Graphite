#ifndef YellowNote_hpp
#define YellowNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class YellowNotePrefab;

class YellowNote : public HittableNote
{
public:
    YellowNote();
    ~YellowNote();

    void updatePosition(int64_t correctedSongPositionPs) override;
    void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    bool handleStrikeAndGetCompleted(NoteGradings grading, int64_t picosecondsOff) override;
    void constructor2(const nlohmann::json& j) override;
    int64_t getSpamHitsCount() const override;
    bool isSpamNote() const override;

    int64_t endTimePicoseconds{0};
    std::atomic<int64_t> spamHits{0};

    YellowNotePrefab* prefab{nullptr};
};

#endif