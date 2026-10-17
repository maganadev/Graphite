#ifndef Chart_hpp
#define Chart_hpp

#include "../srcThirdParty/json.hpp"
#include "Course.hpp"
#include "Fraction.hpp"
#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

class Chart
{
public:
    ////////////////////////////////////////////////////////////
    //
    // Information stored in the chart
    //
    ////////////////////////////////////////////////////////////
    std::string title;
    std::string wave;
    std::string defaultBpm;
    double defaultBpmDouble{0.0};
    std::string defaultOffset;
    int64_t defaultOffsetPicoseconds{0};
    std::vector<Course> courses;

    ////////////////////////////////////////////////////////////
    //
    // Tacked-on information for the game to use during play
    //
    ////////////////////////////////////////////////////////////
    int32_t activeCourseIndex{-1};
    int64_t hitWindowAboutToBeOutOfRange{110000000000};
    int64_t hitWindowFuka{100000000000};
    int64_t hitWindowKa{80000000000};
    int64_t hitWindowRyou{46000000000};
    int64_t hitWindowChou{20000000000};
    bool gameplayActive{false};
    double playbackRate{1.0};
    double userScrollModifier{1.0};

    // Current combo count. Written by the judgement thread and read by the
    // gameplay thread, so it is atomic. Mutable so it can be updated through the
    // const chart pointer held by judgement readers.
    mutable std::atomic<int32_t> combo{0};

    // Chart is not copyable/movable because of the atomic members, so this is
    // used to wipe the chart back to a clean state between runs.
    void reset()
    {
        title.clear();
        wave.clear();
        defaultBpm.clear();
        defaultBpmDouble = 0.0;
        defaultOffset.clear();
        defaultOffsetPicoseconds = 0;
        courses.clear();

        activeCourseIndex = -1;
        hitWindowAboutToBeOutOfRange = 110000000000;
        hitWindowFuka = 100000000000;
        hitWindowKa = 80000000000;
        hitWindowRyou = 46000000000;
        hitWindowChou = 20000000000;
        gameplayActive = false;
        playbackRate = 1.0;
        userScrollModifier = 1.0;
        combo.store(0, std::memory_order_release);
    }
};

#endif
