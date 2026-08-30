#ifndef RedNotePrefab_hpp
#define RedNotePrefab_hpp

#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

using namespace ::godot;
using namespace ::std;

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
