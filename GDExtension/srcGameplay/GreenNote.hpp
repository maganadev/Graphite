#ifndef GreenNote_hpp
#define GreenNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class GreenNotePrefab;

class GreenNote : public HittableNote
{
public:
    GreenNote(const nlohmann::json& j);
    ~GreenNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);
    void getGradingForOfftime(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    void setJudged(NoteGradings grading, int64_t picosecondsOff) override;

    GreenNotePrefab* prefab{nullptr};
};

#endif