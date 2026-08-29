#include "JudgementThread.hpp"
#include "GameManager.hpp"

CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>> JudgementThread::lanes[LANE_COUNT];
QueueSPSC<InputTimingMessage, 1024> JudgementThread::messageQueue{};
QueueSPSC<JudgedNoteMessage, 1024> JudgementThread::judgedNoteQueue{};
std::counting_semaphore<1> JudgementThread::semaphore{0};
std::thread JudgementThread::thread{};
std::atomic<bool> JudgementThread::requestShutdown{false};

NoteGradings JudgementThread::getGradingForOfftime(int64_t timeDelta)
{
    const int64_t absDelta = std::abs(timeDelta);
    const bool isEarly = timeDelta < 0;
    if (absDelta == 0)
    {
        return NoteGradings::CompletlelyPerfect;
    }
    if (absDelta <= TIME_WINDOW_CHOU)
    {
        return isEarly ? NoteGradings::Early_Chou : NoteGradings::Late_Chou;
    }
    if (absDelta <= TIME_WINDOW_RYOU)
    {
        return isEarly ? NoteGradings::Early_Ryou : NoteGradings::Late_Ryou;
    }
    if (absDelta <= TIME_WINDOW_KA)
    {
        return isEarly ? NoteGradings::Early_Ka : NoteGradings::Late_Ka;
    }
    if (absDelta <= TIME_WINDOW_FUKA)
    {
        return isEarly ? NoteGradings::Early_Fuka : NoteGradings::Late_Fuka;
    }
    if (absDelta <= TIME_WINDOW_ABOUT_TO_BE_OOR)
    {
        return isEarly ? NoteGradings::Early_AboutToBeOOR : NoteGradings::Late_AboutToBeOOR;
    }
    return isEarly ? NoteGradings::Early_OutOfRange : NoteGradings::Late_OutOfRange;
}

void JudgementThread::start()
{
    requestShutdown.store(false, std::memory_order_release);
    thread = std::thread(threadBehavior);
}

void JudgementThread::stop()
{
    if (thread.joinable())
    {
        requestShutdown.store(true, std::memory_order_release);
        semaphore.release();
        thread.join();
    }
}

void JudgementThread::signal()
{
    semaphore.release();
}

bool JudgementThread::isRunning()
{
    return thread.joinable() && !requestShutdown.load(std::memory_order_acquire);
}

void JudgementThread::threadBehavior()
{
    while (requestShutdown.load(std::memory_order_acquire) == false)
    {
        semaphore.acquire();

        InputTimingMessage msg{};
        while (messageQueue.try_dequeue(msg))
        {
            // Convert CPU picosecond timestamp to song position
            int64_t songPositionPs = 0;
            uint64_t outHandle = 0;
            if (!GameManager::audioEngine->getPositionForAudioTrack(msg.timestamp, songPositionPs, outHandle))
            {
                continue;
            }

            // Run through all the notes for the lane corresponding to the button pressed and find a match if it exists
            // If we get a rating of EARLY_OUT_OF_RANGE, stop early
        }
    }
}
