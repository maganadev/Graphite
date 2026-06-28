#include <Windows.h>
#define DO_NOT_REORDER_ABOVE
#include "GDExtTestObj.hpp"
#include "TanoshiiTJAGameplayScene/GameplayGlobals.hpp"
#include "TanoshiiTJAGameplayScene/OtogeTime.hpp"
#include "UtilsCode/TextEncoding.hpp"

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
    rhythmAudio{settings};

    // Create actual sound file
    std::string filename = "C:/Users/WindowsUser/Repos/RhythmAudio/audioFile1.ogg";
    if (!rhythmAudio.createAudioTrackBlocking(filename, -28.0, audioFileHandle))
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

    inputEngine = (gameActions, gameBindings);
}

void GDExtTestObj::_process(double delta)
{
    // Parse events
    myRhythmInput.parseEventsSinceLastFrame();

    if (myRhythmInput.gameActions[i].timesPressedSinceLastFrame != 0)
    {
        godot::UtilityFunctions::print("W pressed!");
        audioEngine.playAudioTrack(audioFileHandle);
    }
}
