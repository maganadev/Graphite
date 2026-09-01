#include "JudgementThread.hpp"
#include "GameManager.hpp"

QueueSPSC<InputTimingMessage, 1024> JudgementThread::messageQueue{};
QueueSPSC<uint64_t, 1024> JudgementThread::abandonedCheckQueue{};
std::counting_semaphore<1> JudgementThread::semaphore{0};
std::thread JudgementThread::thread{};
std::atomic<bool> JudgementThread::requestShutdown{false};
std::atomic<int64_t> JudgementThread::judgementOffset{0};

int64_t getNoteTime(const std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>& noteVariant)
{
    if (auto* note = std::get_if<RedNote*>(&noteVariant))
        return (*note)->getNote().time_picoseconds;
    if (auto* note = std::get_if<BlueNote*>(&noteVariant))
        return (*note)->getNote().time_picoseconds;
    if (auto* note = std::get_if<YellowNote*>(&noteVariant))
        return (*note)->getNote().time_picoseconds;
    if (auto* note = std::get_if<GreenNote*>(&noteVariant))
        return (*note)->getNote().time_picoseconds;
    return 0;
}

void setNoteJudged(const std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>& noteVariant, NoteGradings grading)
{
    if (auto* note = std::get_if<RedNote*>(&noteVariant))
    {
        (*note)->setJudged(grading);
        return;
    }
    if (auto* note = std::get_if<BlueNote*>(&noteVariant))
    {
        (*note)->setJudged(grading);
        return;
    }
    if (auto* note = std::get_if<YellowNote*>(&noteVariant))
    {
        (*note)->setJudged(grading);
        return;
    }
    if (auto* note = std::get_if<GreenNote*>(&noteVariant))
    {
        (*note)->setJudged(grading);
        return;
    }
}

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

void JudgementThread::gradeNoteIfNoteExists(CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>>& lane, int64_t songPositionPs, NoteGradings& outGrading)
{
    lane.pointToFirstUncompleted();

    auto* noteVariant = lane.getNextUncompleted();
    while (noteVariant != nullptr)
    {
        int64_t noteTime = getNoteTime(*noteVariant);
        int64_t timeDelta = songPositionPs - noteTime;
        NoteGradings grading = getGradingForOfftime(timeDelta);

        if (grading == NoteGradings::Early_OutOfRange)
        {
            break;
        }

        if (NoteGradings::Early_Fuka <= grading && grading <= NoteGradings::Late_Fuka)
        {
            setNoteJudged(*noteVariant, grading);
            lane.markMostRecentAsCompleted();
            outGrading = grading;
            return;
        }

        noteVariant = lane.getNextUncompleted();
    }
}

void JudgementThread::gradeAllAbandonedNotes(CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>>& lane, int64_t songPositionPs)
{
    lane.pointToFirstUncompleted();

    auto* noteVariant = lane.getNextUncompleted();
    while (noteVariant != nullptr)
    {
        int64_t noteTime = getNoteTime(*noteVariant);
        int64_t timeDelta = songPositionPs - noteTime;
        NoteGradings grading = getGradingForOfftime(timeDelta);

        if (grading == NoteGradings::Late_OutOfRange)
        {
            setNoteJudged(*noteVariant, NoteGradings::Late_OutOfRange);
            lane.markMostRecentAsCompleted();
        }
        else
        {
            break;
        }

        noteVariant = lane.getNextUncompleted();
    }
}

void JudgementThread::threadBehavior()
{
    while (requestShutdown.load(std::memory_order_acquire) == false)
    {
        semaphore.acquire();

        InputTimingMessage msg{};
        while (messageQueue.try_dequeue(msg))
        {
            LFProtectObjReadGuard<Chart> chartGuard(GameManager::currentChart);
            if (!chartGuard.objRef)
            {
                continue;
            }

            // Convert CPU picosecond timestamp to song position
            int64_t songPositionPs = 0;
            uint64_t outHandle = 0;
            if (!GameManager::audioEngine.value().getPositionForAudioTrack(msg.timestamp, songPositionPs, outHandle))
            {
                continue;
            }
            songPositionPs -= judgementOffset.load(std::memory_order_acquire);

            // Determine which lane this button press maps to
            size_t laneIndex = 0;
            switch (msg.button)
            {
            case DrumButtons::DrumRedLeft:
            case DrumButtons::DrumRedRight:
                laneIndex = static_cast<size_t>(Lanes::Red);
                break;
            case DrumButtons::DrumBlueLeft:
            case DrumButtons::DrumBlueRight:
                laneIndex = static_cast<size_t>(Lanes::Blue);
                break;
            }

            Course* course = chartGuard.objRef->findCourseByName(chartGuard.objRef->activeCourse);
            if (!course)
            {
                continue;
            }

            CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>>& lane = (laneIndex == static_cast<size_t>(Lanes::Red)) ? course->laneRed : course->laneBlue;
            NoteGradings outGrading = NoteGradings::Ungraded;
            gradeNoteIfNoteExists(lane, songPositionPs, outGrading);

            const bool isRed = (laneIndex == static_cast<size_t>(Lanes::Red));
            uint64_t hitsoundHandle = 0;
            if (isRed)
            {
                switch (outGrading)
                {
                case NoteGradings::Early_Chou:
                case NoteGradings::Late_Chou:
                case NoteGradings::CompletlelyPerfect:
                    hitsoundHandle = GameManager::redChouHitsoundHandle;
                    break;
                case NoteGradings::Early_Ryou:
                case NoteGradings::Late_Ryou:
                    hitsoundHandle = GameManager::redRyouHitsoundHandle;
                    break;
                case NoteGradings::Early_Ka:
                case NoteGradings::Late_Ka:
                    hitsoundHandle = GameManager::redKaHitsoundHandle;
                    break;
                case NoteGradings::Early_Fuka:
                case NoteGradings::Late_Fuka:
                    hitsoundHandle = GameManager::redFukaHitsoundHandle;
                    break;
                default:
                    hitsoundHandle = GameManager::redAdLibHitsoundHandle;
                    break;
                }
            }
            else
            {
                switch (outGrading)
                {
                case NoteGradings::Early_Chou:
                case NoteGradings::Late_Chou:
                case NoteGradings::CompletlelyPerfect:
                    hitsoundHandle = GameManager::blueChouHitsoundHandle;
                    break;
                case NoteGradings::Early_Ryou:
                case NoteGradings::Late_Ryou:
                    hitsoundHandle = GameManager::blueRyouHitsoundHandle;
                    break;
                case NoteGradings::Early_Ka:
                case NoteGradings::Late_Ka:
                    hitsoundHandle = GameManager::blueKaHitsoundHandle;
                    break;
                case NoteGradings::Early_Fuka:
                case NoteGradings::Late_Fuka:
                    hitsoundHandle = GameManager::blueFukaHitsoundHandle;
                    break;
                default:
                    hitsoundHandle = GameManager::blueAdLibHitsoundHandle;
                    break;
                }
            }

            if (hitsoundHandle != 0)
            {
                GameManager::audioEngine.value().playAudioTrack(hitsoundHandle);
            }
        }

        while (abandonedCheckQueue.try_dequeue(abandonedTimestamp))
        {
            LFProtectObjReadGuard<Chart> chartGuard(GameManager::currentChart);
            if (!chartGuard.objRef)
            {
                continue;
            }

            uint64_t abandonedTimestamp = 0;
            int64_t songPositionPs = 0;
            uint64_t outHandle = 0;
            if (!GameManager::audioEngine.value().getPositionForAudioTrack(abandonedTimestamp, songPositionPs, outHandle))
            {
                continue;
            }
            songPositionPs -= judgementOffset.load(std::memory_order_acquire);

            Course* course = chartGuard.objRef->findCourseByName(chartGuard.objRef->activeCourse);
            if (!course)
            {
                continue;
            }

            gradeAllAbandonedNotes(course->laneRed, songPositionPs);
            gradeAllAbandonedNotes(course->laneBlue, songPositionPs);
        }
    }
}
