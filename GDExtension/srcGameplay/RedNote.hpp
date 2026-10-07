#ifndef RedNote_hpp
#define RedNote_hpp

#include "../srcThirdParty/json.hpp"

#include "HittableNote.hpp"

class RedNotePrefab;

class RedNote : public HittableNote
{
public:
    RedNote(const nlohmann::json& j);
    ~RedNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);
    void getGradingForOfftime(int64_t songPositionPs, const Chart* chart, NoteGradings& grading, int64_t& offtime) override;
    void setJudged(NoteGradings grading, int64_t picosecondsOff) override;

    RedNotePrefab* prefab{nullptr};
};

#endif