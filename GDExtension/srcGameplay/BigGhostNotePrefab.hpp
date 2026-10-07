#ifndef BigGhostNotePrefab_hpp
#define BigGhostNotePrefab_hpp

#include <godot_cpp/classes/sprite2d.hpp>

using namespace ::godot;

class BigGhostNotePrefab : public Sprite2D
{
    GDCLASS(BigGhostNotePrefab, Sprite2D)

protected:
    static void _bind_methods();

public:
    BigGhostNotePrefab();
    ~BigGhostNotePrefab();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;
};

#endif