#ifndef RhythmJudgementSystem_hpp
#define RhythmJudgementSystem_hpp

#include <cstdint>

#include "CompletionList.hpp"
#include "RhythmEnums.hpp"

enum class Lanes : size_t
{
    Red = 0,
    Blue = 1,
};

class RhythmJudgementSystem
{
public:
    using Subseconds = int64_t;

    static constexpr Subseconds TIME_WINDOW_ABOUT_TO_BE_OOR = 110000000000; // 110ms in picoseconds
    static constexpr Subseconds TIME_WINDOW_FUKA = 100000000000;
    static constexpr Subseconds TIME_WINDOW_KA = 80000000000;
    static constexpr Subseconds TIME_WINDOW_RYOU = 46000000000;
    static constexpr Subseconds TIME_WINDOW_CHOU = 20000000000;
    static constexpr size_t LANE_COUNT = 2;

    struct NoteTarget
    {
        int64_t positionPicoseconds;
        bool judged;
    };

private:
    CompletionList<size_t> lanes[LANE_COUNT];
    std::vector<NoteTarget> noteTargets;

public:
    size_t registerNote(int64_t positionPicoseconds, NoteTypes type)
    {
        size_t index = noteTargets.size();
        noteTargets.push_back({positionPicoseconds, false});
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

    size_t noteTargetCount() const
    {
        return noteTargets.size();
    }

    const NoteTarget& getNoteTarget(size_t index) const
    {
        return noteTargets[index];
    }

    void resetCompletionStates()
    {
        for (size_t i = 0; i < LANE_COUNT; i++)
        {
            lanes[i].resetCompletionStates();
        }
    }

    void markAsJudged(size_t noteIndex)
    {
        if (noteIndex < noteTargets.size())
        {
            noteTargets[noteIndex].judged = true;
        }
    }

    bool getNoteAssociatedWithButtonPress(DrumButtons button, int64_t timeElapsedPicoseconds, int64_t& offTimeAmount, NoteGradings& grading, size_t& judgedNoteIndex)
    {
        size_t lane = (button == DrumButtons::DrumRedLeft || button == DrumButtons::DrumRedRight) ? static_cast<size_t>(Lanes::Red) : static_cast<size_t>(Lanes::Blue);

        CompletionList<size_t>& correctLane = lanes[lane];
        correctLane.pointToFirstUncompleted();

        while (true)
        {
            size_t* currentIndexDP = correctLane.getNextUncompleted();
            if (currentIndexDP == nullptr)
            {
                break;
            }

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

    bool getAbandonedNote(int64_t timeElapsedPicoseconds, size_t& abandonedIndex)
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
                        abandonedIndex = currentIndex;
                        return true;
                    }
                }
                else
                {
                    correctLane.markMostRecentAsCompleted();
                    continue;
                }
            }
        }
        return false;
    }

    static NoteGradings getGradingForOfftime(int64_t timeDelta)
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
};
#endif
