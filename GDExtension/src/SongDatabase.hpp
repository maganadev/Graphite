#ifndef SongDatabase_hpp
#define SongDatabase_hpp

#include "MenuCache.hpp"
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct CourseEntry
{
    std::string name;
    uint8_t level;
    int courseId;
};

struct SongEntry
{
    std::string title;
    std::string artist;
    std::string folderName;
    int folderIndex;
    uint8_t minLevel;
    uint16_t availableLevels;
    std::vector<CourseEntry> courses;
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

    // Path to the TJAtoTJAU executable, relative to the working directory.
    std::filesystem::path tjaToTjauPath{"TJAtoTJAU/TJAtoTJAU.exe"};

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

    // Get the levels available in `subset` (descending: 10..1).
    std::vector<int> getLevels(const std::vector<int>& subset);

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

    // Phase 2: pre-process every collected chart into a UTF-8 .tjau file, using all available cores.
    void convertCharts(const std::vector<PendingChart>& charts);

    // Phase 3: read a chart's .tjau metadata and add it to the database.
    void addChartFromTjau(const PendingChart& chart);

    // Invoke TJAtoTJAU on a single chart, unless a newer .tjau already exists.
    // Safe to call from multiple threads at once.
    static bool runTjauConverter(const std::filesystem::path& converterExe, const std::filesystem::path& tjaFile);

    // Read a chart's title, artist, level mask and course data from its .tjau in one pass.
    // Returns false if the document is unreadable or has no title.
    static bool readTjauMetadata(const std::filesystem::path& tjauFile, std::string& outTitle, std::string& outArtist, uint16_t& outLevels, std::vector<CourseEntry>& outCourses);
};

#endif
