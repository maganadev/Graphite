#ifndef RedNotePrefab_hpp
#define RedNotePrefab_hpp

#include "RedNote.hpp"
#include <cstdint>
#include <godot_cpp/classes/sprite2d.hpp>

using namespace ::godot;

class RedNotePrefab : public Sprite2D
{
    GDCLASS(RedNotePrefab, Sprite2D)

protected:
    static void _bind_methods();

public:
    static constexpr double SCROLL_SPEED = 1600.0;
    static constexpr double LANE_Y = 386.0;
    static constexpr double HITZONE_CENTER_X = 618.0;

    RedNotePrefab();
    ~RedNotePrefab();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;
    void updatePosition(int64_t offsetAdjustedTimePs);
    void getRenderPosition(int64_t offsetAdjustedTimePs, double& outX, double& outY) const;
};

#endif
