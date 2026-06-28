#ifndef GDExtTestObj_hpp
#define GDExtTestObj_hpp

#include <Windows.h>
#define DO_NOT_REORDER_ABOVE
#include "AsyncInputSystem/AsyncInputSystem.hpp"
#include "AsyncInputSystem/KeybindedAction.hpp"
#include "AudioSystem/AudioSystem.hpp"
#include "TanoshiiTJAGameplayScene/ChartData.hpp"
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include "RhythmAudio.hpp"
#include "RhythmInput.hpp"
using namespace ::godot;

class GDExtTestObj : public Sprite2D
{
    GDCLASS(GDExtTestObj, Sprite2D)

protected:
    static void _bind_methods();

private:
    static std::optional<RhythmAudio> audioEngine;
    static std::optional<RhythmInput> inputEngine;

public:
    // Godot object functions
    GDExtTestObj();
    ~GDExtTestObj();
    void _ready() override;
    void _process(double delta) override;
};

#endif
