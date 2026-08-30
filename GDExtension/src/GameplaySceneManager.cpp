#include "GameplaySceneManager.hpp"
#include "BlueNote.hpp"
#include "BlueNotePrefab.hpp"
#include "Course.hpp"
#include "RedNote.hpp"
#include "RedNotePrefab.hpp"
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

NoteTypes GameplaySceneManager::noteTypeForEvent(const std::string& type) const
{
    if (type == "red")
        return NoteTypes::RedNoteSmall;
    if (type == "blue")
        return NoteTypes::BlueNoteSmall;
    if (type == "redBig")
        return NoteTypes::RedNoteLarge;
    if (type == "blueBig")
        return NoteTypes::BlueNoteLarge;
    return NoteTypes::RedNoteSmall;
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
    std::vector<Course> courses;
    Course* currentCourse = nullptr;
    for (const auto& c : GameManager::songJson["courses"])
    {
        auto& course = courses.emplace_back(Course::FromJson(c));
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

    int64_t unfilteredVisualOffset = GameManager::visualOffset;
    int64_t unfilteredAudioOffset = m_course.offset_picoseconds + GameManager::audioOffset;
    int64_t unfilteredJudgementOffset = 0;
    int64_t filteredVisualOffset = unfilteredVisualOffset - unfilteredAudioOffset;
    int64_t filteredAudioOffset = unfilteredAudioOffset - unfilteredAudioOffset;
    int64_t filteredJudgementOffset = unfilteredJudgementOffset - unfilteredAudioOffset;
    JudgementThread::judgementOffset.store(filteredJudgementOffset, std::memory_order_release);

    UtilityFunctions::print("Input Visual Offset: ", std::to_string(unfilteredVisualOffset).c_str(), " ps");
    UtilityFunctions::print("Input Audio Offset: ", std::to_string(unfilteredAudioOffset).c_str(), " ps");
    UtilityFunctions::print("Input Judgement Offset: ", std::to_string(unfilteredJudgementOffset).c_str(), " ps");
    UtilityFunctions::print("Output Visual Offset: ", std::to_string(filteredVisualOffset).c_str(), " ps");
    UtilityFunctions::print("Output Audio Offset: ", std::to_string(filteredAudioOffset).c_str(), " ps");
    UtilityFunctions::print("Output Judgement Offset: ", std::to_string(filteredJudgementOffset).c_str(), " ps");

    visualOffsetPicoseconds = filteredVisualOffset;

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

    for (const auto& noteEvent : m_course.notes)
    {
        NoteTypes noteType = noteTypeForEvent(noteEvent.type);

        if (noteType == NoteTypes::RedNoteSmall || noteType == NoteTypes::RedNoteLarge)
        {
            Node* instance = redNoteScene->instantiate();
            RedNotePrefab* prefab = Object::cast_to<RedNotePrefab>(instance);
            if (prefab)
            {
                RedNote* note = new RedNote();
                note->setNote(noteEvent);
                note->setPrefab(prefab);
                prefab->set_z_index(3);
                add_child(prefab);
                m_course.redNotes.push_back(note);
            }
        }
        else if (noteType == NoteTypes::BlueNoteSmall || noteType == NoteTypes::BlueNoteLarge)
        {
            Node* instance = blueNoteScene->instantiate();
            BlueNotePrefab* prefab = Object::cast_to<BlueNotePrefab>(instance);
            if (prefab)
            {
                BlueNote* note = new BlueNote();
                note->setNote(noteEvent);
                note->setPrefab(prefab);
                prefab->set_z_index(3);
                add_child(prefab);
                m_course.blueNotes.push_back(note);
            }
        }
    }

    m_course.populateLanes();
    GameManager::currentCourse = &m_course;

    UtilityFunctions::print("Spawned notes for course: ", m_course.name.c_str(), " | Red: ", std::to_string(m_course.redNotes.size()).c_str(), " Blue: ", std::to_string(m_course.blueNotes.size()).c_str());

    std::string wavePath = GameManager::songJson.value("wave", "");
    if (wavePath.empty())
    {
        UtilityFunctions::print("No wave file specified in song JSON");
        return;
    }

    // Load the wave file
    if (!GameManager::audioEngine.value().createAudioTrackBlocking(wavePath, -24, audioTrackHandle))
    {
        UtilityFunctions::print("Failed to load audio track: ", wavePath.c_str());
        return;
    }

    // Play the audio track
    GameManager::audioEngine.value().playAudioTrack(audioTrackHandle);
    GameManager::audioEngine.value().setTimedAudioTrack(audioTrackHandle);

    UtilityFunctions::print("Loaded song: ", GameManager::songJson.value("title", "unknown").c_str(), " | Course: ", m_course.name.c_str(), " | Level: ", std::to_string(m_course.level).c_str(), " | Events: ", std::to_string(m_course.notes.size()).c_str(), " | Wave: ", wavePath.c_str());
}

void GameplaySceneManager::_exit_tree()
{
    if (GameManager::currentCourse == &m_course)
    {
        GameManager::currentCourse = nullptr;
    }
    for (RedNote* note : m_course.redNotes)
    {
        delete note;
    }
    m_course.redNotes.clear();
    if (audioTrackHandle != 0)
    {
        GameManager::audioEngine.value().stopAudioTrack(audioTrackHandle);
        GameManager::audioEngine.value().freeAudioTrackBlocking(audioTrackHandle);
        audioTrackHandle = 0;
    }
}

void GameplaySceneManager::_process(double delta)
{
    // Get time
    uint64_t cpuTimePs = TimingOSSingletons::cpuTimer.GetValue();

    // Grade abandoned notes
    JudgementThread::abandonedCheckQueue.try_enqueue(cpuTimePs);
    JudgementThread::signal();

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::Back].timesPressedSinceLastFrame > 0)
    {
        UtilityFunctions::print("Back action detected, loading DebugLauncherScene");
        get_tree()->change_scene_to_file("res://Scenes/DebugLauncherScene.tscn");
        return;
    }

    int64_t trackPositionPs;
    uint64_t outHandle;
    if (GameManager::audioEngine.value().getPositionForAudioTrack(cpuTimePs, trackPositionPs, outHandle))
    {
        for (RedNote* note : m_course.redNotes)
        {
            note->updatePosition(trackPositionPs, visualOffsetPicoseconds);
        }
        for (BlueNote* note : m_course.blueNotes)
        {
            note->updatePosition(trackPositionPs, visualOffsetPicoseconds);
        }
    }

    for (RedNote* note : m_course.redNotes)
    {
        if (note->isJudged())
        {
            RedNotePrefab* prefab = note->getPrefab();
            if (prefab && prefab->is_inside_tree())
            {
                prefab->queue_free();
                note->setPrefab(nullptr);
            }
        }
    }
    for (BlueNote* note : m_course.blueNotes)
    {
        if (note->isJudged())
        {
            BlueNotePrefab* prefab = note->getPrefab();
            if (prefab && prefab->is_inside_tree())
            {
                prefab->queue_free();
                note->setPrefab(nullptr);
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

void GameplaySceneManager::set_blue_note_scene(Ref<PackedScene> scene)
{
    blueNoteScene = scene;
}

Ref<PackedScene> GameplaySceneManager::get_blue_note_scene() const
{
    return blueNoteScene;
}
