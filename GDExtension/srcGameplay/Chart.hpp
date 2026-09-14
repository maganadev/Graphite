#ifndef Chart_hpp
#define Chart_hpp

#include <cstdint>
#include <string>
#include <vector>

#include "../srcThirdParty/json.hpp"

#include "Course.hpp"

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

    static Chart FromJson(const nlohmann::json& j)
    {
        Chart chart;
        chart.title = j.value("title", "");
        chart.wave = j.value("wave", "");
        chart.defaultBpm = j.value("defaultBpm_fractional", "0/1");
        chart.defaultBpmDouble = j.value("defaultBpm_double", 0.0);
        chart.defaultOffset = j.value("defaultOffset_fractional", "0/1");
        chart.defaultOffsetPicoseconds = j.value("defaultOffset_picoseconds", static_cast<int64_t>(0));
        if (j.contains("courses"))
        {
            for (const auto& c : j["courses"])
            {
                chart.courses.push_back(Course::FromJson(c));
            }
        }
        return chart;
    }
};

#endif