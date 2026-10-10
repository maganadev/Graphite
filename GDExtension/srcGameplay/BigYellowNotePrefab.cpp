#include "BigYellowNotePrefab.hpp"

void BigYellowNotePrefab::_bind_methods()
{
}

BigYellowNotePrefab::BigYellowNotePrefab()
{
}

BigYellowNotePrefab::~BigYellowNotePrefab()
{
}

void BigYellowNotePrefab::_ready()
{
    midBlack = get_node<Control>("MidBlack");
    midYellow = get_node<Control>("MidYellow");
    rightBlack = get_node<Control>("RightBlack");
    rightYellow = get_node<Control>("RightYellow");
}

void BigYellowNotePrefab::_exit_tree()
{
}

void BigYellowNotePrefab::_process(double delta)
{
}

void BigYellowNotePrefab::elongateTo(double tailX)
{
    float tail = static_cast<float>(tailX);

    if (midBlack)
    {
        midBlack->set_offset(SIDE_RIGHT, tail);
    }
    if (midYellow)
    {
        midYellow->set_offset(SIDE_RIGHT, tail);
    }

    if (rightBlack)
    {
        float center = (rightBlack->get_offset(SIDE_LEFT) + rightBlack->get_offset(SIDE_RIGHT)) * 0.5f;
        float delta = tail - center;
        rightBlack->set_offset(SIDE_LEFT, rightBlack->get_offset(SIDE_LEFT) + delta);
        rightBlack->set_offset(SIDE_RIGHT, rightBlack->get_offset(SIDE_RIGHT) + delta);
    }
    if (rightYellow)
    {
        float center = (rightYellow->get_offset(SIDE_LEFT) + rightYellow->get_offset(SIDE_RIGHT)) * 0.5f;
        float delta = tail - center;
        rightYellow->set_offset(SIDE_LEFT, rightYellow->get_offset(SIDE_LEFT) + delta);
        rightYellow->set_offset(SIDE_RIGHT, rightYellow->get_offset(SIDE_RIGHT) + delta);
    }
}