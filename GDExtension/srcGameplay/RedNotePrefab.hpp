#ifndef RedNotePrefab_hpp
#define RedNotePrefab_hpp

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

#include "RedNote.hpp"

using namespace ::godot;

class RedNotePrefab : public Node2D
{
    GDCLASS(RedNotePrefab, Node2D)

protected:
    static void _bind_methods();

public:
    RedNotePrefab();
    ~RedNotePrefab();

    void setNote(const Note& note);
    const Note& getNote() const;

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

private:
    RedNote m_note;
};

#endif
