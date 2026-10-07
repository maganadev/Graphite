#ifndef BigRedNotePrefab_hpp
#define BigRedNotePrefab_hpp

#include <godot_cpp/classes/sprite2d.hpp>

using namespace ::godot;

class BigRedNotePrefab : public Sprite2D
{
    GDCLASS(BigRedNotePrefab, Sprite2D)

protected:
    static void _bind_methods();

public:
    BigRedNotePrefab();
    ~BigRedNotePrefab();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;
};

#endif