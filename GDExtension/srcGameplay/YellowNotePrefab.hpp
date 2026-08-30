#ifndef YellowNotePrefab_hpp
#define YellowNotePrefab_hpp

#include <godot_cpp/classes/sprite2d.hpp>

using namespace ::godot;

class YellowNotePrefab : public Sprite2D
{
    GDCLASS(YellowNotePrefab, Sprite2D)

protected:
    static void _bind_methods();

public:
    YellowNotePrefab();
    ~YellowNotePrefab();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;
};

#endif