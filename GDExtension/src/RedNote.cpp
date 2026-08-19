#include "RedNote.hpp"

void RedNote::_bind_methods()
{
    //
}

RedNote::RedNote()
{
    //
}

RedNote::~RedNote()
{
    //
}

void RedNote::setEvent(const TJAEvent& event)
{
    m_event = event;
}

const TJAEvent& RedNote::getEvent() const
{
    return m_event;
}