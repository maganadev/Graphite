#include "GameplaySceneManager.hpp"
#include "BlueNote.hpp"
#include "BlueNotePrefab.hpp"
#include "Course.hpp"
#include "GreenNote.hpp"
#include "GreenNotePrefab.hpp"
#include "RedNote.hpp"
#include "RedNotePrefab.hpp"
#include "TimingOSSingletons.hpp"
#include "YellowNote.hpp"
#include "YellowNotePrefab.hpp"
#include <fstream>
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
    std::string songFileName = "Song.json";
    std::string courseDifficulty = "Oni";

    // Open the song
    std::ifstream ifs(songFileName);
    if (!ifs.is_open())
    {
        UtilityFunctions::print("Failed to open song file: ", songFileName.c_str());
        return;
    }
    json songJson = json::parse(ifs);

    Chart chart = Chart::FromJson(songJson);

    int32_t courseIndex = -1;
    for (int32_t i = 0; i < static_cast<int32_t>(chart.courses.size()); ++i)
    {
        if (chart.courses[i].name == courseDifficulty)
        {
            courseIndex = i;
            break;
        }
    }
    if (courseIndex < 0)
    {
        UtilityFunctions::print("Course not found: ", courseDifficulty.c_str());
        return;
    }
    Course* targetCourse = &chart.courses[courseIndex];

    int64_t unfilteredVisualOffset = GameManager::visualOffset;
    int64_t unfilteredAudioOffset = targetCourse->offset_picoseconds + GameManager::audioOffset;
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

    chart.activeCourse = courseDifficulty;
    chart.activeCourseIndex = courseIndex;

    // Build the course under write guard so judgment thread can't read it before it's ready
    {
        LFProtectObjWriteGuard<Chart> guard(GameManager::currentChart, true);
        *guard.objRef = std::move(chart);
        if (guard.objRef->activeCourseIndex < 0)
        {
            UtilityFunctions::print("Failed to find course in chart after move");
            return;
        }
        Course* courseInChart = &guard.objRef->courses[guard.objRef->activeCourseIndex];

        for (const auto& noteEvent : courseInChart->notes)
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
                    courseInChart->redNotes.push_back(note);
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
                    courseInChart->blueNotes.push_back(note);
                }
            }
        }

        courseInChart->populateLanes();
    }

    UtilityFunctions::print("Spawned notes for course: ", courseDifficulty.c_str());

    // Read wave path from the chart via read guard
    std::string wavePath;
    {
        LFProtectObjReadGuard<Chart> chartGuard(GameManager::currentChart);
        if (chartGuard.objRef)
        {
            wavePath = chartGuard.objRef->wave;
        }
    }

    if (wavePath.empty())
    {
        UtilityFunctions::print("No wave file specified in song JSON");
        return;
    }

    // Load the wave file
    if (!GameManager::audioEngine.value().createAudioTrackBlocking(wavePath, -36, audioTrackHandle))
    {
        UtilityFunctions::print("Failed to load audio track: ", wavePath.c_str());
        return;
    }

    // Play the audio track
    GameManager::audioEngine.value().playAudioTrack(audioTrackHandle);
    GameManager::audioEngine.value().setTimedAudioTrack(audioTrackHandle);

    {
        LFProtectObjReadGuard<Chart> chartGuard(GameManager::currentChart);
        if (chartGuard.objRef)
        {
            UtilityFunctions::print("Loaded song: ", chartGuard.objRef->title.c_str(), " | Course: ", chartGuard.objRef->activeCourse.c_str(), " | Wave: ", wavePath.c_str());
        }
    }
}

void GameplaySceneManager::_exit_tree()
{
    //
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
        UtilityFunctions::print("Back action detected, loading ResultsScreen");
        get_tree()->change_scene_to_file("res://Scenes/ResultsScreen.tscn");
        return;
    }

    LFProtectObjReadGuard<Chart> chartGuard(GameManager::currentChart);
    if (!chartGuard.objRef)
    {
        return;
    }

    if (chartGuard.objRef->activeCourseIndex < 0)
    {
        return;
    }

    Course* course = const_cast<Course*>(&chartGuard.objRef->courses[chartGuard.objRef->activeCourseIndex]);

    int64_t trackPositionPs;
    uint64_t outHandle;
    if (GameManager::audioEngine.value().getPositionForAudioTrack(cpuTimePs, trackPositionPs, outHandle))
    {
        for (RedNote* note : course->redNotes)
        {
            note->updatePosition(trackPositionPs, visualOffsetPicoseconds);
        }
        for (BlueNote* note : course->blueNotes)
        {
            note->updatePosition(trackPositionPs, visualOffsetPicoseconds);
        }
    }

    for (RedNote* note : course->redNotes)
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
    for (BlueNote* note : course->blueNotes)
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

    if (!resultsScreenTriggered)
    {
        bool allJudged = true;
        for (RedNote* note : course->redNotes)
        {
            if (!note->isJudged())
            {
                allJudged = false;
                break;
            }
        }
        if (allJudged)
        {
            for (BlueNote* note : course->blueNotes)
            {
                if (!note->isJudged())
                {
                    allJudged = false;
                    break;
                }
            }
        }
        if (allJudged)
        {
            resultsScreenTriggered = true;
            UtilityFunctions::print("All notes judged, switching to ResultsScreen");
            get_tree()->change_scene_to_file("res://Scenes/ResultsScreen.tscn");
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