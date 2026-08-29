#include "InputThreadFunctions.hpp"
#include "GameManager.hpp"

void InputThreadFunctions::DrumRimLeft(uint64_t timestamp)
{
    InputTimingMessage msg{};
    msg.timestamp = timestamp;
    msg.audioTrackHandle = GameManager::blueAdLibHitsoundHandle;
    GameManager::inputTimingMessageQueue.try_enqueue(msg);
    GameManager::inputTimingSemaphore.release();
}

void InputThreadFunctions::DrumCenterLeft(uint64_t timestamp)
{
    InputTimingMessage msg{};
    msg.timestamp = timestamp;
    msg.audioTrackHandle = GameManager::redAdLibHitsoundHandle;
    GameManager::inputTimingMessageQueue.try_enqueue(msg);
    GameManager::inputTimingSemaphore.release();
}

void InputThreadFunctions::DrumCenterRight(uint64_t timestamp)
{
    InputTimingMessage msg{};
    msg.timestamp = timestamp;
    msg.audioTrackHandle = GameManager::redAdLibHitsoundHandle;
    GameManager::inputTimingMessageQueue.try_enqueue(msg);
    GameManager::inputTimingSemaphore.release();
}

void InputThreadFunctions::DrumRimRight(uint64_t timestamp)
{
    InputTimingMessage msg{};
    msg.timestamp = timestamp;
    msg.audioTrackHandle = GameManager::blueAdLibHitsoundHandle;
    GameManager::inputTimingMessageQueue.try_enqueue(msg);
    GameManager::inputTimingSemaphore.release();
}