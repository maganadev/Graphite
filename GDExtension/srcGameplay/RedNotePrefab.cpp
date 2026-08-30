#include "RedNotePrefab.hpp"

void RedNotePrefab::_bind_methods()
{
    //
}

RedNotePrefab::RedNotePrefab()
{
    //
}

RedNotePrefab::~RedNotePrefab()
{
    //
}

void RedNotePrefab::_ready()
{
    //
}

void RedNotePrefab::_exit_tree()
{
    //
}

void RedNotePrefab::_process(double delta)
{
    //
}

void RedNotePrefab::updatePosition(int64_t offsetAdjustedTimePs)
{
    double x = 0.0;
    double y = 0.0;
    getRenderPosition(offsetAdjustedTimePs, x, y);
    set_position(Vector2(x, y));
}

void RedNotePrefab::getRenderPosition(int64_t offsetAdjustedTimePs, double& outX, double& outY) const
{
    const int64_t timeUntilNote = m_note.getNote().time_picoseconds - offsetAdjustedTimePs;
    outX = HITZONE_CENTER_X + SCROLL_SPEED * (static_cast<double>(timeUntilNote) / 1.0e12);
    outY = LANE_Y;
}
