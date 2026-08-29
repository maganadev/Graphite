#include "GameplaySceneManager.hpp"
#include "TJACourse.hpp"
#include "TimingOSSingletons.hpp"
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void GameplaySceneManager::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_red_note_scene", "scene"), &GameplaySceneManager::set_red_note_scene);
    ClassDB::bind_method(D_METHOD("get_red_note_scene"), &GameplaySceneManager::get_red_note_scene);
    ClassDB::bind_method(D_METHOD("set_blue_note_scene", "scene"), &GameplaySceneManager::set_blue_note_scene);
    ClassDB::bind_method(D_METHOD("get_blue_note_scene"), &GameplaySceneManager::get_blue_note_scene);
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
    GameManager::songName = "Song.json";
    GameManager::courseDifficulty = "Oni";

    // Open the song
    std::ifstream ifs(GameManager::songName);
    if (!ifs.is_open())
    {
        UtilityFunctions::print("Failed to open song file: ", GameManager::songName.c_str());
        return;
    }
    GameManager::songJson = json::parse(ifs);
    std::vector<TJACourse> courses;
    TJACourse* currentCourse = nullptr;
    for (const auto& c : GameManager::songJson["courses"])
    {
        auto& course = courses.emplace_back(TJACourse::FromJson(c));
        if (course.name == GameManager::courseDifficulty)
        {
            currentCourse = &course;
        }
    }

    if (!currentCourse)
    {
        UtilityFunctions::print("Course not found: ", GameManager::courseDifficulty.c_str());
        return;
    }

    m_course = *currentCourse;
    visualOffsetPicoseconds = m_course.offset_picoseconds;
    UtilityFunctions::print("Visual offset: ", std::to_string(visualOffsetPicoseconds).c_str(), " ps");

    // Spawn notes for each note event in the course
    redNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/RedNote.tscn");
    if (redNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load RedNote scene");
        return;
    }

    blueNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/BlueNote.tscn");
    if (blueNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load BlueNote scene");
        return;
    }

    for (const auto& event : m_course.events)
    {
        Ref<PackedScene> noteScene;
        if (event.type == "red")
        {
            noteScene = redNoteScene;
        }
        else if (event.type == "blue")
        {
            noteScene = blueNoteScene;
        }
        else
        {
            continue;
        }

        Node* instance = noteScene->instantiate();
        RedNote* note = Object::cast_to<RedNote>(instance);
        if (note)
        {
            note->setEvent(event);
            note->set_z_index(3);
            add_child(note);
            m_course.spawnedNotes.push_back(note);
        }
    }

    UtilityFunctions::print("Spawned notes for course: ", m_course.name.c_str());

    std::string wavePath = GameManager::songJson.value("wave", "");
    if (wavePath.empty())
    {
        UtilityFunctions::print("No wave file specified in song JSON");
        return;
    }

    // Load the wave file
    if (!GameManager::audioEngine->createAudioTrackBlocking(wavePath, -24, audioTrackHandle))
    {
        UtilityFunctions::print("Failed to load audio track: ", wavePath.c_str());
        return;
    }

    // Play the audio track
    GameManager::audioEngine->playAudioTrack(audioTrackHandle);
    GameManager::audioEngine->setTimedAudioTrack(audioTrackHandle);

    UtilityFunctions::print("Loaded song: ", GameManager::songJson.value("title", "unknown").c_str(), " | Course: ", m_course.name.c_str(), " | Level: ", std::to_string(m_course.level).c_str(), " | Events: ", std::to_string(m_course.events.size()).c_str(), " | Wave: ", wavePath.c_str());
}

void GameplaySceneManager::_exit_tree()
{
    if (audioTrackHandle != 0)
    {
        GameManager::audioEngine->stopAudioTrack(audioTrackHandle);
        GameManager::audioEngine->freeAudioTrackBlocking(audioTrackHandle);
        audioTrackHandle = 0;
    }
}

void GameplaySceneManager::_process(double delta)
{
    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::Back].timesPressedSinceLastFrame > 0)
    {
        UtilityFunctions::print("Back action detected, loading DebugLauncherScene");
        get_tree()->change_scene_to_file("res://Scenes/DebugLauncherScene.tscn");
        return;
    }

    uint64_t cpuTimePs = TimingOSSingletons::cpuTimer.GetValue();

    int64_t trackPositionPs;
    uint64_t outHandle;
    if (GameManager::audioEngine->getPositionForAudioTrack(cpuTimePs, trackPositionPs, outHandle))
    {
        for (RedNote* note : m_course.spawnedNotes)
        {
            note->updatePosition(trackPositionPs, visualOffsetPicoseconds);
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

void GameplaySceneManager::set_blue_note_scene(Ref<PackedScene> scene)
{
    blueNoteScene = scene;
}

Ref<PackedScene> GameplaySceneManager::get_blue_note_scene() const
{
    return blueNoteScene;
}
