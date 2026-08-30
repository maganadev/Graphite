#ifndef GreenNotePrefab_hpp
#define GreenNotePrefab_hpp

#include <godot_cpp/classes/sprite2d.hpp>

using namespace ::godot;

class GreenNotePrefab : public Sprite2D
{
    GDCLASS(GreenNotePrefab, Sprite2D)

protected:
    static void _bind_methods();

public:
    GreenNotePrefab();
    ~GreenNotePrefab();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;
};

#endif