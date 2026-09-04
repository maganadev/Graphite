#ifndef GameManager_hpp
#define GameManager_hpp

#include "GraphiteGlobals.hpp"
#include "../../RhythmAudio/RhythmAudio/LFProtectObj.hpp"
#include "../../RhythmAudio/RhythmAudio/RhythmAudioEngine.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"
#include "../srcGameplay/Chart.hpp"
#include "../srcThirdParty/json.hpp"
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;
using namespace ::godot;

namespace GameActionIndices
{
    constexpr size_t DrumRimLeft = 0;
    constexpr size_t DrumRimRight = 1;
    constexpr size_t DrumCenterLeft = 2;
    constexpr size_t DrumCenterRight = 3;
    constexpr size_t Enter = 4;
    constexpr size_t Back = 5;
} // namespace GameActionIndices

class GameManager : public Sprite2D
{
    GDCLASS(GameManager, Sprite2D)

protected:
    static void _bind_methods();

public:
    GameManager();
    ~GameManager();
    void _ready() override;
    void _exit_tree() override;
    void _process(double delta) override;

    private:
    bool processFunctionRan = false;
    void initializeInputEngine();
};

#endif