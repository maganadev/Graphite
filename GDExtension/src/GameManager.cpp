#include "GameManager.hpp"
#include "Globals.hpp"
#include "SettingsFile.hpp"

#include "../../RhythmAudio/RhythmAudio/RhythmAudioEngine.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"

void GameManager::_bind_methods()
{
    //
}

GameManager::GameManager()
{
    //
}

GameManager::~GameManager()
{
    //
}

void GameManager::_ready()
{
    RhythmAudio::RhythmAudioSettings settings{};

    SettingsFile audioSettingsFile("audio_settings.json");
    audioSettingsFile.load();

    audioSettingsFile.ensureContainsString("backendMode", "WASAPIShared");
    audioSettingsFile.ensureContainsInteger("WASAPIShared_sampleRate", 48000);
    audioSettingsFile.ensureContainsInteger("WASAPIShared_bufferSizeInSamples", 1024);
    audioSettingsFile.ensureContainsInteger("WASAPIExclusive_sampleRate", 48000);
    audioSettingsFile.ensureContainsInteger("WASAPIExclusive_bufferSizeInSamples", 256);
    audioSettingsFile.ensureContainsInteger("ALSA_sampleRate", 48000);
    audioSettingsFile.ensureContainsInteger("ALSA_bufferSizeInSamples", 1024);
    audioSettingsFile.ensureContainsInteger("JACK_sampleRate", 48000);
    audioSettingsFile.ensureContainsInteger("JACK_bufferSizeInSamples", 1024);
    audioSettingsFile.save();

    std::string backendMode = audioSettingsFile.jsonObj.value("backendMode", "WASAPIShared");
    if (backendMode == "WASAPIShared")
        settings.backendMode = RhythmAudio::AudioBackendMode::WASAPIShared;
    else if (backendMode == "WASAPIExclusive")
        settings.backendMode = RhythmAudio::AudioBackendMode::WASAPIExclusive;
    else if (backendMode == "JACK")
        settings.backendMode = RhythmAudio::AudioBackendMode::JACK;
    else if (backendMode == "ALSA")
        settings.backendMode = RhythmAudio::AudioBackendMode::ALSA;

    settings.WASAPIShared_sampleRate = audioSettingsFile.jsonObj.value("WASAPIShared_sampleRate", 48000U);
    settings.WASAPIShared_bufferSizeInSamples =
        audioSettingsFile.jsonObj.value("WASAPIShared_bufferSizeInSamples", 1024U);
    settings.WASAPIExclusive_sampleRate = audioSettingsFile.jsonObj.value("WASAPIExclusive_sampleRate", 48000U);
    settings.WASAPIExclusive_bufferSizeInSamples =
        audioSettingsFile.jsonObj.value("WASAPIExclusive_bufferSizeInSamples", 256U);
    settings.ALSA_sampleRate = audioSettingsFile.jsonObj.value("ALSA_sampleRate", 48000U);
    settings.ALSA_bufferSizeInSamples = audioSettingsFile.jsonObj.value("ALSA_bufferSizeInSamples", 1024U);
    settings.JACK_sampleRate = audioSettingsFile.jsonObj.value("JACK_sampleRate", 48000U);
    settings.JACK_bufferSizeInSamples = audioSettingsFile.jsonObj.value("JACK_bufferSizeInSamples", 1024U);

    Globals::audioEngine.emplace(settings);

    std::vector<RhythmInput::RhythmInputAction> gameActions;
    std::vector<RhythmInput::RhythmInputBinding> gameBindings;

    RhythmInput::RhythmInputAction DrumRimLeftKeybind{};
    DrumRimLeftKeybind.name = "DrumRimLeft";
    DrumRimLeftKeybind.callbackOnPress = nullptr;
    DrumRimLeftKeybind.callbackOnRelease = nullptr;
    gameActions.push_back(DrumRimLeftKeybind);

    RhythmInput::RhythmInputAction DrumRimRightKeybind{};
    DrumRimRightKeybind.name = "DrumRimRight";
    DrumRimRightKeybind.callbackOnPress = nullptr;
    DrumRimRightKeybind.callbackOnRelease = nullptr;
    gameActions.push_back(DrumRimRightKeybind);

    RhythmInput::RhythmInputAction DrumCenterLeftKeybind{};
    DrumCenterLeftKeybind.name = "DrumCenterLeft";
    DrumCenterLeftKeybind.callbackOnPress = nullptr;
    DrumCenterLeftKeybind.callbackOnRelease = nullptr;
    gameActions.push_back(DrumCenterLeftKeybind);

    RhythmInput::RhythmInputAction DrumCenterRightKeybind{};
    DrumCenterRightKeybind.name = "DrumCenterRight";
    DrumCenterRightKeybind.callbackOnPress = nullptr;
    DrumCenterRightKeybind.callbackOnRelease = nullptr;
    gameActions.push_back(DrumCenterRightKeybind);

    RhythmInput::RhythmInputAction EnterKeybind{};
    EnterKeybind.name = "Enter";
    EnterKeybind.callbackOnPress = nullptr;
    EnterKeybind.callbackOnRelease = nullptr;
    gameActions.push_back(EnterKeybind);

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

    Globals::inputEngine.emplace(gameActions, gameBindings);
}

void GameManager::_process(double delta)
{
    // Parse events
    Globals::inputEngine->parseEventsSinceLastFrame();
}
