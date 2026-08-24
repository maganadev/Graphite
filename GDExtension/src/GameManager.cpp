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

    std::string backendMode = audioSettingsFile.jsonObj.value("backendMode", "WASAPIShared");
    if (backendMode == "WASAPIShared")       settings.backendMode = RhythmAudio::AudioBackendMode::WASAPIShared;
    else if (backendMode == "WASAPIExclusive") settings.backendMode = RhythmAudio::AudioBackendMode::WASAPIExclusive;
    else if (backendMode == "JACK")           settings.backendMode = RhythmAudio::AudioBackendMode::JACK;
    else if (backendMode == "ALSA")           settings.backendMode = RhythmAudio::AudioBackendMode::ALSA;

    settings.WASAPIShared_sampleRate       = audioSettingsFile.jsonObj.value("WASAPIShared_sampleRate", 0U);
    settings.WASAPIShared_bufferSizeInSamples = audioSettingsFile.jsonObj.value("WASAPIShared_bufferSizeInSamples", 0U);
    settings.WASAPIExclusive_sampleRate    = audioSettingsFile.jsonObj.value("WASAPIExclusive_sampleRate", 0U);
    settings.WASAPIExclusive_bufferSizeInSamples = audioSettingsFile.jsonObj.value("WASAPIExclusive_bufferSizeInSamples", 0U);
    settings.ALSA_sampleRate               = audioSettingsFile.jsonObj.value("ALSA_sampleRate", 0U);
    settings.ALSA_bufferSizeInSamples      = audioSettingsFile.jsonObj.value("ALSA_bufferSizeInSamples", 0U);
    settings.JACK_sampleRate               = audioSettingsFile.jsonObj.value("JACK_sampleRate", 0U);
    settings.JACK_bufferSizeInSamples      = audioSettingsFile.jsonObj.value("JACK_bufferSizeInSamples", 0U);

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

    SettingsFile inputSettingsFile("input_settings.json");
    inputSettingsFile.load();

    if (inputSettingsFile.jsonObj.contains("bindings"))
    {
        for (auto& bindingJson : inputSettingsFile.jsonObj["bindings"])
        {
            RhythmInput::RhythmInputBinding binding{};
            binding.button = bindingJson.value("button", "");
            binding.action = bindingJson.value("action", "");
            gameBindings.push_back(binding);
        }
    }

    Globals::inputEngine.emplace(gameActions, gameBindings);
}

void GameManager::_process(double delta)
{
    // Parse events
    Globals::inputEngine->parseEventsSinceLastFrame();
}
