#ifndef YellowNotePrefab_hpp
#define YellowNotePrefab_hpp

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

#include "YellowNote.hpp"

using namespace ::godot;

class YellowNotePrefab : public Node2D
{
    GDCLASS(YellowNotePrefab, Node2D)

protected:
    static void _bind_methods();

public:
    YellowNotePrefab();
    ~YellowNotePrefab();

    void setNote(const Note& note);
    const Note& getNote() const;

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

private:
    YellowNote m_note;
};

#endif
