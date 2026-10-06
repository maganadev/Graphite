#ifndef SongDatabase_hpp
#define SongDatabase_hpp

#include "MenuCache.hpp"
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct SongEntry
{
    std::string title;
    std::string artist;
    std::string folderName;
    int folderIndex;
    uint8_t minLevel;
    uint16_t availableLevels;
    std::string chartPath;
};

struct FolderEntry
{
    std::string name;
    std::vector<int> songIndices;
    uint16_t availableLevels;
};

// A .tja chart found while walking the songs directory, recorded in scan order so
// that the parallel conversion pass and the metadata pass stay deterministic.
struct PendingChart
{
    std::filesystem::path tjaFile;
    int folderIndex;
};

class SongDatabase
{
public:
    std::vector<FolderEntry> folders;
    std::vector<SongEntry> songs;
    std::filesystem::path songsPath;

    // Path to the TJAtoTJAP executable, relative to the working directory.
    std::filesystem::path tjaParserPath{"GodotProject/TJAtoTJAP/TJAtoTJAP.exe"};

    // Load from cache if valid, otherwise scan from scratch.
    // Returns true on success.
    bool loadOrBuild(const std::filesystem::path& songsDirectory);

    // Force a full rescan.
    void rebuild();

    // Get all song indices.
    std::vector<int> getAllSongs();

    // Get song indices for a specific folder.
    std::vector<int> getFolderSongs(int folderIndex);

    // Get songs from `subset` that have the given level.
    std::vector<int> getSongsByLevel(const std::vector<int>& subset, uint8_t level);

    // Get level group labels available in `subset` (descending: Level 10..1).
    std::vector<std::string> getLevelGroups(const std::vector<int>& subset);

    // Get unique title group labels from `subset`.
    std::vector<std::string> getTitleGroups(const std::vector<int>& subset);

    // Get unique artist group labels from `subset`.
    std::vector<std::string> getArtistGroups(const std::vector<int>& subset);

    // Get songs from `subset` that match the given group key.
    // The group key is a lowercase comparison against title or artist.
    std::vector<int> getSongsByGroup(const std::vector<int>& subset, const std::string& groupKey);

private:
    std::filesystem::path cachePath;

    // Phase 1: record every .tja chart under a folder, in scan order.
    void collectCharts(const std::filesystem::path& dir, int folderIndex, std::vector<PendingChart>& charts);

    // Phase 2: convert every collected chart to .json, using all available cores.
    void convertCharts(const std::vector<PendingChart>& charts);

    // Phase 3: read a chart's .json metadata and add it to the database.
    void addChartFromJson(const PendingChart& chart);

    // Invoke TJAParser on a single chart, unless a newer .json already exists.
    // Safe to call from multiple threads at once.
    static bool runTjaParser(const std::filesystem::path& parserExe, const std::filesystem::path& tjaFile);

    // Read a chart's title, artist and level mask from its .json in one pass.
    // TJAParser emits the compact single-letter keys (see JsonKeys.hpp).
    // Returns false if the document is unreadable or has no title.
    static bool readChartMetadata(const std::filesystem::path& jsonFile, std::string& outTitle, std::string& outArtist, uint16_t& outLevels);
};

#endif
