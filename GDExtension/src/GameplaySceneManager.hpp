#ifndef GameplaySceneManager_hpp
#define GameplaySceneManager_hpp

#include "Course.hpp"
#include "GameManager.hpp"
#include "JudgementThread.hpp"
#include "RhythmEnums.hpp"
#include <fstream>
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

class GameplaySceneManager : public Sprite2D
{
    GDCLASS(GameplaySceneManager, Sprite2D)

protected:
    static void _bind_methods();

public:
    GameplaySceneManager();
    ~GameplaySceneManager();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;

    void set_red_note_scene(Ref<PackedScene> scene);
    Ref<PackedScene> get_red_note_scene() const;
    void set_blue_note_scene(Ref<PackedScene> scene);
    Ref<PackedScene> get_blue_note_scene() const;

private:
    uint64_t audioTrackHandle{0};
    Ref<PackedScene> redNoteScene;
    Ref<PackedScene> blueNoteScene;
    int64_t visualOffsetPicoseconds{0};
    NoteTypes noteTypeForEvent(const std::string& type) const;
};

#endif
