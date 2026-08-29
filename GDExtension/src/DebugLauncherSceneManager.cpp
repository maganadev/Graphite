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
    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::Enter].timesPressedSinceLastFrame > 0)
    {
        godot::UtilityFunctions::print("Enter action detected, loading GameplayScene");
        get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        return;
    }
}