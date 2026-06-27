#ifndef SceneGameplayManager_hpp
#define SceneGameplayManager_hpp

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
using namespace ::godot;

class SceneGameplayManager : public Sprite2D
{
    GDCLASS(SceneGameplayManager, Sprite2D)

  private:
    AnimationPlayer* AP_DrumBlueLeft = nullptr;
    AnimationPlayer* AP_DrumRedLeft = nullptr;
    AnimationPlayer* AP_DrumRedRight = nullptr;
    AnimationPlayer* AP_DrumBlueRight = nullptr;
    AnimationPlayer* AP_Chou0 = nullptr;
    AnimationPlayer* AP_Chou1 = nullptr;
    AnimationPlayer* AP_Ryou0 = nullptr;
    AnimationPlayer* AP_Ryou1 = nullptr;
    AnimationPlayer* AP_Ka0 = nullptr;
    AnimationPlayer* AP_Ka1 = nullptr;
    AnimationPlayer* AP_Fuka0 = nullptr;
    AnimationPlayer* AP_Fuka1 = nullptr;
    Node2D* DynamicObjectsAttachPoint = nullptr;
    Vector2 reuseableVector2 = Vector2(0, 0);
    MachineTimestamp currentTimestamp = 0;
    Subseconds currentSubseconds = 0;

  protected:
    static void _bind_methods();

  public:
    // Godot object functions
    SceneGameplayManager();
    ~SceneGameplayManager();
    void _ready() override;
    void _ready_GodotObjects();
    void _ready_ChartFile();
    void _ready_Offsets();
    void _ready_LoadMusic();
    void _ready_CreateGameplayObjects();
    void _process(double delta) override;
    void _process_UpdateCurrentSubseconds();
    void _process_UpdateNotePositions();
    void _process_UpdateDrumAnimations();
    void _process_UpdateJumpingTextAnimations();
    void _process_SendRequestToHandleAbandonedNotes();
};

#endif