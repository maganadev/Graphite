#ifndef GhostNotePrefab_hpp
#define GhostNotePrefab_hpp

#include <godot_cpp/classes/sprite2d.hpp>

using namespace ::godot;

class GhostNotePrefab : public Sprite2D
{
    GDCLASS(GhostNotePrefab, Sprite2D)

protected:
    static void _bind_methods();

public:
    GhostNotePrefab();
    ~GhostNotePrefab();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;
};

#endif