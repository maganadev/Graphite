#include "InputThreadFunctions.hpp"
#include "JudgementThread.hpp"

void InputThreadFunctions::DrumRimLeft(uint64_t timestamp)
{
    InputTimingMessage msg{};
    msg.timestamp = timestamp;
    msg.button = DrumButtons::DrumBlueLeft;
    JudgementThread::messageQueue.try_enqueue(msg);
    JudgementThread::signal();
}

void InputThreadFunctions::DrumCenterLeft(uint64_t timestamp)
{
    InputTimingMessage msg{};
    msg.timestamp = timestamp;
    msg.button = DrumButtons::DrumRedLeft;
    JudgementThread::messageQueue.try_enqueue(msg);
    JudgementThread::signal();
}

void InputThreadFunctions::DrumCenterRight(uint64_t timestamp)
{
    InputTimingMessage msg{};
    msg.timestamp = timestamp;
    msg.button = DrumButtons::DrumRedRight;
    JudgementThread::messageQueue.try_enqueue(msg);
    JudgementThread::signal();
}

void InputThreadFunctions::DrumRimRight(uint64_t timestamp)
{
    InputTimingMessage msg{};
    msg.timestamp = timestamp;
    msg.button = DrumButtons::DrumBlueRight;
    JudgementThread::messageQueue.try_enqueue(msg);
    JudgementThread::signal();
}
