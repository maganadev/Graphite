#ifndef MenuCache_hpp
#define MenuCache_hpp

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct CachedCourse
{
    uint32_t nameIndex;
    uint8_t level;
    uint16_t courseId;
};

struct CachedFolder
{
    uint32_t nameIndex;
    uint32_t songStart;
    uint32_t songCount;
    uint16_t availableLevels;
};

struct CachedSong
{
    uint32_t titleIndex;
    uint32_t artistIndex;
    uint32_t folderIndex;
    uint8_t minLevel;
    uint16_t availableLevels;
    uint32_t chartPathIndex;
    uint16_t courseCount;
    uint32_t courseStart;
};

struct CachedData
{
    std::vector<std::string> stringTable;
    std::vector<CachedFolder> folders;
    std::vector<CachedSong> songs;
    std::vector<CachedCourse> courses;
};

class MenuCache
{
public:
    static constexpr uint32_t MAGIC = 0x4D454E55;
    static constexpr uint32_t VERSION = 2;

    // Check if a valid cache exists by comparing fingerprints.
    // If fpValid is true the songs dir has not changed.
    // Sets outFingerprint to the current fingerprint.
    static bool isCacheValid(const std::filesystem::path& cachePath, const std::filesystem::path& songsDir, std::vector<uint8_t>& outFingerprint);

    // Load cached data from disk. Returns false on any error.
    // Does not check fingerprint validity.
    static bool load(const std::filesystem::path& cachePath, std::vector<uint8_t>& outFingerprint, CachedData& outData);

    // Write cache atomically (write to temp then rename).
    static void save(const std::filesystem::path& cachePath, const std::vector<uint8_t>& fingerprint, const CachedData& data);

    // Compute a fingerprint (hash of file paths, sizes, mtimes) for the
    // songs directory tree. Used to detect changes.
    static void computeFingerprint(const std::filesystem::path& songsDir, std::vector<uint8_t>& outFingerprint);
};

#endif