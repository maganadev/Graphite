#ifndef Chart_hpp
#define Chart_hpp

#include "../srcThirdParty/json.hpp"
#include "Course.hpp"
#include "Fraction.hpp"
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
};

#endif
