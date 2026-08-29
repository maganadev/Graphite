#ifndef SettingsFile_hpp
#define SettingsFile_hpp

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "../srcThirdParty/json.hpp"

using namespace ::nlohmann;
using namespace ::std;

class SettingsFile
{
private:
    std::string filepath = "";
    uint64_t loadTimeHash = 0;

public:
    json jsonObj{};

    SettingsFile(std::string pFilepath);
    void load();
    void save();

    void ensureContainsInteger(string key, int64_t defaultValue)
    {
        if (!jsonObj.contains(key) || !jsonObj[key].is_number_integer())
        {
            jsonObj[key] = defaultValue;
        }
    }

    void ensureContainsFloat(string key, float defaultValue)
    {
        if (!jsonObj.contains(key) || !jsonObj[key].is_number_float())
        {
            jsonObj[key] = defaultValue;
        }
    }

    void ensureContainsString(string key, string defaultValue)
    {
        if (!jsonObj.contains(key) || !jsonObj[key].is_string())
        {
            jsonObj[key] = defaultValue;
        }
    }

    static uint64_t simpleHash(const std::string& str);
};

#endif
