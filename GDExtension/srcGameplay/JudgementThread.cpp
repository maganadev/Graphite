#include "JudgementThread.hpp"
#include "GameManager.hpp"

CompletionList<size_t> JudgementThread::lanes[LANE_COUNT];
std::vector<NoteTarget> JudgementThread::noteTargets;

QueueSPSC<InputTimingMessage, 1024> JudgementThread::messageQueue{};
QueueSPSC<JudgedNoteMessage, 1024> JudgementThread::judgedNoteQueue{};

std::counting_semaphore<1> JudgementThread::semaphore{0};
std::thread JudgementThread::thread{};
std::atomic<bool> JudgementThread::requestShutdown{false};

size_t JudgementThread::registerNote(int64_t positionPicoseconds, NoteTypes type)
{
    size_t index = noteTargets.size();
    noteTargets.push_back({positionPicoseconds, false, NoteGradings::Ungraded});
    switch (type)
    {
    case NoteTypes::RedNoteSmall:
    case NoteTypes::RedNoteLarge:
        lanes[static_cast<size_t>(Lanes::Red)].push_back(index);
        break;
    case NoteTypes::BlueNoteSmall:
    case NoteTypes::BlueNoteLarge:
        lanes[static_cast<size_t>(Lanes::Blue)].push_back(index);
        break;
    }
    return index;
}

size_t JudgementThread::noteTargetCount()
{
    return noteTargets.size();
}

const NoteTarget& JudgementThread::getNoteTarget(size_t index)
{
    return noteTargets[index];
}

void JudgementThread::resetCompletionStates()
{
    for (size_t i = 0; i < LANE_COUNT; i++)
    {
        lanes[i].resetCompletionStates();
    }
}

bool JudgementThread::getNoteAssociatedWithButtonPress(DrumButtons button, int64_t timeElapsedPicoseconds, int64_t& offTimeAmount, NoteGradings& grading, size_t& judgedNoteIndex)
{
    size_t lane = (button == DrumButtons::DrumRedLeft || button == DrumButtons::DrumRedRight) ? static_cast<size_t>(Lanes::Red) : static_cast<size_t>(Lanes::Blue);

    CompletionList<size_t>& correctLane = lanes[lane];
    correctLane.pointToFirstUncompleted();

    while (true)
    {
        size_t* currentIndexDP = correctLane.getNextUncompleted();
        if (currentIndexDP == nullptr)
            break;

        size_t currentIndex = *currentIndexDP;
        if (currentIndex >= noteTargets.size())
        {
            correctLane.markMostRecentAsCompleted();
            continue;
        }

        NoteTarget& target = noteTargets[currentIndex];
        if (!target.judged)
        {
            offTimeAmount = timeElapsedPicoseconds - target.positionPicoseconds;
            grading = getGradingForOfftime(offTimeAmount);

            if (grading >= NoteGradings::Early_OutOfRange && grading <= NoteGradings::Early_AboutToBeOOR)
            {
                return false;
            }
            else if (grading >= NoteGradings::Early_Fuka && grading <= NoteGradings::Late_Fuka)
            {
                correctLane.markMostRecentAsCompleted();
                target.judged = true;
                target.grading = grading;
                judgedNoteIndex = currentIndex;
                return true;
            }
            else
            {
                continue;
            }
        }
        else
        {
            correctLane.markMostRecentAsCompleted();
            continue;
        }
    }
    return false;
}

bool JudgementThread::getAbandonedNote(int64_t timeElapsedPicoseconds, size_t& abandonedIndex)
{
    for (size_t lane = 0; lane < LANE_COUNT; lane++)
    {
        CompletionList<size_t>& correctLane = lanes[lane];
        correctLane.pointToFirstUncompleted();
        while (true)
        {
            size_t* currentIndexDP = correctLane.getNextUncompleted();
            if (currentIndexDP == nullptr)
                break;

            size_t currentIndex = *currentIndexDP;
            if (currentIndex >= noteTargets.size())
            {
                correctLane.markMostRecentAsCompleted();
                continue;
            }

            NoteTarget& target = noteTargets[currentIndex];
            if (!target.judged)
            {
                int64_t offTimeAmount = timeElapsedPicoseconds - target.positionPicoseconds;
                NoteGradings gradingIfHit = getGradingForOfftime(offTimeAmount);

                if (gradingIfHit >= NoteGradings::Early_OutOfRange && gradingIfHit <= NoteGradings::Late_AboutToBeOOR)
                {
                    return false;
                }
                else
                {
                    correctLane.markMostRecentAsCompleted();
                    target.judged = true;
                    target.grading = NoteGradings::Late_OutOfRange;
                    abandonedIndex = currentIndex;
                    return true;
                }
            }
            else
            {
                // This note was already graded, skip
                correctLane.markMostRecentAsCompleted();
                continue;
            }
        }
    }
    return false;
}

NoteGradings JudgementThread::getGradingForOfftime(int64_t timeDelta)
{
    if (timeDelta < 0)
    {
        int64_t absDelta = -timeDelta;
        if (absDelta <= TIME_WINDOW_CHOU)
            return NoteGradings::Early_Chou;
        if (absDelta <= TIME_WINDOW_RYOU)
            return NoteGradings::Early_Ryou;
        if (absDelta <= TIME_WINDOW_KA)
            return NoteGradings::Early_Ka;
        if (absDelta <= TIME_WINDOW_FUKA)
            return NoteGradings::Early_Fuka;
        if (absDelta <= TIME_WINDOW_ABOUT_TO_BE_OOR)
            return NoteGradings::Early_AboutToBeOOR;
        return NoteGradings::Early_OutOfRange;
    }
    else if (timeDelta > 0)
    {
        if (timeDelta <= TIME_WINDOW_CHOU)
            return NoteGradings::Late_Chou;
        if (timeDelta <= TIME_WINDOW_RYOU)
            return NoteGradings::Late_Ryou;
        if (timeDelta <= TIME_WINDOW_KA)
            return NoteGradings::Late_Ka;
        if (timeDelta <= TIME_WINDOW_FUKA)
            return NoteGradings::Late_Fuka;
        if (timeDelta <= TIME_WINDOW_ABOUT_TO_BE_OOR)
            return NoteGradings::Late_AboutToBeOOR;
        return NoteGradings::Late_OutOfRange;
    }
    return NoteGradings::CompletlelyPerfect;
}

void JudgementThread::start()
{
    requestShutdown.store(false, std::memory_order_release);
    thread = std::thread(threadEntry);
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

void JudgementThread::threadEntry()
{
    while (requestShutdown.load(std::memory_order_acquire) == false)
    {
        semaphore.acquire();

        InputTimingMessage msg{};
        while (messageQueue.try_dequeue(msg))
        {
            // Convert CPU picosecond timestamp to song position
            int64_t songPositionPs;
            uint64_t outHandle;
            if (!GameManager::audioEngine->getPositionForAudioTrack(msg.timestamp, songPositionPs, outHandle))
            {
                continue;
            }

            // Judge the note
            int64_t offTimeAmount = 0;
            NoteGradings grading = NoteGradings::Ungraded;
            size_t judgedNoteIndex = 0;
            bool hit = getNoteAssociatedWithButtonPress(msg.button, songPositionPs, offTimeAmount, grading, judgedNoteIndex);

            bool isRed = (msg.button == DrumButtons::DrumRedLeft || msg.button == DrumButtons::DrumRedRight);

            if (hit)
            {
                uint64_t hitsoundHandle = 0;
                switch (grading)
                {
                case NoteGradings::Early_Chou:
                case NoteGradings::CompletlelyPerfect:
                case NoteGradings::Late_Chou:
                    hitsoundHandle = isRed ? GameManager::redChouHitsoundHandle : GameManager::blueChouHitsoundHandle;
                    break;
                case NoteGradings::Early_Ryou:
                case NoteGradings::Late_Ryou:
                    hitsoundHandle = isRed ? GameManager::redRyouHitsoundHandle : GameManager::blueRyouHitsoundHandle;
                    break;
                case NoteGradings::Early_Ka:
                case NoteGradings::Late_Ka:
                    hitsoundHandle = isRed ? GameManager::redKaHitsoundHandle : GameManager::blueKaHitsoundHandle;
                    break;
                case NoteGradings::Early_Fuka:
                case NoteGradings::Late_Fuka:
                    hitsoundHandle = isRed ? GameManager::redFukaHitsoundHandle : GameManager::blueFukaHitsoundHandle;
                    break;
                default:
                    hitsoundHandle = isRed ? GameManager::redAdLibHitsoundHandle : GameManager::blueAdLibHitsoundHandle;
                    break;
                }
                if (hitsoundHandle != 0)
                {
                    GameManager::audioEngine->playAudioTrack(hitsoundHandle);
                }
            }
            else
            {
                uint64_t hitsoundHandle = isRed ? GameManager::redAdLibHitsoundHandle : GameManager::blueAdLibHitsoundHandle;
                if (hitsoundHandle != 0)
                {
                    GameManager::audioEngine->playAudioTrack(hitsoundHandle);
                }
            }
        }
    }
}
