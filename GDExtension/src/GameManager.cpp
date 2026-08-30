#include "GameManager.hpp"
#include "../../RhythmAudio/RhythmAudio/RhythmAudioEngine.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"
#include "InputThreadFunctions.hpp"
#include "JudgementThread.hpp"
#include "SettingsFile.hpp"
#include "Course.hpp"

std::optional<RhythmAudio::RhythmAudioEngine> GameManager::audioEngine{std::nullopt};
std::optional<RhythmInput::RhythmInputEngine> GameManager::inputEngine{std::nullopt};
std::string GameManager::songName;
std::string GameManager::courseDifficulty;
json GameManager::songJson;
uint64_t GameManager::blueRyouHitsoundHandle{0};
uint64_t GameManager::blueKaHitsoundHandle{0};
uint64_t GameManager::blueFukaHitsoundHandle{0};
uint64_t GameManager::blueChouHitsoundHandle{0};
uint64_t GameManager::blueAdLibHitsoundHandle{0};
uint64_t GameManager::redRyouHitsoundHandle{0};
uint64_t GameManager::redKaHitsoundHandle{0};
uint64_t GameManager::redFukaHitsoundHandle{0};
uint64_t GameManager::redChouHitsoundHandle{0};
uint64_t GameManager::redAdLibHitsoundHandle{0};
Course* GameManager::currentCourse{nullptr};

void GameManager::_bind_methods()
{
}

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

void GameManager::_ready()
{
    // RhythmAudio::RhythmAudioSettings settings{};
    // SettingsFile audioSettingsFile("audio_settings.json");
    // audioSettingsFile.load();
    // audioSettingsFile.ensureContainsString("backendMode", "");
    // audioSettingsFile.ensureContainsString("audioMonoStereo", "stereo");
    // audioSettingsFile.ensureContainsInteger("ASIO_sampleRate", 0);
    // audioSettingsFile.ensureContainsInteger("ASIO_bufferSizeInSamples", 0);
    // audioSettingsFile.ensureContainsString("ASIO_cardToUse", "");
    // audioSettingsFile.ensureContainsInteger("ASIO_leftChannel", 0);
    // audioSettingsFile.ensureContainsInteger("ASIO_rightChannel", 1);
    // audioSettingsFile.save();
    // std::string backendMode = audioSettingsFile.jsonObj.value("backendMode", "");
    // if (backendMode == "ASIO")
    // {
    //     settings.backendMode = RhythmAudio::AudioBackendMode::ASIO;
    // }
    // settings.audioMonoStereo = audioSettingsFile.jsonObj.value("audioMonoStereo", "stereo");
    // settings.ASIO_sampleRate = audioSettingsFile.jsonObj.value("ASIO_sampleRate", 0);
    // settings.ASIO_bufferSizeInSamples = audioSettingsFile.jsonObj.value("ASIO_bufferSizeInSamples", 0);
    // settings.ASIO_cardToUse = audioSettingsFile.jsonObj.value("ASIO_cardToUse", "");
    // settings.ASIO_leftChannel = audioSettingsFile.jsonObj.value("ASIO_leftChannel", 0);
    // settings.ASIO_rightChannel = audioSettingsFile.jsonObj.value("ASIO_rightChannel", 1);
    // GameManager::audioEngine.emplace(settings);
    // GameManager::audioEngine.value().createAudioTrackBlocking("GameplayBlueRyouHitsound.ogg", -36, GameManager::blueRyouHitsoundHandle);
    // GameManager::audioEngine.value().createAudioTrackBlocking("GameplayBlueKaHitsound.ogg", -36, GameManager::blueKaHitsoundHandle);
    // GameManager::audioEngine.value().createAudioTrackBlocking("GameplayBlueFukaHitsound.ogg", -36, GameManager::blueFukaHitsoundHandle);
    // GameManager::audioEngine.value().createAudioTrackBlocking("GameplayBlueChouHitsound.ogg", -36, GameManager::blueChouHitsoundHandle);
    // GameManager::audioEngine.value().createAudioTrackBlocking("GameplayBlueAdLibHitsound.ogg", -36, GameManager::blueAdLibHitsoundHandle);
    // GameManager::audioEngine.value().createAudioTrackBlocking("GameplayRedRyouHitsound.ogg", -36, GameManager::redRyouHitsoundHandle);
    // GameManager::audioEngine.value().createAudioTrackBlocking("GameplayRedKaHitsound.ogg", -36, GameManager::redKaHitsoundHandle);
    // GameManager::audioEngine.value().createAudioTrackBlocking("GameplayRedFukaHitsound.ogg", -36, GameManager::redFukaHitsoundHandle);
    // GameManager::audioEngine.value().createAudioTrackBlocking("GameplayRedChouHitsound.ogg", -36, GameManager::redChouHitsoundHandle);
    // GameManager::audioEngine.value().createAudioTrackBlocking("GameplayRedAdLibHitsound.ogg", -36, GameManager::redAdLibHitsoundHandle);

    // UtilityFunctions::print("GameManager::blueRyouHitsoundHandle: ", std::to_string(GameManager::blueRyouHitsoundHandle).c_str());

    // JudgementThread::start();

    // RhythmAudio::RhythmAudioStats stats{};
    // GameManager::audioEngine.value().getEngineStats(stats);
    // UtilityFunctions::print("--- Engine Stats ---");
    // UtilityFunctions::print("EngineState: ", std::to_string(stats.engineState).c_str());
    // UtilityFunctions::print("SampleRate: ", std::to_string(stats.sampleRate).c_str());
    // UtilityFunctions::print("BufferSize: ", std::to_string(stats.bufferSizeInSamples).c_str());
    // UtilityFunctions::print("Channels: ", std::to_string(stats.channels).c_str());
    // UtilityFunctions::print("BackendMode: ", std::to_string(static_cast<int>(stats.backendMode)).c_str());
    // UtilityFunctions::print("GlobalSampleCount: ", std::to_string(stats.globalSampleCount).c_str());
    // UtilityFunctions::print("TimingStdDev: ", std::to_string(stats.timingStdDev).c_str());
    // UtilityFunctions::print("SuggestedOutputLatency: ", std::to_string(stats.suggestedOutputLatency).c_str());
    // UtilityFunctions::print("ActualOutputLatency: ", std::to_string(stats.actualOutputLatency).c_str());
}

void GameManager::_exit_tree()
{
    // if (!GameManager::audioEngine.has_value())
    // {
    //     JudgementThread::stop();
    //     return;
    // }
    // auto freeTrack = [](uint64_t& handle)
    // {
    //     if (handle != 0)
    //     {
    //         GameManager::audioEngine.value().freeAudioTrackBlocking(handle);
    //         handle = 0;
    //     }
    // };
    // freeTrack(GameManager::blueRyouHitsoundHandle);
    // freeTrack(GameManager::blueKaHitsoundHandle);
    // freeTrack(GameManager::blueFukaHitsoundHandle);
    // freeTrack(GameManager::blueChouHitsoundHandle);
    // freeTrack(GameManager::blueAdLibHitsoundHandle);
    // freeTrack(GameManager::redRyouHitsoundHandle);
    // freeTrack(GameManager::redKaHitsoundHandle);
    // freeTrack(GameManager::redFukaHitsoundHandle);
    // freeTrack(GameManager::redChouHitsoundHandle);
    // freeTrack(GameManager::redAdLibHitsoundHandle);

    // JudgementThread::stop();
}

void GameManager::_process(double delta)
{
    if (!processFunctionRan)
    {
        initializeInputEngine();
        processFunctionRan = true;
    }

    GameManager::inputEngine.value().parseEventsSinceLastFrame();
}

void GameManager::initializeInputEngine()
{
    std::vector<RhythmInput::RhythmInputAction> gameActions;
    std::vector<RhythmInput::RhythmInputBinding> gameBindings;

    RhythmInput::RhythmInputAction DrumRimLeftKeybind{};
    DrumRimLeftKeybind.name = "DrumRimLeft";
    DrumRimLeftKeybind.callbackOnPress = InputThreadFunctions::DrumRimLeft;
    DrumRimLeftKeybind.callbackOnRelease = nullptr;
    gameActions.push_back(DrumRimLeftKeybind);

    RhythmInput::RhythmInputAction DrumRimRightKeybind{};
    DrumRimRightKeybind.name = "DrumRimRight";
    DrumRimRightKeybind.callbackOnPress = InputThreadFunctions::DrumRimRight;
    DrumRimRightKeybind.callbackOnRelease = nullptr;
    gameActions.push_back(DrumRimRightKeybind);

    RhythmInput::RhythmInputAction DrumCenterLeftKeybind{};
    DrumCenterLeftKeybind.name = "DrumCenterLeft";
    DrumCenterLeftKeybind.callbackOnPress = InputThreadFunctions::DrumCenterLeft;
    DrumCenterLeftKeybind.callbackOnRelease = nullptr;
    gameActions.push_back(DrumCenterLeftKeybind);

    RhythmInput::RhythmInputAction DrumCenterRightKeybind{};
    DrumCenterRightKeybind.name = "DrumCenterRight";
    DrumCenterRightKeybind.callbackOnPress = InputThreadFunctions::DrumCenterRight;
    DrumCenterRightKeybind.callbackOnRelease = nullptr;
    gameActions.push_back(DrumCenterRightKeybind);

    RhythmInput::RhythmInputAction EnterKeybind{};
    EnterKeybind.name = "Enter";
    EnterKeybind.callbackOnPress = nullptr;
    EnterKeybind.callbackOnRelease = nullptr;
    gameActions.push_back(EnterKeybind);

    RhythmInput::RhythmInputAction BackKeybind{};
    BackKeybind.name = "Back";
    BackKeybind.callbackOnPress = nullptr;
    BackKeybind.callbackOnRelease = nullptr;
    gameActions.push_back(BackKeybind);

    SettingsFile inputSettingsFile("input_settings.json");
    inputSettingsFile.load();

    if (!inputSettingsFile.jsonObj.contains("bindings"))
    {
        json defaultBindings = json::array();
        defaultBindings.push_back({{"button", "z"}, {"action", "DrumRimLeft"}});
        defaultBindings.push_back({{"button", "x"}, {"action", "DrumCenterLeft"}});
        defaultBindings.push_back({{"button", "n"}, {"action", "DrumCenterRight"}});
        defaultBindings.push_back({{"button", "m"}, {"action", "DrumRimRight"}});
        defaultBindings.push_back({{"button", "enter"}, {"action", "Enter"}});
        defaultBindings.push_back({{"button", "escape"}, {"action", "Back"}});
        inputSettingsFile.jsonObj["bindings"] = defaultBindings;
    }

    inputSettingsFile.save();

    for (auto& bindingJson : inputSettingsFile.jsonObj["bindings"])
    {
        RhythmInput::RhythmInputBinding binding{};
        binding.button = bindingJson.value("button", "");
        binding.action = bindingJson.value("action", "");
        gameBindings.push_back(binding);
    }

    GameManager::inputEngine.emplace(gameActions, gameBindings);
}