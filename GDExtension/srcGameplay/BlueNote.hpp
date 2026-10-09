#ifndef BlueNote_hpp
#define BlueNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class BlueNotePrefab;

class BlueNote : public HittableNote
{
public:
    BlueNote();
    ~BlueNote();

    void updatePosition(int64_t correctedSongPositionPs) override;
    void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    bool handleStrikeAndGetCompleted(NoteGradings grading, int64_t picosecondsOff) override;
    void constructor2(const nlohmann::json& j) override;
    int64_t getSpamHitsCount() const override;

    int64_t picosecondsOff{0};

    BlueNotePrefab* prefab{nullptr};
};

#endif