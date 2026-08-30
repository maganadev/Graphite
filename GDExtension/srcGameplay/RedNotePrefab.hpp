#ifndef RedNotePrefab_hpp
#define RedNotePrefab_hpp

#include <godot_cpp/classes/sprite2d.hpp>

using namespace ::godot;

class RedNotePrefab : public Sprite2D
{
    GDCLASS(RedNotePrefab, Sprite2D)

protected:
    static void _bind_methods();

public:
    RedNotePrefab();
    ~RedNotePrefab();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;
};

#endif