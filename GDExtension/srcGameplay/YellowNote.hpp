#ifndef YellowNote_hpp
#define YellowNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class YellowNotePrefab;

class YellowNote : public HittableNote
{
public:
    YellowNote(const nlohmann::json& j);
    ~YellowNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);
    void getGradingForOfftime(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    void setJudged(NoteGradings grading, int64_t picosecondsOff) override;

    YellowNotePrefab* prefab{nullptr};
};

#endif