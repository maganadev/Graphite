#include "SongDatabase.hpp"
#include "../srcGameplay/JsonKeys.hpp"
#include "../srcThirdParty/json.hpp"
#include "MenuCache.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

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

static std::string courseIdToName(int id)
{
    switch (id)
    {
    case 0:
        return "Easy";
    case 1:
        return "Normal";
    case 2:
        return "Hard";
    case 3:
        return "Oni";
    case 4:
        return "Ura Oni";
    default:
        return "Course " + std::to_string(id);
    }
}

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
    std::filesystem::path dir = tjaFile.parent_path();
    std::string filename = tjaFile.stem().string() + ".tjap";
    return dir / filename;
}

bool SongDatabase::runTjaParser(const std::filesystem::path& parserExe, const std::filesystem::path& tjaFile)
{
    std::filesystem::path jsonFile = jsonForTja(tjaFile);

    // If the .tjap already exists and is newer than the .tja, skip conversion.
    // Uses the error_code overloads throughout: this runs on worker threads, and
    // the extension is built with exceptions disabled.
    std::error_code ec;
    auto jsonTime = std::filesystem::last_write_time(jsonFile, ec);
    if (!ec)
    {
        auto tjaTime = std::filesystem::last_write_time(tjaFile, ec);
        if (!ec && jsonTime >= tjaTime)
        {
            return true;
        }
    }

    // Run TJAParser to convert .tja -> .tjap, without showing a window.
#ifdef _WIN32
    std::string exePath = parserExe.string();
    std::string inPath = tjaFile.string();
    std::string outPath = jsonFile.string();
    std::string cmdLine = "\"" + exePath + "\" -i \"" + inPath + "\" -o \"" + outPath + "\"";
    std::vector<char> cmdBuf(cmdLine.begin(), cmdLine.end());
    cmdBuf.push_back('\0');

    STARTUPINFO si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi{};

    if (!CreateProcess(exePath.c_str(), cmdBuf.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
    {
        return false;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
#else
    std::string cmdLine = "\"" + parserExe.string() + "\" -i \"" + tjaFile.string() + "\" -o \"" + jsonFile.string() + "\" > /dev/null 2>&1";
    if (std::system(cmdLine.c_str()) != 0)
    {
        return false;
    }
#endif

    return std::filesystem::exists(jsonFile, ec);
}

bool SongDatabase::readChartMetadata(const std::filesystem::path& jsonFile, std::string& outTitle, std::string& outArtist, uint16_t& outLevels, std::vector<CourseEntry>& outCourses)
{
    std::ifstream file(jsonFile);
    if (!file.is_open())
    {
        return false;
    }

    json j = json::parse(file);

    outTitle = j.value(JC_TITLE, "");
    if (outTitle.empty())
    {
        return false;
    }

    std::string subtitle = j.value(JC_SUBTITLE, "");
    if (subtitle.find("--") == 0)
    {
        subtitle = subtitle.substr(2);
    }
    outArtist = subtitle;

    outLevels = 0;
    outCourses.clear();
    if (j.contains(JC_COURSES) && j[JC_COURSES].is_array())
    {
        for (const auto& c : j[JC_COURSES])
        {
            int courseId = c.value(JC_COURSE, 0);
            int level = c.value(JC_LEVEL, 0);
            if (level >= 1 && level <= 10)
            {
                outLevels |= static_cast<uint16_t>(1 << (level - 1));
                CourseEntry entry;
                entry.name = courseIdToName(courseId);
                entry.level = static_cast<uint8_t>(level);
                entry.courseId = courseId;
                outCourses.push_back(entry);
            }
        }
    }

    return true;
}

bool SongDatabase::loadOrBuild(const std::filesystem::path& songsDirectory)
{
    songsPath = songsDirectory;
    cachePath = songsDirectory / "song_cache.bin";

    std::vector<uint8_t> fingerprint;
    if (MenuCache::isCacheValid(cachePath, songsDirectory, fingerprint))
    {
        CachedData cached;
        if (MenuCache::load(cachePath, fingerprint, cached) && cached.songs.size() > 0)
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
                songs[i].courses.reserve(cached.songs[i].courseCount);
                for (uint16_t ci = 0; ci < cached.songs[i].courseCount; ci++)
                {
                    uint32_t idx = cached.songs[i].courseStart + ci;
                    CourseEntry ce;
                    ce.name = cached.stringTable[cached.courses[idx].nameIndex];
                    ce.level = cached.courses[idx].level;
                    ce.courseId = cached.courses[idx].courseId;
                    songs[i].courses.push_back(ce);
                }
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
    std::vector<PendingChart> charts;
    for (const auto& entry : std::filesystem::directory_iterator(songsPath, ec))
    {
        if (!entry.is_directory())
        {
            continue;
        }

        std::string folderName = entry.path().filename().string();

        FolderEntry folder;
        folder.name = folderName;
        folder.availableLevels = 0;
        folders.push_back(folder);

        collectCharts(entry.path(), static_cast<int>(folders.size()) - 1, charts);
    }

    convertCharts(charts);

    for (size_t i = 0; i < charts.size(); i++)
    {
        addChartFromJson(charts[i]);
    }

    if (songs.empty())
    {
        return;
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
    uint32_t courseWriteIndex = 0;
    for (const SongEntry& s : songs)
    {
        CachedSong cs;
        cs.titleIndex = getOrAddString(s.title);
        cs.artistIndex = getOrAddString(s.artist);
        cs.folderIndex = getOrAddString(s.folderName);
        cs.minLevel = s.minLevel;
        cs.availableLevels = s.availableLevels;
        cs.chartPathIndex = getOrAddString(s.chartPath);
        cs.courseCount = static_cast<uint16_t>(s.courses.size());
        cs.courseStart = courseWriteIndex;
        cached.songs.push_back(cs);

        for (const CourseEntry& ce : s.courses)
        {
            CachedCourse cc;
            cc.nameIndex = getOrAddString(ce.name);
            cc.level = ce.level;
            cc.courseId = static_cast<uint16_t>(ce.courseId);
            cached.courses.push_back(cc);
        }
        courseWriteIndex += static_cast<uint32_t>(s.courses.size());
    }

    MenuCache::save(cachePath, fingerprint, cached);
}

void SongDatabase::collectCharts(const std::filesystem::path& dir, int folderIndex, std::vector<PendingChart>& charts)
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

        charts.push_back({filePath, folderIndex});
    }
}

void SongDatabase::convertCharts(const std::vector<PendingChart>& charts)
{
    if (charts.empty())
    {
        return;
    }

    unsigned int workerCount = std::thread::hardware_concurrency();
    if (workerCount == 0)
    {
        workerCount = 1;
    }
    workerCount = std::min(workerCount, 32u);
    workerCount = std::min(static_cast<unsigned int>(charts.size()), workerCount);

    const std::filesystem::path parserExe = tjaParserPath;
    std::atomic<size_t> nextChart{0};

    // Each worker claims charts one at a time so that a slow chart doesn't strand a
    // whole fixed-size chunk.
    auto work = [&charts, &parserExe, &nextChart]()
    {
        for (size_t i = nextChart.fetch_add(1); i < charts.size(); i = nextChart.fetch_add(1))
        {
            runTjaParser(parserExe, charts[i].tjaFile);
        }
    };

    std::vector<std::thread> workers;
    workers.reserve(workerCount);
    for (unsigned int i = 0; i < workerCount; i++)
    {
        workers.emplace_back(work);
    }
    for (std::thread& t : workers)
    {
        t.join();
    }
}

void SongDatabase::addChartFromJson(const PendingChart& chart)
{
    std::filesystem::path jsonFile = jsonForTja(chart.tjaFile);
    if (!std::filesystem::exists(jsonFile))
    {
        return;
    }

    std::string title;
    std::string artist;
    uint16_t levels = 0;
    std::vector<CourseEntry> courses;
    if (!readChartMetadata(jsonFile, title, artist, levels, courses))
    {
        return;
    }
    if (levels == 0)
    {
        return;
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
    song.folderName = folders[chart.folderIndex].name;
    song.folderIndex = chart.folderIndex;
    song.minLevel = minLevel;
    song.availableLevels = levels;
    song.courses = std::move(courses);
    song.chartPath = jsonFile.string();

    int songIndex = static_cast<int>(songs.size());
    songs.push_back(song);
    folders[chart.folderIndex].songIndices.push_back(songIndex);
    folders[chart.folderIndex].availableLevels |= levels;
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

std::vector<int> SongDatabase::getSongsByGroup(const std::vector<int>& subset, const std::string& groupKey)
{
    std::vector<int> result;
    std::string keyLower = lowercaseAscii(groupKey);
    for (int idx : subset)
    {
        if (idx >= 0 && idx < static_cast<int>(songs.size()))
        {
            if (lowercaseAscii(songs[idx].title) == keyLower || lowercaseAscii(songs[idx].artist) == keyLower)
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
                    if (s == key)
                    {
                        dup = true;
                        break;
                    }
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
                    if (s == key)
                    {
                        dup = true;
                        break;
                    }
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