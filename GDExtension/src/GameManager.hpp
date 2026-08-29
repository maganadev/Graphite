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

#include "../../RhythmAudio/RhythmAudio/QueueSPSC.hpp"
#include "../../RhythmAudio/RhythmAudio/RhythmAudioEngine.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"
#include "../UtilsCode/json.hpp"
#include <atomic>
#include <semaphore>
#include <string>
#include <thread>
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

struct InputTimingMessage
{
    uint64_t timestamp;
    uint64_t audioTrackHandle;
};

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
    void _exit_tree() override;
    void _process(double delta) override;

    static RhythmAudio::RhythmAudioEngine* audioEngine;
    static RhythmInput::RhythmInputEngine* inputEngine;
    static std::string songName;
    static std::string courseDifficulty;
    static json songJson;
    static uint64_t blueRyouHitsoundHandle;
    static uint64_t blueKaHitsoundHandle;
    static uint64_t blueFukaHitsoundHandle;
    static uint64_t blueChouHitsoundHandle;
    static uint64_t blueAdLibHitsoundHandle;
    static uint64_t redRyouHitsoundHandle;
    static uint64_t redKaHitsoundHandle;
    static uint64_t redFukaHitsoundHandle;
    static uint64_t redChouHitsoundHandle;
    static uint64_t redAdLibHitsoundHandle;

    static QueueSPSC<InputTimingMessage, 1024> inputTimingMessageQueue;
    static std::counting_semaphore<1> inputTimingSemaphore;
    static std::thread inputTimingThread;
    static std::atomic<bool> requestInputTimingThreadShutdown;
    static void inputTimingThreadEntry();

private:
    bool processFunctionRan = false;
    uint64_t frameCount = 0;
    void initializeInputEngine();
};

#endif
