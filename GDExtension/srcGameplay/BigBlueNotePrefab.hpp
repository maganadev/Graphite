#ifndef BigBlueNotePrefab_hpp
#define BigBlueNotePrefab_hpp

#include <godot_cpp/classes/sprite2d.hpp>

using namespace ::godot;

class BigBlueNotePrefab : public Sprite2D
{
    GDCLASS(BigBlueNotePrefab, Sprite2D)

protected:
    static void _bind_methods();

public:
    BigBlueNotePrefab();
    ~BigBlueNotePrefab();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;
};

#endif