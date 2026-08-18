#ifndef GameManager_hpp
#define GameManager_hpp
    
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

#include <optional>
#include <vector>

#include "../../RhythmAudio/RhythmAudio/RhythmAudioEngine.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"

using namespace ::godot;

class GameManager : public Sprite2D
{
    GDCLASS(GameManager, Sprite2D)

protected:
    static void _bind_methods();

public:
    // Godot object functions
    GameManager();
    ~GameManager();
    void _ready() override;
    void _process(double delta) override;

    std::optional<RhythmAudio::RhythmAudioEngine> audioEngine;
    std::optional<RhythmInput::RhythmInputEngine> inputEngine;
};

#endif
