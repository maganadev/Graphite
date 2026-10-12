#ifndef SettingsFile_hpp
#define SettingsFile_hpp

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <type_traits>

#include "../srcThirdParty/json.hpp"

using namespace ::nlohmann;
using namespace ::std;

class SettingsFile
{
private:
    std::string filepath = "";
    uint64_t loadTimeHash = 0;
    json jsonObj{};

    template <typename T>
    static bool matchesType(const json& value)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            return value.is_string();
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            return value.is_boolean();
        }
        else if constexpr (std::is_integral_v<T>)
        {
            return value.is_number_integer();
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            return value.is_number();
        }
        else
        {
            return true;
        }
    }

public:
    SettingsFile(std::string pFilepath);
    void load();
    void save();

    // Reads the stored value for key, falling back to defaultValue when the key
    // is missing or holds an incompatible type. Missing keys are written back so
    // that save() persists the default, keeping memory and storage in sync.
    template <typename T>
    T get(const std::string& key, const T& defaultValue)
    {
        if (!jsonObj.contains(key) || !matchesType<T>(jsonObj[key]))
        {
            jsonObj[key] = defaultValue;
            return defaultValue;
        }
        return jsonObj[key].get<T>();
    }

    template <typename T>
    void set(const std::string& key, const T& value)
    {
        jsonObj[key] = value;
    }

    static uint64_t simpleHash(const std::string& str);
};

#endif
