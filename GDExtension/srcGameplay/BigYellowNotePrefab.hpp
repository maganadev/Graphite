#ifndef BigYellowNotePrefab_hpp
#define BigYellowNotePrefab_hpp

#include <godot_cpp/classes/sprite2d.hpp>

using namespace ::godot;

class BigYellowNotePrefab : public Sprite2D
{
    GDCLASS(BigYellowNotePrefab, Sprite2D)

protected:
    static void _bind_methods();

public:
    BigYellowNotePrefab();
    ~BigYellowNotePrefab();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;
};

#endif