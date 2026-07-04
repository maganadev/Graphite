#include <Windows.h>
#define DO_NOT_REORDER_ABOVE
#include "GDExtTestObj.hpp"
#include <iostream>
#include <string>
#include <vector>

void GDExtTestObj::_bind_methods()
{
    //
}

GDExtTestObj::GDExtTestObj()
{
    //
}

GDExtTestObj::~GDExtTestObj()
{
    //
}

void GDExtTestObj::_ready()
{
    godot::UtilityFunctions::print("Hello from C++!");
    godot::UtilityFunctions::print("Launching RhythmAudio");

    RhythmAudioSettings settings{};
    settings.backendMode = AudioBackendMode::WASAPIShared;
    audioEngine.emplace(settings);

    // Create actual sound file
    std::string filename = "C:/Users/WindowsUser/Repos/RhythmAudio/audioFile1.ogg";
    if (!audioEngine->createAudioTrackBlocking(filename, -28.0, audioFileHandle))
    {
        godot::UtilityFunctions::print("Issue loading audio file");
        std::cout << "Issue loading audio file" << std::endl;
        return;
    }

    godot::UtilityFunctions::print("Launching RhythmInput");

    std::vector<RhythmInputAction> gameActions;
    std::vector<RhythmInputBinding> gameBindings;

    RhythmInputAction wKeybind{};
    wKeybind.name = "wKeybind";
    wKeybind.callbackOnPress = nullptr;
    wKeybind.callbackOnRelease = nullptr;
    gameActions.push_back(wKeybind);

    RhythmInputBinding wBinding;
    wBinding.button = "w";
    wBinding.action = "wKeybind";
    gameBindings.push_back(wBinding);

    inputEngine.emplace(gameActions, gameBindings);
}

void GDExtTestObj::_process(double delta)
{
    // Parse events
    inputEngine->parseEventsSinceLastFrame();

    if (inputEngine->gameActions[0].timesPressedSinceLastFrame != 0)
    {
        godot::UtilityFunctions::print("W pressed!");
        audioEngine->playAudioTrack(audioFileHandle);
    }
}
