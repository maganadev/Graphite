#include "GameplaySceneManager.hpp"
#include "TJACourse.hpp"
#include "TimingOSSingletons.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

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
    // Set the song name
    // Set the course
    Globals::songName = "Song.json";
    Globals::courseDifficulty = "Oni";

    // Open the song
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

    // Find the matching course
    TJACourse* currentCourse = nullptr;
    for (auto& c : courses)
    {
        if (c.name == Globals::courseDifficulty)
        {
            currentCourse = &c;
            break;
        }
    }

    if (!currentCourse)
    {
        UtilityFunctions::print("Course not found: ", Globals::courseDifficulty.c_str());
        return;
    }

    // Spawn RedNotes for each note event in the course
    redNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/RedNote.tscn");
    if (redNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load RedNote scene");
        return;
    }

    for (const auto& event : currentCourse->events)
    {
        if (event.type != "red" && event.type != "blue")
        {
            continue;
        }

        Node* instance = redNoteScene->instantiate();
        RedNote* note = Object::cast_to<RedNote>(instance);
        if (note)
        {
            note->setEvent(event);
            add_child(note);
        }
    }

    UtilityFunctions::print("Spawned RedNotes for course: ", currentCourse->name.c_str());

    // Load the wave file
    std::string wavePath = Globals::songJson.value("wave", "");
    if (wavePath.empty())
    {
        UtilityFunctions::print("No wave file specified in song JSON");
        return;
    }
    if (!Globals::audioEngine->createAudioTrackBlocking(wavePath, 0.25, audioTrackHandle))
    {
        UtilityFunctions::print("Failed to load audio track: ", wavePath.c_str());
        return;
    }

    // Play the audio track
    Globals::audioEngine->playAudioTrack(audioTrackHandle);
    Globals::audioEngine->setTimedAudioTrack(audioTrackHandle);

    UtilityFunctions::print(
        "Loaded song: ", Globals::songJson.value("title", "unknown").c_str(),
        " | Course: ", currentCourse->name.c_str(), " | Level: ", std::to_string(currentCourse->level).c_str(),
        " | Events: ", std::to_string(currentCourse->events.size()).c_str(), " | Wave: ", wavePath.c_str());
}

void GameplaySceneManager::_process(double delta)
{
    uint64_t cpuTimePs = TimingOSSingletons::cpuTimer.GetValue();

    int64_t trackPositionPs;
    uint64_t outHandle;
    if (Globals::audioEngine->getPositionForAudioTrack(cpuTimePs, trackPositionPs, outHandle))
    {
        for (int i = 0; i < get_child_count(); i++)
        {
            RedNote* note = Object::cast_to<RedNote>(get_child(i));
            if (note)
            {
                note->updatePosition(trackPositionPs, scrollSpeed, laneY);
            }
        }
    }
}

void GameplaySceneManager::set_red_note_scene(Ref<PackedScene> scene)
{
    redNoteScene = scene;
}

Ref<PackedScene> GameplaySceneManager::get_red_note_scene() const
{
    return redNoteScene;
}
