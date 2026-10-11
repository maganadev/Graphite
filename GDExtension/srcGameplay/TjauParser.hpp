#ifndef TjauParser_hpp
#define TjauParser_hpp

#include "Chart.hpp"
#include <string>
#include <vector>

// Lightweight metadata extracted from a .tjau document without building notes.
struct TjauMetadataCourse
{
    int courseId;
    int level;
};

struct TjauMetadata
{
    std::string title;
    std::string subtitle;
    std::vector<TjauMetadataCourse> courses;
};

// Parse a full .tjau document into a Chart. Returns false on malformed input.
// outError receives a human readable reason when parsing fails.
bool parseTjauContent(const std::string& content, Chart& outChart, std::string& outError);

// Load a .tjau file and parse it into a Chart.
bool parseTjauFile(const std::string& filePath, Chart& outChart, std::string& outError);

// Lightweight pass that reads only title/subtitle/courses without building notes.
bool readTjauMetadataContent(const std::string& content, TjauMetadata& outMeta, std::string& outError);

// Load a .tjau file and read only its metadata.
bool readTjauMetadataFile(const std::string& filePath, TjauMetadata& outMeta, std::string& outError);

#endif