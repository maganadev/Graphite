#include "InputThreadFunctions.hpp"
#include "GameManager.hpp"

void InputThreadFunctions::DrumRimLeft(uint64_t timestamp)
{
    if (GameManager::audioEngine.has_value())
    {
        GameManager::audioEngine->playAudioTrack(GameManager::blueAdLibHitsoundHandle);
    }
}

void InputThreadFunctions::DrumCenterLeft(uint64_t timestamp)
{
    if (GameManager::audioEngine.has_value())
    {
        GameManager::audioEngine->playAudioTrack(GameManager::redAdLibHitsoundHandle);
    }
}

void InputThreadFunctions::DrumCenterRight(uint64_t timestamp)
{
    if (GameManager::audioEngine.has_value())
    {
        GameManager::audioEngine->playAudioTrack(GameManager::redAdLibHitsoundHandle);
    }
}

void InputThreadFunctions::DrumRimRight(uint64_t timestamp)
{
    if (GameManager::audioEngine.has_value())
    {
        GameManager::audioEngine->playAudioTrack(GameManager::blueAdLibHitsoundHandle);
    }
}