#include "JudgementThread.hpp"
#include "GameManager.hpp"
#include "GraphiteGlobals.hpp"

QueueSPSC<InputTimingMessage, 1024> JudgementThread::messageQueue{};
QueueSPSC<uint64_t, 1024> JudgementThread::abandonedCheckQueue{};
std::counting_semaphore<1> JudgementThread::semaphore{0};
std::thread JudgementThread::thread{};
std::atomic<bool> JudgementThread::requestShutdown{false};
std::atomic<int64_t> JudgementThread::judgementOffset{0};

void JudgementThread::gradeNoteIfNoteExists(CompletionList<HittableNoteVariant>& lane, int64_t songPositionPs, NoteGradings& outGrading, const Chart* chart)
{
    lane.pointToFirstUncompleted();

    auto* noteVariant = lane.getNextUncompleted();
    while (noteVariant != nullptr)
    {
        NoteGradings grading;
        int64_t offtime;
        (*noteVariant)->getWhatGradingWouldBe(songPositionPs, chart, grading, offtime);

        if (grading == NoteGradings::Early_OutOfRange)
        {
            break;
        }

        if (NoteGradings::Early_Fuka <= grading && grading <= NoteGradings::Late_Fuka)
        {
            if ((*noteVariant)->handleStrikeAndGetCompleted(grading, offtime))
            {
                lane.markMostRecentAsCompleted();
            }
            outGrading = grading;
            return;
        }

        noteVariant = lane.getNextUncompleted();
    }
}

void JudgementThread::gradeAllAbandonedNotes(CompletionList<HittableNoteVariant>& lane, int64_t songPositionPs, const Chart* chart)
{
    lane.pointToFirstUncompleted();

    auto* noteVariant = lane.getNextUncompleted();
    while (noteVariant != nullptr)
    {
        NoteGradings grading;
        int64_t offtime;
        (*noteVariant)->getWhatGradingWouldBe(songPositionPs, chart, grading, offtime);

        if (grading == NoteGradings::Late_OutOfRange)
        {
            if ((*noteVariant)->isSpamNote())
            {
                (*noteVariant)->finishedJudging.store(true, std::memory_order_release);
            }
            else
            {
                (*noteVariant)->handleStrikeAndGetCompleted(NoteGradings::Late_OutOfRange, offtime);
            }
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
            LFProtectObjReadGuard<Chart> chartGuard(GraphiteGlobals::currentChart);
            if (!chartGuard.objRef)
            {
                continue;
            }

            // If the gameplay is not active, we don't want to grade any notes, so we bail here
            if (!chartGuard.objRef->gameplayActive)
            {
                continue;
            }

            // Convert CPU picosecond timestamp to song position, bail if failed
            int64_t songPositionPs = 0;
            uint64_t outHandle = 0;
            if (!GraphiteGlobals::audioEngine.value().getPositionForAudioTrack(msg.timestamp, songPositionPs, outHandle))
            {
                continue;
            }

            // Bail if failed
            if (outHandle == 0)
            {
                continue;
            }

            // Apply the judgement offset
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

            if (chartGuard.objRef->activeCourseIndex < 0)
            {
                continue;
            }

            Course* course = const_cast<Course*>(&chartGuard.objRef->courses[chartGuard.objRef->activeCourseIndex]);

            CompletionList<HittableNoteVariant>& lane = (laneIndex == static_cast<size_t>(Lanes::Red)) ? course->laneRed : course->laneBlue;
            NoteGradings outGrading = NoteGradings::Ungraded;
            gradeNoteIfNoteExists(lane, songPositionPs, outGrading, chartGuard.objRef);

            const bool isRed = (laneIndex == static_cast<size_t>(Lanes::Red));
            uint64_t hitsoundHandle = 0;
            if (isRed)
            {
                switch (outGrading)
                {
                case NoteGradings::Early_Chou:
                case NoteGradings::Late_Chou:
                case NoteGradings::CompletelyPerfect:
                    hitsoundHandle = GraphiteGlobals::redChouHitsoundHandle;
                    break;
                case NoteGradings::Early_Ryou:
                case NoteGradings::Late_Ryou:
                    hitsoundHandle = GraphiteGlobals::redRyouHitsoundHandle;
                    break;
                case NoteGradings::Early_Ka:
                case NoteGradings::Late_Ka:
                    hitsoundHandle = GraphiteGlobals::redKaHitsoundHandle;
                    break;
                case NoteGradings::Early_Fuka:
                case NoteGradings::Late_Fuka:
                    hitsoundHandle = GraphiteGlobals::redFukaHitsoundHandle;
                    break;
                default:
                    hitsoundHandle = GraphiteGlobals::redAdLibHitsoundHandle;
                    break;
                }
            }
            else
            {
                switch (outGrading)
                {
                case NoteGradings::Early_Chou:
                case NoteGradings::Late_Chou:
                case NoteGradings::CompletelyPerfect:
                    hitsoundHandle = GraphiteGlobals::blueChouHitsoundHandle;
                    break;
                case NoteGradings::Early_Ryou:
                case NoteGradings::Late_Ryou:
                    hitsoundHandle = GraphiteGlobals::blueRyouHitsoundHandle;
                    break;
                case NoteGradings::Early_Ka:
                case NoteGradings::Late_Ka:
                    hitsoundHandle = GraphiteGlobals::blueKaHitsoundHandle;
                    break;
                case NoteGradings::Early_Fuka:
                case NoteGradings::Late_Fuka:
                    hitsoundHandle = GraphiteGlobals::blueFukaHitsoundHandle;
                    break;
                default:
                    hitsoundHandle = GraphiteGlobals::blueAdLibHitsoundHandle;
                    break;
                }
            }

            if (hitsoundHandle != 0 && !GraphiteGlobals::modVisualOffsetCalibration)
            {
                GraphiteGlobals::audioEngine.value().playAudioTrack(hitsoundHandle);
            }
        }

        uint64_t abandonedTimestamp = 0;
        while (abandonedCheckQueue.try_dequeue(abandonedTimestamp))
        {
            LFProtectObjReadGuard<Chart> chartGuard(GraphiteGlobals::currentChart);
            if (!chartGuard.objRef)
            {
                continue;
            }

            // If the gameplay is not active, we don't want to grade any notes, so we bail here
            if (!chartGuard.objRef->gameplayActive)
            {
                continue;
            }

            // Convert CPU picosecond timestamp to song position, bail if failed
            int64_t songPositionPs = 0;
            uint64_t outHandle = 0;
            if (!GraphiteGlobals::audioEngine.value().getPositionForAudioTrack(abandonedTimestamp, songPositionPs, outHandle))
            {
                continue;
            }

            // Bail if failed
            if (outHandle == 0)
            {
                continue;
            }

            // Apply the judgement offset
            songPositionPs -= judgementOffset.load(std::memory_order_acquire);

            if (chartGuard.objRef->activeCourseIndex < 0)
            {
                continue;
            }

            Course* course = const_cast<Course*>(&chartGuard.objRef->courses[chartGuard.objRef->activeCourseIndex]);

            gradeAllAbandonedNotes(course->laneRed, songPositionPs, chartGuard.objRef);
            gradeAllAbandonedNotes(course->laneBlue, songPositionPs, chartGuard.objRef);
        }
    }
}

void JudgementThread::start()
{
    requestShutdown.store(false, std::memory_order_release);
    thread = std::thread(threadBehavior);
    thread.detach();
}

void JudgementThread::stop()
{
    requestShutdown.store(true, std::memory_order_release);
    signal();
}

void JudgementThread::signal()
{
    semaphore.release();
}

bool JudgementThread::isRunning()
{
    return thread.joinable();
}