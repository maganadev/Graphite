#include "SettingsFile.hpp"
#include <cstdint>

SettingsFile::SettingsFile(std::string pFilepath)
{
    filepath = pFilepath;
}

void SettingsFile::load()
{
    // Load file
    std::ifstream file(filepath);
    if (file.is_open() && file.good())
    {
        jsonObj = json::parse(file);
    }

    // Instantiate hash if it does not exist
    if (!jsonObj.contains("hash"))
    {
        jsonObj["hash"] = 0;
    }

    // Keep hash
    loadTimeHash = jsonObj["hash"];
}

void SettingsFile::save()
{
    // Set our hash to zero
    // Get string of our object
    // Get hash of our string
    jsonObj["hash"] = 0;
    std::string settingsText = jsonObj.dump();
    uint64_t newHash = simpleHash(settingsText);

    // If the file changed since last save
    if (newHash != loadTimeHash)
    {
        // Set correct hash
        jsonObj["hash"] = newHash;

        // Write JSON to file
        std::ofstream file(filepath);
        if (!file.is_open())
        {
            return;
        }
        file << std::setw(4) << jsonObj << std::endl;
        file.close();

        // loadTimeHash needs to be updated
        loadTimeHash = newHash;
    }
}

uint64_t SettingsFile::simpleHash(const std::string& str)
{
    uint64_t hash = 0;
    size_t strSize = str.size();
    for (size_t i = 0; i < strSize; i++)
    {
        hash = hash * 31 + str[i];
    }
    return hash;
}