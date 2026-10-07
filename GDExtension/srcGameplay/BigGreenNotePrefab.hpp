#ifndef BigGreenNotePrefab_hpp
#define BigGreenNotePrefab_hpp

#include <godot_cpp/classes/sprite2d.hpp>

using namespace ::godot;

class BigGreenNotePrefab : public Sprite2D
{
    GDCLASS(BigGreenNotePrefab, Sprite2D)

protected:
    static void _bind_methods();

public:
    BigGreenNotePrefab();
    ~BigGreenNotePrefab();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;
};

#endif