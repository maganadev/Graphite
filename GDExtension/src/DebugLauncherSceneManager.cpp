#include "DebugLauncherSceneManager.hpp"
#include <godot_cpp/classes/scene_tree.hpp>

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
    if (!Globals::inputEngine.has_value())
    {
        return;
    }

    auto& actions = RhythmInput::RhythmInputEngine::gameActions;
    for (auto& action : actions)
    {
        if (action.name == "Enter" && action.timesPressedSinceLastFrame > 0)
        {
            get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
            return;
        }
    }
}