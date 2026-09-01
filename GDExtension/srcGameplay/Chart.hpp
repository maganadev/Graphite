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
    std::string title;
    std::string wave;
    std::string defaultBpm;
    double defaultBpmDouble{0.0};
    std::string defaultOffset;
    int64_t defaultOffsetPicoseconds{0};
    std::vector<Course> courses;
    std::string activeCourse;

    static Chart FromJson(const nlohmann::json& j)
    {
        Chart chart;
        chart.title = j.value("title", "");
        chart.wave = j.value("wave", "");
        chart.defaultBpm = j.value("defaultBpm", "0/1");
        chart.defaultBpmDouble = j.value("defaultBpm_double", 0.0);
        chart.defaultOffset = j.value("defaultOffset", "0/1");
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

    Course* findCourseByName(const std::string& name) const
    {
        for (auto& course : courses)
        {
            if (course.name == name)
            {
                return const_cast<Course*>(&course);
            }
        }
        return nullptr;
    }
};

#endif
