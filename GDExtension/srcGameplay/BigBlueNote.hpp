#ifndef BigBlueNote_hpp
#define BigBlueNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class BigBlueNotePrefab;

class BigBlueNote : public HittableNote
{
public:
    BigBlueNote();
    ~BigBlueNote();

    void updatePosition(int64_t correctedSongPositionPs) override;
    void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    bool handleStrikeAndGetCompleted(NoteGradings grading, int64_t picosecondsOff) override;
    int64_t getSpamHitsCount() const override;

    int64_t picosecondsOff{0};

    BigBlueNotePrefab* prefab{nullptr};
};

#endif