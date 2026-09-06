#include "DebugLauncherSceneManager.hpp"
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void DebugLauncherSceneManager::_bind_methods()
{
    //
}

DebugLauncherSceneManager::DebugLauncherSceneManager()
{
    //
}

DebugLauncherSceneManager::~DebugLauncherSceneManager()
{
    //
}

void DebugLauncherSceneManager::_ready()
{
    //
}

void DebugLauncherSceneManager::_process(double delta)
{
    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::PlaySong].timesPressedSinceLastFrame > 0)
    {
        godot::UtilityFunctions::print("PlaySong action detected, loading GameplayScene with Song.json");
        GraphiteGlobals::modVisualOffsetCalibration = false;
        GraphiteGlobals::modAudioOffsetCalibration = false;
        GraphiteGlobals::currentSongFileName = "Song.json";
        get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        return;
    }

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::AudioCalibration].timesPressedSinceLastFrame > 0)
    {
        godot::UtilityFunctions::print("AudioCalibration action detected, loading GameplayScene with MagicCalibration.json");
        GraphiteGlobals::modVisualOffsetCalibration = false;
        GraphiteGlobals::modAudioOffsetCalibration = true;
        GraphiteGlobals::currentSongFileName = "MagicCalibration.json";
        get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        return;
    }

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::VisualCalibration].timesPressedSinceLastFrame > 0)
    {
        godot::UtilityFunctions::print("VisualCalibration action detected, loading GameplayScene with MagicCalibration.json");
        GraphiteGlobals::modVisualOffsetCalibration = true;
        GraphiteGlobals::modAudioOffsetCalibration = false;
        GraphiteGlobals::currentSongFileName = "MagicCalibration.json";
        get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        return;
    }

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::Enter].timesPressedSinceLastFrame > 0)
    {
        godot::UtilityFunctions::print("Enter action detected, loading GameplayScene with Song.json");
        GraphiteGlobals::modVisualOffsetCalibration = false;
        GraphiteGlobals::modAudioOffsetCalibration = false;
        GraphiteGlobals::currentSongFileName = "Song.json";
        get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        return;
    }
}