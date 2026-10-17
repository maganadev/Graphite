#ifndef RedNote_hpp
#define RedNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class RedNotePrefab;

class RedNote : public HittableNote
{
public:
    RedNote();
    ~RedNote();

    void updatePosition(int64_t correctedSongPositionPs, double userScrollModifier) override;
    void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    bool handleStrikeAndGetCompleted(NoteGradings grading, int64_t picosecondsOff) override;
    int64_t getSpamHitsCount() const override;

    int64_t picosecondsOff{0};

    RedNotePrefab* prefab{nullptr};
};

#endif