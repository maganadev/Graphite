#ifndef GameplaySceneManager_hpp
#define GameplaySceneManager_hpp
    
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

#include <fstream>

#include "Globals.hpp"

using namespace ::godot;
using namespace ::std;

class GameplaySceneManager : public Sprite2D
{
    GDCLASS(GameplaySceneManager, Sprite2D)

protected:
    static void _bind_methods();

public:
    GameplaySceneManager();
    ~GameplaySceneManager();
    void _ready() override;
    void _process(double delta) override;

private:
    uint64_t audioTrackHandle{0};
};

#endif
