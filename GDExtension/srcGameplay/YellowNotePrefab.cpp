#include "YellowNotePrefab.hpp"

void YellowNotePrefab::_bind_methods()
{
    //
}

YellowNotePrefab::YellowNotePrefab()
{
    //
}

YellowNotePrefab::~YellowNotePrefab()
{
    //
}

void YellowNotePrefab::_ready()
{
    midBlack = get_node<Control>("MidBlack");
    midYellow = get_node<Control>("MidYellow");
    rightBlack = get_node<Control>("RightBlack");
    rightYellow = get_node<Control>("RightYellow");
}

void YellowNotePrefab::_exit_tree()
{
    //
}

void YellowNotePrefab::_process(double delta)
{
    //
}

void YellowNotePrefab::elongateTo(double tailX)
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