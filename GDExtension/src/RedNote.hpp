#ifndef RedNote_hpp
#define RedNote_hpp

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/ref.hpp>

#include "TJACourse.hpp"

using namespace ::godot;

class RedNote : public Node2D
{
    GDCLASS(RedNote, Node2D)

protected:
    static void _bind_methods();

public:
    RedNote();
    ~RedNote();

    void setEvent(const TJAEvent& event);
    const TJAEvent& getEvent() const;

private:
    TJAEvent m_event;
};

#endif