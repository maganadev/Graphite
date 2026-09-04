#include "GameManager.hpp"
#include "../../RhythmAudio/RhythmAudio/RhythmAudioEngine.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"
#include "Course.hpp"
#include "InputThreadFunctions.hpp"
#include "JudgementThread.hpp"
#include "SettingsFile.hpp"

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
    audioSettingsFile.ensureContainsString("backendMode", "");
    audioSettingsFile.ensureContainsString("audioMonoStereo", "stereo");
    audioSettingsFile.ensureContainsInteger("ASIO_sampleRate", 0);
    audioSettingsFile.ensureContainsInteger("ASIO_bufferSizeInSamples", 0);
    audioSettingsFile.ensureContainsString("ASIO_cardToUse", "");
    audioSettingsFile.ensureContainsInteger("ASIO_leftChannel", 0);
    audioSettingsFile.ensureContainsInteger("ASIO_rightChannel", 1);
    audioSettingsFile.save();

    // Populate C++ RhythmAudioSettings from JSON
    std::string backendMode = audioSettingsFile.jsonObj.value("backendMode", "");
    if (backendMode == "ASIO")
    {
        settings.backendMode = RhythmAudio::AudioBackendMode::ASIO;
    }
    settings.audioMonoStereo = audioSettingsFile.jsonObj.value("audioMonoStereo", "stereo");
    settings.ASIO_sampleRate = audioSettingsFile.jsonObj.value("ASIO_sampleRate", 0);
    settings.ASIO_bufferSizeInSamples = audioSettingsFile.jsonObj.value("ASIO_bufferSizeInSamples", 0);
    settings.ASIO_cardToUse = audioSettingsFile.jsonObj.value("ASIO_cardToUse", "");
    settings.ASIO_leftChannel = audioSettingsFile.jsonObj.value("ASIO_leftChannel", 0);
    settings.ASIO_rightChannel = audioSettingsFile.jsonObj.value("ASIO_rightChannel", 1);
    GraphiteGlobals::audioEngine.emplace(settings);

    SettingsFile offsetSettingsFile("offset_settings.json");
    offsetSettingsFile.load();
    offsetSettingsFile.ensureContainsInteger("AudioOffset", 0);
    offsetSettingsFile.ensureContainsInteger("VisualOffset", 0);
    offsetSettingsFile.save();
    GraphiteGlobals::audioOffset = offsetSettingsFile.jsonObj.value("AudioOffset", 0);
    GraphiteGlobals::visualOffset = offsetSettingsFile.jsonObj.value("VisualOffset", 0);

    GraphiteGlobals::audioEngine.value().createAudioTrack("GameplayBlueRyouHitsound.ogg", -36, GraphiteGlobals::blueRyouHitsoundHandle);
    GraphiteGlobals::audioEngine.value().createAudioTrack("GameplayBlueKaHitsound.ogg", -36, GraphiteGlobals::blueKaHitsoundHandle);
    GraphiteGlobals::audioEngine.value().createAudioTrack("GameplayBlueFukaHitsound.ogg", -36, GraphiteGlobals::blueFukaHitsoundHandle);
    GraphiteGlobals::audioEngine.value().createAudioTrack("GameplayBlueChouHitsound.ogg", -36, GraphiteGlobals::blueChouHitsoundHandle);
    GraphiteGlobals::audioEngine.value().createAudioTrack("GameplayBlueAdLibHitsound.ogg", -36, GraphiteGlobals::blueAdLibHitsoundHandle);
    GraphiteGlobals::audioEngine.value().createAudioTrack("GameplayRedRyouHitsound.ogg", -36, GraphiteGlobals::redRyouHitsoundHandle);
    GraphiteGlobals::audioEngine.value().createAudioTrack("GameplayRedKaHitsound.ogg", -36, GraphiteGlobals::redKaHitsoundHandle);
    GraphiteGlobals::audioEngine.value().createAudioTrack("GameplayRedFukaHitsound.ogg", -36, GraphiteGlobals::redFukaHitsoundHandle);
    GraphiteGlobals::audioEngine.value().createAudioTrack("GameplayRedChouHitsound.ogg", -36, GraphiteGlobals::redChouHitsoundHandle);
    GraphiteGlobals::audioEngine.value().createAudioTrack("GameplayRedAdLibHitsound.ogg", -36, GraphiteGlobals::redAdLibHitsoundHandle);
    GraphiteGlobals::audioEngine.value().createAudioTrack("GameplaySineWave.ogg", -6, GraphiteGlobals::sineWaveHitsoundHandle);

    UtilityFunctions::print("GraphiteGlobals::blueRyouHitsoundHandle: ", std::to_string(GraphiteGlobals::blueRyouHitsoundHandle).c_str());

    JudgementThread::start();

    RhythmAudio::RhythmAudioStats stats{};
    GraphiteGlobals::audioEngine.value().getEngineStats(stats);
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
            GraphiteGlobals::audioEngine.value().freeAudioTrack(handle);
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
    inputSettingsFile.ensureContainsInteger("uncappedPolling", 0);
    inputSettingsFile.save();

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

    RhythmInput::RhythmInputSettings inputSettings{};
    inputSettings.uncappedPolling = inputSettingsFile.jsonObj.value("uncappedPolling", 0) != 0;
    GraphiteGlobals::inputEngine.emplace(gameActions, gameBindings, inputSettings);
}