#include "MenuSceneManager.hpp"
#include "GameManager.hpp"
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void MenuSceneManager::_bind_methods()
{
    //
}

MenuSceneManager::MenuSceneManager()
{
    //
}

MenuSceneManager::~MenuSceneManager()
{
    //
}

void MenuSceneManager::_ready()
{
    UtilityFunctions::print("MenuSceneManager ready");
}

void MenuSceneManager::_process(double delta)
{
    //
}