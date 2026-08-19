#include "GameplaySceneManager.hpp"
#include "TJACourse.hpp"

#include <godot_cpp/classes/utility_functions.hpp>

void GameplaySceneManager::_bind_methods()
{
    //
}

GameplaySceneManager::GameplaySceneManager()
{
    //
}

GameplaySceneManager::~GameplaySceneManager()
{
    //
}

void GameplaySceneManager::_ready()
{
    //Set the song name
    //Set the course
    Globals::songName = "Song.json";
    Globals::courseDifficulty = "Oni";

    //Open the song
    std::ifstream ifs(Globals::songName);
    if (!ifs.is_open())
    {
        UtilityFunctions::print("Failed to open song file: ", Globals::songName.c_str());
        return;
    }
    Globals::songJson = json::parse(ifs);
    std::vector<TJACourse> courses;
    for (const auto& c : Globals::songJson["courses"])
    {
        courses.push_back(TJACourse::FromJson(c));
    }

    //Find the matching course
    TJACourse* currentCourse = nullptr;
    for (auto& c : courses)
    {
        if (c.name == Globals::courseDifficulty)
        {
            currentCourse = &c;
            break;
        }
    }

    if (currentCourse)
    {
        UtilityFunctions::print(
            "Loaded song: ", Globals::songJson.value("title", "unknown").c_str(),
            " | Course: ", currentCourse->name.c_str(),
            " | Level: ", std::to_string(currentCourse->level).c_str(),
            " | Events: ", std::to_string(currentCourse->events.size()).c_str()
        );
    }
    else
    {
        UtilityFunctions::print("Course not found: ", Globals::courseDifficulty.c_str());
    }
}

void GameplaySceneManager::_process(double delta)
{
    //
}

GameplaySceneManager::GameplaySceneManager()
{
    //
}

GameplaySceneManager::~GameplaySceneManager()
{
    //
}

void GameplaySceneManager::_ready()
{
//Set the song name
//Set the course
Globals::songName = "Song.json";
Globals::courseDifficulty = "Oni";

//Open the song
Globals::songJson = json::parse(file);

//Convert the json course to C++ object course


    if (file.is_open() && file.good())
    {

        std::string title = Globals::songJson.value("title", "unknown");
        std::string courseName = Globals::songJson["courses"][0].value("name", "unknown");
        int level = Globals::songJson["courses"][0].value("level", 0);
        UtilityFunctions::print("Loaded song: ", title.c_str(), " | Course: ", courseName.c_str(), " | Level: ", level);
    }
}

void GameplaySceneManager::_process(double delta)
{
    //
}
