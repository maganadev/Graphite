#ifndef GhostNote_hpp
#define GhostNote_hpp

#include "../srcThirdParty/json.hpp"
#include <cstdint>

#include "RhythmEnums.hpp"

class GhostNotePrefab;

class GhostNote
{
public:
    static constexpr double SCROLL_SPEED_FACTOR = 2000.0 / (240.0 * 1.0e12);
    static constexpr double LANE_Y = 386.0;
    static constexpr double HITZONE_CENTER_X = 618.0;

    GhostNote(const nlohmann::json& j);
    ~GhostNote();

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);
    void getRenderPosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds, double& outX, double& outY) const;

    int64_t timePicoseconds;
    double bpmForScrollDouble;
    GhostNotePrefab* prefab{nullptr};
};

#endif
