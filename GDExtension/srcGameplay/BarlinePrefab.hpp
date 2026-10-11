#ifndef BarlinePrefab_hpp
#define BarlinePrefab_hpp

#include <godot_cpp/classes/sprite2d.hpp>

using namespace ::godot;

class BarlinePrefab : public Sprite2D
{
    GDCLASS(BarlinePrefab, Sprite2D)

protected:
    static void _bind_methods();

public:
    BarlinePrefab();
    ~BarlinePrefab();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;
};

#endif