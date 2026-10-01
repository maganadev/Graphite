#include "SongDatabase.hpp"
#include "MenuCache.hpp"
#include "../srcThirdParty/json.hpp"
#include <cstdint>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

using json = nlohmann::json;

static std::string trim(const std::string& s)
{
    size_t start = 0;
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r'))
    {
        start++;
    }
    size_t end = s.size();
    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r'))
    {
        end--;
    }
    return s.substr(start, end - start);
}

static std::string lowercaseAscii(const std::string& s)
{
    std::string r(s.size(), '\0');
    for (size_t i = 0; i < s.size(); i++)
    {
        char c = s[i];
        r[i] = (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
    }
    return r;
}

static std::filesystem::path jsonForTja(const std::filesystem::path& tjaFile)
{
    std::filesystem::path p = tjaFile;
    return p.replace_extension(".json");
}

bool SongDatabase::ensureJsonForTja(const std::filesystem::path& tjaFile)
{
    std::filesystem::path jsonFile = jsonForTja(tjaFile);

    // If the .json already exists and is newer than the .tja, skip conversion.
    if (std::filesystem::exists(jsonFile))
    {
        auto tjaTime = std::filesystem::last_write_time(tjaFile);
        auto jsonTime = std::filesystem::last_write_time(jsonFile);
        if (jsonTime >= tjaTime)
        {
            return true;
        }
    }

    // Run TJAParser to convert .tja -> .json, without showing a window.
#ifdef _WIN32
    std::string exePath = tjaParserPath.string();
    std::string cmdLine = "\"" + exePath + "\" \"" + tjaFile.string() + "\"";
    std::vector<char> cmdBuf(cmdLine.begin(), cmdLine.end());
    cmdBuf.push_back('\0');

    STARTUPINFO si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi{};

    if (!CreateProcess(exePath.c_str(), cmdBuf.data(), nullptr, nullptr,
                       FALSE, 0, nullptr, nullptr, &si, &pi))
    {
        return false;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
#else
    std::string cmdLine = "\"" + tjaParserPath.string() + "\" \"" + tjaFile.string() + "\" > /dev/null 2>&1";
    if (std::system(cmdLine.c_str()) != 0)
    {
        return false;
    }
#endif

    return std::filesystem::exists(jsonFile);
}

std::string SongDatabase::parseTitleFromJson(const std::filesystem::path& jsonFile)
{
    std::ifstream file(jsonFile);
    if (!file.is_open())
    {
        return "";
    }
    json j = json::parse(file);
    return j.value("title", "");
}

std::string SongDatabase::parseArtistFromJson(const std::filesystem::path& jsonFile)
{
    std::ifstream file(jsonFile);
    if (!file.is_open())
    {
        return "";
    }
    json j = json::parse(file);
    std::string subtitle = j.value("subtitle", "");
    if (subtitle.find("--") == 0)
    {
        subtitle = subtitle.substr(2);
    }
    return subtitle;
}

uint16_t SongDatabase::parseLevelsFromJson(const std::filesystem::path& jsonFile)
{
    uint16_t levels = 0;
    std::ifstream file(jsonFile);
    if (!file.is_open())
    {
        return levels;
    }
    json j = json::parse(file);
    if (j.contains("courses") && j["courses"].is_array())
    {
        for (const auto& c : j["courses"])
        {
            int level = c.value("level", 0);
            if (level >= 1 && level <= 10)
            {
                levels |= static_cast<uint16_t>(1 << (level - 1));
            }
        }
    }
    return levels;
}

bool SongDatabase::loadOrBuild(const std::filesystem::path& songsDirectory)
{
    songsPath = songsDirectory;
    cachePath = songsDirectory / "song_cache.bin";

    std::vector<uint8_t> fingerprint;
    if (MenuCache::isCacheValid(cachePath, songsDirectory, fingerprint))
    {
        CachedData cached;
        if (MenuCache::load(cachePath, fingerprint, cached))
        {
            folders.resize(cached.folders.size());
            for (size_t i = 0; i < cached.folders.size(); i++)
            {
                folders[i].name = cached.stringTable[cached.folders[i].nameIndex];
                folders[i].availableLevels = cached.folders[i].availableLevels;
            }

            songs.resize(cached.songs.size());
            for (size_t i = 0; i < cached.songs.size(); i++)
            {
                songs[i].title = cached.stringTable[cached.songs[i].titleIndex];
                songs[i].artist = cached.stringTable[cached.songs[i].artistIndex];
                songs[i].folderName = cached.stringTable[cached.songs[i].folderIndex];
                songs[i].folderIndex = static_cast<int>(cached.songs[i].folderIndex);
                songs[i].minLevel = cached.songs[i].minLevel;
                songs[i].availableLevels = cached.songs[i].availableLevels;
                songs[i].chartPath = cached.stringTable[cached.songs[i].chartPathIndex];
            }

            for (size_t si = 0; si < songs.size(); si++)
            {
                int fi = songs[si].folderIndex;
                if (fi >= 0 && fi < static_cast<int>(folders.size()))
                {
                    folders[fi].songIndices.push_back(static_cast<int>(si));
                }
            }

            return true;
        }
    }

    rebuild();
    return true;
}

void SongDatabase::rebuild()
{
    songs.clear();
    folders.clear();

    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(songsPath, ec))
    {
        if (!entry.is_directory())
        {
            continue;
        }

        std::string folderName = entry.path().filename().string();
        int folderIndex = static_cast<int>(folders.size());

        FolderEntry folder;
        folder.name = folderName;
        folder.availableLevels = 0;
        folders.push_back(folder);

        scanDirectory(entry.path(), folderName, folderIndex);
    }

    std::vector<uint8_t> fingerprint;
    MenuCache::computeFingerprint(songsPath, fingerprint);

    CachedData cached;
    std::unordered_map<std::string, uint32_t> stringIndex;
    auto getOrAddString = [&](const std::string& s) -> uint32_t
    {
        auto it = stringIndex.find(s);
        if (it != stringIndex.end())
        {
            return it->second;
        }
        uint32_t idx = static_cast<uint32_t>(cached.stringTable.size());
        cached.stringTable.push_back(s);
        stringIndex[s] = idx;
        return idx;
    };

    cached.folders.reserve(folders.size());
    for (const FolderEntry& f : folders)
    {
        CachedFolder cf;
        cf.nameIndex = getOrAddString(f.name);
        cf.songStart = 0;
        cf.songCount = static_cast<uint32_t>(f.songIndices.size());
        cf.availableLevels = f.availableLevels;
        cached.folders.push_back(cf);
    }

    cached.songs.reserve(songs.size());
    for (const SongEntry& s : songs)
    {
        CachedSong cs;
        cs.titleIndex = getOrAddString(s.title);
        cs.artistIndex = getOrAddString(s.artist);
        cs.folderIndex = getOrAddString(s.folderName);
        cs.minLevel = s.minLevel;
        cs.availableLevels = s.availableLevels;
        cs.chartPathIndex = getOrAddString(s.chartPath);
        cached.songs.push_back(cs);
    }

    MenuCache::save(cachePath, fingerprint, cached);
}

void SongDatabase::scanDirectory(const std::filesystem::path& dir,
                                  const std::string& folderName,
                                  int folderIndex)
{
    std::error_code ec;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir, ec))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        std::filesystem::path filePath = entry.path();
        std::string ext = filePath.extension().string();
        if (ext != ".tja" && ext != ".TJA")
        {
            continue;
        }

        if (!ensureJsonForTja(filePath))
        {
            continue;
        }

        std::filesystem::path jsonFile = jsonForTja(filePath);
        std::string title = parseTitleFromJson(jsonFile);
        if (title.empty())
        {
            continue;
        }

        std::string artist = parseArtistFromJson(jsonFile);
        uint16_t levels = parseLevelsFromJson(jsonFile);
        if (levels == 0)
        {
            continue;
        }

        uint8_t minLevel = 10;
        for (int l = 1; l <= 10; l++)
        {
            if (levels & (1 << (l - 1)))
            {
                minLevel = static_cast<uint8_t>(l);
                break;
            }
        }

        SongEntry song;
        song.title = title;
        song.artist = artist;
        song.folderName = folderName;
        song.folderIndex = folderIndex;
        song.minLevel = minLevel;
        song.availableLevels = levels;
        song.chartPath = jsonFile.string();

        int songIndex = static_cast<int>(songs.size());
        songs.push_back(song);
        folders[folderIndex].songIndices.push_back(songIndex);
        folders[folderIndex].availableLevels |= levels;
    }
}

std::vector<int> SongDatabase::getAllSongs()
{
    std::vector<int> result;
    result.reserve(songs.size());
    for (int i = 0; i < static_cast<int>(songs.size()); i++)
    {
        result.push_back(i);
    }
    return result;
}

std::vector<int> SongDatabase::getFolderSongs(int folderIndex)
{
    if (folderIndex < 0 || folderIndex >= static_cast<int>(folders.size()))
    {
        return {};
    }
    return folders[folderIndex].songIndices;
}

std::vector<int> SongDatabase::getSongsByLevel(const std::vector<int>& subset, uint8_t level)
{
    std::vector<int> result;
    uint16_t mask = static_cast<uint16_t>(1 << (level - 1));
    for (int idx : subset)
    {
        if (idx >= 0 && idx < static_cast<int>(songs.size()) && (songs[idx].availableLevels & mask))
        {
            result.push_back(idx);
        }
    }
    return result;
}

std::vector<int> SongDatabase::getSongsByGroup(const std::vector<int>& subset,
                                                const std::string& groupKey)
{
    std::vector<int> result;
    std::string keyLower = lowercaseAscii(groupKey);
    for (int idx : subset)
    {
        if (idx >= 0 && idx < static_cast<int>(songs.size()))
        {
            if (lowercaseAscii(songs[idx].title) == keyLower ||
                lowercaseAscii(songs[idx].artist) == keyLower)
            {
                result.push_back(idx);
            }
        }
    }
    return result;
}

std::vector<std::string> SongDatabase::getLevelGroups(const std::vector<int>& subset)
{
    uint16_t levels = 0;
    for (int idx : subset)
    {
        if (idx >= 0 && idx < static_cast<int>(songs.size()))
        {
            levels |= songs[idx].availableLevels;
        }
    }
    std::vector<std::string> groups;
    for (int l = 10; l >= 1; l--)
    {
        if (levels & (1 << (l - 1)))
        {
            groups.push_back("Level " + std::to_string(l));
        }
    }
    return groups;
}

std::vector<std::string> SongDatabase::getTitleGroups(const std::vector<int>& subset)
{
    std::vector<std::string> groups;
    std::vector<std::string> seen;
    for (int idx : subset)
    {
        if (idx >= 0 && idx < static_cast<int>(songs.size()))
        {
            std::string key = lowercaseAscii(songs[idx].title);
            if (!key.empty())
            {
                bool dup = false;
                for (const std::string& s : seen)
                {
                    if (s == key) { dup = true; break; }
                }
                if (!dup)
                {
                    seen.push_back(key);
                    groups.push_back(songs[idx].title);
                }
            }
        }
    }
    return groups;
}

std::vector<std::string> SongDatabase::getArtistGroups(const std::vector<int>& subset)
{
    std::vector<std::string> groups;
    std::vector<std::string> seen;
    for (int idx : subset)
    {
        if (idx >= 0 && idx < static_cast<int>(songs.size()))
        {
            std::string key = lowercaseAscii(songs[idx].artist);
            if (!key.empty())
            {
                bool dup = false;
                for (const std::string& s : seen)
                {
                    if (s == key) { dup = true; break; }
                }
                if (!dup)
                {
                    seen.push_back(key);
                    groups.push_back(songs[idx].artist);
                }
            }
        }
    }
    return groups;
}