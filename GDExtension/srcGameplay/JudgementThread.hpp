#ifndef JudgementThread_hpp
#define JudgementThread_hpp

#include "../../RhythmAudio/RhythmAudio/QueueSPSC.hpp"
#include "CompletionList.hpp"
#include "Course.hpp"
#include "RhythmEnums.hpp"
#include <atomic>
#include <cstdint>
#include <semaphore>
#include <thread>
#include <vector>

class Chart;

enum class Lanes : size_t
{
    Red = 0,
    Blue = 1,
};

struct InputTimingMessage
{
    uint64_t timestamp;
    DrumButtons button;
};

class JudgementThread
{
public:
    static NoteGradings getGradingForOfftime(int64_t timeDelta, const Chart* chart);
    static void gradeNoteIfNoteExists(CompletionList<HittableNoteVariant>& lane, int64_t songPositionPs, NoteGradings& outGrading, const Chart* chart);
    static void gradeAllAbandonedNotes(CompletionList<HittableNoteVariant>& lane, int64_t songPositionPs, const Chart* chart);

    // Thread lifecycle
    static void start();
    static void stop();
    static void signal();
    static bool isRunning();

    // Queues
    static QueueSPSC<InputTimingMessage, 1024> messageQueue;
    static QueueSPSC<uint64_t, 1024> abandonedCheckQueue;
    static std::counting_semaphore<1> semaphore;
    static std::thread thread;

    static std::atomic<int64_t> judgementOffset;

private:
    static std::atomic<bool> requestShutdown;
    static void threadBehavior();
};

#endif