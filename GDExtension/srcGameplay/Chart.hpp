#ifndef Chart_hpp
#define Chart_hpp

#include "../srcThirdParty/json.hpp"
#include "Course.hpp"
#include "Fraction.hpp"
#include "JsonKeys.hpp"
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

    static Chart FromJson(const nlohmann::json& j)
    {
        Chart chart;
        chart.title = j.value(JC_TITLE, "");
        chart.wave = j.value(JC_WAVE, "");
        chart.defaultBpm = j.value(JC_DEFAULTBPM_F, "0/1");
        bool convOk;
        Fraction bpmFrac;
        bpmFrac.assignFromString(chart.defaultBpm, convOk);
        if (convOk)
            chart.defaultBpmDouble = bpmFrac.toDouble(convOk);
        chart.defaultOffset = j.value(JC_DEFAULTOFFSET_F, "0/1");
        convOk = false;
        Fraction offFrac;
        offFrac.assignFromString(chart.defaultOffset, convOk);
        if (convOk)
        {
            Fraction ps;
            ps.assignFromUInt64(1000000000000ULL);
            offFrac.multiply(ps);
            bool intOk;
            chart.defaultOffsetPicoseconds = offFrac.toInt(intOk);
        }
        if (j.contains(JC_COURSES))
        {
            for (const auto& c : j[JC_COURSES])
            {
                chart.courses.push_back(Course::FromJson(c));
            }
        }
        return chart;
    }
};

#endif
