#include "GameplaySceneManager.hpp"

#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"

void GameplaySceneManager::_bind_methods()
{
    //
}

GameplaySceneManager::GameplaySceneManager()
{
    //
}

GameplaySceneManager::~GameplaySceneManager()
{
    //
}

void GameplaySceneManager::_ready()
{
//
}

void GameplaySceneManager::_process(double delta)
{
    // Parse events
    inputEngine->parseEventsSinceLastFrame();
}
