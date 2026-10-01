#include "MenuCache.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

static void hashUpdate(uint64_t& hash, const uint8_t* data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        hash ^= data[i];
        hash *= 0x100000001B3ull;
    }
}

static void hashU64(uint64_t& hash, uint64_t v)
{
    hashUpdate(hash, reinterpret_cast<const uint8_t*>(&v), sizeof(v));
}

void MenuCache::computeFingerprint(const std::filesystem::path& songsDir,
                                    std::vector<uint8_t>& outFingerprint)
{
    uint64_t hash = 0xCBF29CE484222325ull;
    std::error_code ec;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(
             songsDir, std::filesystem::directory_options::skip_permission_denied, ec))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        std::string name = entry.path().filename().string();
        if (name == "song_cache.bin")
        {
            continue;
        }

        std::string rel = entry.path().string();
        hashUpdate(hash, reinterpret_cast<const uint8_t*>(rel.data()), rel.size());
        hashU64(hash, entry.file_size());

        auto mtime = entry.last_write_time();
        int64_t ft = mtime.time_since_epoch().count();
        hashU64(hash, static_cast<uint64_t>(ft));
    }

    outFingerprint.resize(8);
    for (int i = 0; i < 8; i++)
    {
        outFingerprint[i] = static_cast<uint8_t>((hash >> (i * 8)) & 0xFF);
    }
}

static bool readExact(std::ifstream& file, uint8_t* buf, size_t len)
{
    return file.read(reinterpret_cast<char*>(buf), static_cast<std::streamsize>(len)).gcount() == static_cast<std::streamsize>(len);
}

static uint32_t readU32(std::ifstream& file)
{
    uint32_t v = 0;
    readExact(file, reinterpret_cast<uint8_t*>(&v), sizeof(v));
    return v;
}

static uint16_t readU16(std::ifstream& file)
{
    uint16_t v = 0;
    readExact(file, reinterpret_cast<uint8_t*>(&v), sizeof(v));
    return v;
}

static uint8_t readU8(std::ifstream& file)
{
    uint8_t v = 0;
    readExact(file, &v, 1);
    return v;
}

bool MenuCache::isCacheValid(const std::filesystem::path& cachePath,
                              const std::filesystem::path& songsDir,
                              std::vector<uint8_t>& outFingerprint)
{
    // Build current fingerprint
    computeFingerprint(songsDir, outFingerprint);

    // Check if cache file exists
    if (!std::filesystem::exists(cachePath))
    {
        return false;
    }

    // Try to load header + fingerprint
    std::ifstream file(cachePath, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    uint32_t magic = readU32(file);
    if (magic != MAGIC)
    {
        return false;
    }

    uint32_t version = readU32(file);
    if (version != VERSION)
    {
        return false;
    }

    uint32_t fpLen = readU32(file);
    if (fpLen != outFingerprint.size())
    {
        return false;
    }

    std::vector<uint8_t> diskFingerprint(fpLen);
    if (fpLen > 0)
    {
        readExact(file, diskFingerprint.data(), fpLen);
    }

    return diskFingerprint == outFingerprint;
}

bool MenuCache::load(const std::filesystem::path& cachePath,
                      std::vector<uint8_t>& outFingerprint,
                      CachedData& outData)
{
    std::ifstream file(cachePath, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    uint32_t magic = readU32(file);
    if (magic != MAGIC)
    {
        return false;
    }

    uint32_t version = readU32(file);
    if (version != VERSION)
    {
        return false;
    }

    uint32_t fpLen = readU32(file);
    outFingerprint.resize(fpLen);
    if (fpLen > 0)
    {
        readExact(file, outFingerprint.data(), fpLen);
    }

    uint32_t folderCount = readU32(file);
    uint32_t songCount = readU32(file);
    uint32_t stringDataSize = readU32(file);
    uint32_t stringCount = readU32(file);

    std::vector<uint32_t> stringOffsets(stringCount + 1);
    readExact(file, reinterpret_cast<uint8_t*>(stringOffsets.data()),
              (stringCount + 1) * sizeof(uint32_t));

    std::vector<uint8_t> rawStrings(stringDataSize);
    if (stringDataSize > 0)
    {
        readExact(file, rawStrings.data(), stringDataSize);
    }

    outData.stringTable.resize(stringCount);
    for (uint32_t i = 0; i < stringCount; i++)
    {
        uint32_t start = stringOffsets[i];
        uint32_t end = stringOffsets[i + 1];
        outData.stringTable[i] = std::string(
            reinterpret_cast<const char*>(rawStrings.data()) + start,
            end - start);
    }

    outData.folders.resize(folderCount);
    for (uint32_t i = 0; i < folderCount; i++)
    {
        outData.folders[i].nameIndex = readU32(file);
        outData.folders[i].songStart = readU32(file);
        outData.folders[i].songCount = readU32(file);
        outData.folders[i].availableLevels = readU16(file);
    }

    outData.songs.resize(songCount);
    for (uint32_t i = 0; i < songCount; i++)
    {
        outData.songs[i].titleIndex = readU32(file);
        outData.songs[i].artistIndex = readU32(file);
        outData.songs[i].folderIndex = readU32(file);
        outData.songs[i].minLevel = readU8(file);
        outData.songs[i].availableLevels = readU16(file);
        outData.songs[i].chartPathIndex = readU32(file);
    }

    return true;
}

static void writeU32(std::ofstream& file, uint32_t v)
{
    file.write(reinterpret_cast<const char*>(&v), sizeof(v));
}

static void writeU16(std::ofstream& file, uint16_t v)
{
    file.write(reinterpret_cast<const char*>(&v), sizeof(v));
}

static void writeU8(std::ofstream& file, uint8_t v)
{
    file.write(reinterpret_cast<const char*>(&v), sizeof(v));
}

void MenuCache::save(const std::filesystem::path& cachePath,
                      const std::vector<uint8_t>& fingerprint,
                      const CachedData& data)
{
    // Build string table offsets
    std::vector<uint32_t> stringOffsets;
    std::vector<uint8_t> rawStrings;
    stringOffsets.reserve(data.stringTable.size() + 1);
    rawStrings.reserve(4096);

    uint32_t offset = 0;
    for (const std::string& s : data.stringTable)
    {
        stringOffsets.push_back(offset);
        rawStrings.insert(rawStrings.end(), s.begin(), s.end());
        offset += static_cast<uint32_t>(s.size());
    }
    stringOffsets.push_back(offset);

    std::string tempPath = cachePath.string() + ".tmp";
    std::ofstream file(tempPath, std::ios::binary);
    if (!file.is_open())
    {
        return;
    }

    writeU32(file, MAGIC);
    writeU32(file, VERSION);

    writeU32(file, static_cast<uint32_t>(fingerprint.size()));
    if (!fingerprint.empty())
    {
        file.write(reinterpret_cast<const char*>(fingerprint.data()),
                   static_cast<std::streamsize>(fingerprint.size()));
    }

    writeU32(file, static_cast<uint32_t>(data.folders.size()));
    writeU32(file, static_cast<uint32_t>(data.songs.size()));

    uint32_t stringDataSize = static_cast<uint32_t>(rawStrings.size());
    writeU32(file, stringDataSize);
    writeU32(file, static_cast<uint32_t>(data.stringTable.size()));

    file.write(reinterpret_cast<const char*>(stringOffsets.data()),
               static_cast<std::streamsize>(stringOffsets.size() * sizeof(uint32_t)));
    if (stringDataSize > 0)
    {
        file.write(reinterpret_cast<const char*>(rawStrings.data()),
                   static_cast<std::streamsize>(stringDataSize));
    }

    for (const CachedFolder& f : data.folders)
    {
        writeU32(file, f.nameIndex);
        writeU32(file, f.songStart);
        writeU32(file, f.songCount);
        writeU16(file, f.availableLevels);
    }

    for (const CachedSong& s : data.songs)
    {
        writeU32(file, s.titleIndex);
        writeU32(file, s.artistIndex);
        writeU32(file, s.folderIndex);
        writeU8(file, s.minLevel);
        writeU16(file, s.availableLevels);
        writeU32(file, s.chartPathIndex);
    }

    file.close();

    // Atomic replace
    std::error_code ec;
    std::filesystem::rename(tempPath, cachePath, ec);
    if (ec)
    {
        // If rename failed, try remove and rename
        std::filesystem::remove(cachePath, ec);
        std::filesystem::rename(tempPath, cachePath, ec);
    }
}