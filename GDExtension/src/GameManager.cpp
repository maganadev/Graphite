#include "GameManager.hpp"
#include "../../RhythmAudio/RhythmAudio/RhythmAudioEngine.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"
#include "Course.hpp"
#include "InputThreadFunctions.hpp"
#include "JudgementThread.hpp"
#include "SettingsFile.hpp"
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/engine.hpp>

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
    // Load and save RhythmAudioSettings
    RhythmAudio::RhythmAudioSettings settings{};
    SettingsFile audioSettingsFile("audio_settings.json");
    audioSettingsFile.load();
    settings.audioMonoStereo = audioSettingsFile.get<std::string>("audioMonoStereo", "stereo");
    settings.ASIO_sampleRate = audioSettingsFile.get<uint32_t>("ASIO_sampleRate", 48000);
    settings.ASIO_bufferSizeInSamples = audioSettingsFile.get<uint32_t>("ASIO_bufferSizeInSamples", 1024);
    settings.ASIO_cardToUse = audioSettingsFile.get<std::string>("ASIO_cardToUse", "");
    settings.ASIO_leftChannel = audioSettingsFile.get<int32_t>("ASIO_leftChannel", 0);
    settings.ASIO_rightChannel = audioSettingsFile.get<int32_t>("ASIO_rightChannel", 1);

    // Populate C++ RhythmAudioSettings from JSON
    std::string backendMode = audioSettingsFile.get<std::string>("backendMode", "");
    if (backendMode == "ASIO")
    {
        settings.backendMode = RhythmAudio::AudioBackendMode::ASIO;
    }

    // Record the ASIO cards available on this machine so the settings file
    // lists the valid values for ASIO_cardToUse.
    std::vector<std::string> availableAsioCards = RhythmAudio::getAvailableAudioCardNames(RhythmAudio::AudioBackendMode::ASIO);
    audioSettingsFile.set("ASIO_availableCards", availableAsioCards);

    audioSettingsFile.save();
    GraphiteGlobals::audioEngine.emplace(settings);

    SettingsFile mainSettingsFile("main_settings.json");
    mainSettingsFile.load();
    GraphiteGlobals::audioOffset = mainSettingsFile.get<int64_t>("AudioOffset", 0);
    GraphiteGlobals::visualOffset = mainSettingsFile.get<int64_t>("VisualOffset", 0);
    GraphiteGlobals::fullscreen = mainSettingsFile.get<bool>("Fullscreen", false);
    GraphiteGlobals::playbackRate = mainSettingsFile.get<double>("PlaybackRate", 1.0);
    GraphiteGlobals::userScrollModifier = mainSettingsFile.get<double>("ScrollSpeed", 1.0);
    GraphiteGlobals::fpsCap = mainSettingsFile.get<int32_t>("FpsCap", 240);
    GraphiteGlobals::fpsCapEnabled = mainSettingsFile.get<bool>("FpsCapEnabled", false);
    GraphiteGlobals::vsync = mainSettingsFile.get<bool>("Vsync", true);
    GraphiteGlobals::songFolder = mainSettingsFile.get<std::string>("SongFolder", "Songs");
    mainSettingsFile.save();

    DisplayServer* displayServer = DisplayServer::get_singleton();
    if (displayServer)
    {
        displayServer->window_set_mode(GraphiteGlobals::fullscreen ? DisplayServer::WINDOW_MODE_FULLSCREEN : DisplayServer::WINDOW_MODE_WINDOWED);
        displayServer->window_set_vsync_mode(GraphiteGlobals::vsync ? DisplayServer::VSYNC_ENABLED : DisplayServer::VSYNC_DISABLED);
    }

    Engine* engine = Engine::get_singleton();
    if (engine)
    {
        engine->set_max_fps(GraphiteGlobals::fpsCapEnabled ? GraphiteGlobals::fpsCap : 0);
    }

    auto loadHitsound = [](const char* filePath, uint64_t& handle)
    {
        RhythmAudio::RhythmAudioResult result = GraphiteGlobals::audioEngine.value().createAudioTrack(filePath, -36, handle);
        GraphiteGlobals::logAudioResult(result, filePath);
    };
    loadHitsound("OutsideAssets/GameplayBlueRyouHitsound.ogg", GraphiteGlobals::blueRyouHitsoundHandle);
    loadHitsound("OutsideAssets/GameplayBlueKaHitsound.ogg", GraphiteGlobals::blueKaHitsoundHandle);
    loadHitsound("OutsideAssets/GameplayBlueFukaHitsound.ogg", GraphiteGlobals::blueFukaHitsoundHandle);
    loadHitsound("OutsideAssets/GameplayBlueChouHitsound.ogg", GraphiteGlobals::blueChouHitsoundHandle);
    loadHitsound("OutsideAssets/GameplayBlueAdLibHitsound.ogg", GraphiteGlobals::blueAdLibHitsoundHandle);
    loadHitsound("OutsideAssets/GameplayRedRyouHitsound.ogg", GraphiteGlobals::redRyouHitsoundHandle);
    loadHitsound("OutsideAssets/GameplayRedKaHitsound.ogg", GraphiteGlobals::redKaHitsoundHandle);
    loadHitsound("OutsideAssets/GameplayRedFukaHitsound.ogg", GraphiteGlobals::redFukaHitsoundHandle);
    loadHitsound("OutsideAssets/GameplayRedChouHitsound.ogg", GraphiteGlobals::redChouHitsoundHandle);
    loadHitsound("OutsideAssets/GameplayRedAdLibHitsound.ogg", GraphiteGlobals::redAdLibHitsoundHandle);
    loadHitsound("OutsideAssets/GameplayGreenNoteComplete.ogg", GraphiteGlobals::greenNoteCompleteHitsoundHandle);

    UtilityFunctions::print("GraphiteGlobals::blueRyouHitsoundHandle: ", std::to_string(GraphiteGlobals::blueRyouHitsoundHandle).c_str());

    JudgementThread::start();

    RhythmAudio::RhythmAudioStats stats{};
    RhythmAudio::RhythmAudioResult statsResult = GraphiteGlobals::audioEngine.value().getEngineStats(stats);
    GraphiteGlobals::logAudioResult(statsResult, "getEngineStats");
    GraphiteGlobals::logAudioResult(stats.lastError, "audioEngine initialization");
    UtilityFunctions::print("--- Engine Stats ---");
    UtilityFunctions::print("EngineState: ", std::to_string(stats.engineState).c_str());
    UtilityFunctions::print("SampleRate: ", std::to_string(stats.sampleRate).c_str());
    UtilityFunctions::print("BufferSize: ", std::to_string(stats.bufferSizeInSamples).c_str());
    UtilityFunctions::print("Channels: ", std::to_string(stats.channels).c_str());
    UtilityFunctions::print("BackendMode: ", std::to_string(static_cast<int>(stats.backendMode)).c_str());
    UtilityFunctions::print("GlobalSampleCount: ", std::to_string(stats.globalSampleCount).c_str());
    UtilityFunctions::print("TimingStdDev: ", std::to_string(stats.timingStdDev).c_str());
    UtilityFunctions::print("SuggestedOutputLatency: ", std::to_string(stats.suggestedOutputLatency).c_str());
    UtilityFunctions::print("ActualOutputLatency: ", std::to_string(stats.actualOutputLatency).c_str());
}

void GameManager::_exit_tree()
{
    if (!GraphiteGlobals::audioEngine.has_value())
    {
        JudgementThread::stop();
        return;
    }
    auto freeTrack = [](uint64_t& handle)
    {
        if (handle != 0)
        {
            RhythmAudio::RhythmAudioResult result = GraphiteGlobals::audioEngine.value().freeAudioTrack(handle);
            GraphiteGlobals::logAudioResult(result, "freeAudioTrack");
            handle = 0;
        }
    };
    freeTrack(GraphiteGlobals::blueRyouHitsoundHandle);
    freeTrack(GraphiteGlobals::blueKaHitsoundHandle);
    freeTrack(GraphiteGlobals::blueFukaHitsoundHandle);
    freeTrack(GraphiteGlobals::blueChouHitsoundHandle);
    freeTrack(GraphiteGlobals::blueAdLibHitsoundHandle);
    freeTrack(GraphiteGlobals::redRyouHitsoundHandle);
    freeTrack(GraphiteGlobals::redKaHitsoundHandle);
    freeTrack(GraphiteGlobals::redFukaHitsoundHandle);
    freeTrack(GraphiteGlobals::redChouHitsoundHandle);
    freeTrack(GraphiteGlobals::redAdLibHitsoundHandle);
    freeTrack(GraphiteGlobals::greenNoteCompleteHitsoundHandle);

    JudgementThread::stop();
}

void GameManager::_process(double delta)
{
    if (!processFunctionRan)
    {
        initializeInputEngine();
        processFunctionRan = true;
    }

    GraphiteGlobals::inputEngine.value().parseEventsSinceLastFrame();
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

    json defaultBindings = json::array();
    defaultBindings.push_back({{"button", "z"}, {"action", "DrumRimLeft"}});
    defaultBindings.push_back({{"button", "x"}, {"action", "DrumCenterLeft"}});
    defaultBindings.push_back({{"button", "n"}, {"action", "DrumCenterRight"}});
    defaultBindings.push_back({{"button", "m"}, {"action", "DrumRimRight"}});
    defaultBindings.push_back({{"button", "enter"}, {"action", "Enter"}});
    defaultBindings.push_back({{"button", "escape"}, {"action", "Back"}});

    json bindings = inputSettingsFile.get<json>("bindings", defaultBindings);
    bool highPriorityInputThread = inputSettingsFile.get<bool>("HighPriorityInputThread", true);
    inputSettingsFile.save();

    for (auto& bindingJson : bindings)
    {
        RhythmInput::RhythmInputBinding binding{};
        binding.button = bindingJson.value("button", "");
        binding.action = bindingJson.value("action", "");
        gameBindings.push_back(binding);
    }

    RhythmInput::RhythmInputSettings inputSettings{};
    inputSettings.highPriorityInputThread = highPriorityInputThread;
    GraphiteGlobals::inputEngine.emplace(gameActions, gameBindings, inputSettings);
}