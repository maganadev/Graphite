#ifndef JudgementThread_hpp
#define JudgementThread_hpp

#include "../../RhythmAudio/RhythmAudio/QueueSPSC.hpp"
#include "BlueNote.hpp"
#include "CompletionList.hpp"
#include "GreenNote.hpp"
#include "RedNote.hpp"
#include "RhythmEnums.hpp"
#include "YellowNote.hpp"
#include <atomic>
#include <cstdint>
#include <semaphore>
#include <thread>
#include <variant>
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

class JudgementThread
{
public:
    static constexpr int64_t TIME_WINDOW_ABOUT_TO_BE_OOR = 110000000000;
    static constexpr int64_t TIME_WINDOW_FUKA = 100000000000;
    static constexpr int64_t TIME_WINDOW_KA = 80000000000;
    static constexpr int64_t TIME_WINDOW_RYOU = 46000000000;
    static constexpr int64_t TIME_WINDOW_CHOU = 20000000000;
    static constexpr size_t LANE_COUNT = 2;

    static NoteGradings getGradingForOfftime(int64_t timeDelta);
    static void gradeNoteIfNoteExists(CompletionList<std::variant<RedNote*, BlueNote*, YellowNote*, GreenNote*>>& lane, int64_t songPositionPs, NoteGradings& outGrading);

    // Thread lifecycle
    static void start();
    static void stop();
    static void signal();
    static bool isRunning();

    // Queues
    static QueueSPSC<InputTimingMessage, 1024> messageQueue;
    static std::counting_semaphore<1> semaphore;
    static std::thread thread;

private:
    static std::atomic<bool> requestShutdown;
    static void threadBehavior();
};

#endif
