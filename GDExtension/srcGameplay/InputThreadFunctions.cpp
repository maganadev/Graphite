#include "InputThreadFunctions.hpp"
#include "GameManager.hpp"

void InputThreadFunctions::DrumRimLeft(uint64_t timestamp)
{
    InputTimingMessage msg{};
    msg.timestamp = timestamp;
    msg.button = DrumButtons::DrumBlueLeft;
    GameManager::JudgementThreadMessageQueue.try_enqueue(msg);
    GameManager::JudgementThreadSemaphore.release();
}

void InputThreadFunctions::DrumCenterLeft(uint64_t timestamp)
{
    InputTimingMessage msg{};
    msg.timestamp = timestamp;
    msg.button = DrumButtons::DrumRedLeft;
    GameManager::JudgementThreadMessageQueue.try_enqueue(msg);
    GameManager::JudgementThreadSemaphore.release();
}

void InputThreadFunctions::DrumCenterRight(uint64_t timestamp)
{
    InputTimingMessage msg{};
    msg.timestamp = timestamp;
    msg.button = DrumButtons::DrumRedRight;
    GameManager::JudgementThreadMessageQueue.try_enqueue(msg);
    GameManager::JudgementThreadSemaphore.release();
}

void InputThreadFunctions::DrumRimRight(uint64_t timestamp)
{
    InputTimingMessage msg{};
    msg.timestamp = timestamp;
    msg.button = DrumButtons::DrumBlueRight;
    GameManager::JudgementThreadMessageQueue.try_enqueue(msg);
    GameManager::JudgementThreadSemaphore.release();
}
