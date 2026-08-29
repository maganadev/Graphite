#ifndef JudgementThread_hpp
#define JudgementThread_hpp

#include "../../RhythmAudio/RhythmAudio/QueueSPSC.hpp"
#include "CompletionList.hpp"
#include "RhythmEnums.hpp"
#include <atomic>
#include <cstdint>
#include <semaphore>
#include <thread>
#include <vector>

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

struct JudgedNoteMessage
{
    size_t noteIndex;
    NoteGradings grading;
    bool isHit;
    DrumButtons button;
};

struct NoteTarget
{
    int64_t positionPicoseconds;
    bool judged;
    NoteGradings grading;
};

class JudgementThread
{
public:
    using Subseconds = int64_t;

    static constexpr Subseconds TIME_WINDOW_ABOUT_TO_BE_OOR = 110000000000;
    static constexpr Subseconds TIME_WINDOW_FUKA = 100000000000;
    static constexpr Subseconds TIME_WINDOW_KA = 80000000000;
    static constexpr Subseconds TIME_WINDOW_RYOU = 46000000000;
    static constexpr Subseconds TIME_WINDOW_CHOU = 20000000000;
    static constexpr size_t LANE_COUNT = 2;

    // Note registration / grading
    static size_t registerNote(int64_t positionPicoseconds, NoteTypes type);
    static size_t noteTargetCount();
    static const NoteTarget& getNoteTarget(size_t index);
    static void resetCompletionStates();
    static bool getNoteAssociatedWithButtonPress(DrumButtons button, int64_t timeElapsedPicoseconds, int64_t& offTimeAmount, NoteGradings& grading, size_t& judgedNoteIndex);
    static bool getAbandonedNote(int64_t timeElapsedPicoseconds, size_t& abandonedIndex);
    static NoteGradings getGradingForOfftime(int64_t timeDelta);

    // Thread lifecycle
    static void start();
    static void stop();
    static void signal();
    static bool isRunning();

    // Queues
    static QueueSPSC<InputTimingMessage, 1024> messageQueue;
    static QueueSPSC<JudgedNoteMessage, 1024> judgedNoteQueue;

private:
    static CompletionList<size_t> lanes[LANE_COUNT];
    static std::vector<NoteTarget> noteTargets;

    static std::counting_semaphore<1> semaphore;
    static std::thread thread;
    static std::atomic<bool> requestShutdown;

    static void threadEntry();
};

#endif
