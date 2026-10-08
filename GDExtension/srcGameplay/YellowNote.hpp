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

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds) override;
    void getWhatGradingWouldBe(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    void handleGrading(NoteGradings grading, int64_t picosecondsOff) override;
    void constructor2(const nlohmann::json& j) override;

    int64_t endTimePicoseconds{0};
    int64_t spamHits{0};

    YellowNotePrefab* prefab{nullptr};
};

#endif