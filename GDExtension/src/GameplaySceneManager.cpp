#include "GameplaySceneManager.hpp"
#include "BlueNote.hpp"
#include "BlueNotePrefab.hpp"
#include "Course.hpp"
#include "GhostNote.hpp"
#include "GhostNotePrefab.hpp"
#include "GreenNote.hpp"
#include "GreenNotePrefab.hpp"
#include "RedNote.hpp"
#include "RedNotePrefab.hpp"
#include "TimingOSSingletons.hpp"
#include "YellowNote.hpp"
#include "YellowNotePrefab.hpp"
#include <algorithm>
#include <fstream>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <vector>

void GameplaySceneManager::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_red_note_scene", "scene"), &GameplaySceneManager::set_red_note_scene);
    ClassDB::bind_method(D_METHOD("get_red_note_scene"), &GameplaySceneManager::get_red_note_scene);
    ClassDB::bind_method(D_METHOD("set_blue_note_scene", "scene"), &GameplaySceneManager::set_blue_note_scene);
    ClassDB::bind_method(D_METHOD("get_blue_note_scene"), &GameplaySceneManager::get_blue_note_scene);
    ClassDB::bind_method(D_METHOD("set_yellow_note_scene", "scene"), &GameplaySceneManager::set_yellow_note_scene);
    ClassDB::bind_method(D_METHOD("get_yellow_note_scene"), &GameplaySceneManager::get_yellow_note_scene);
    ClassDB::bind_method(D_METHOD("set_green_note_scene", "scene"), &GameplaySceneManager::set_green_note_scene);
    ClassDB::bind_method(D_METHOD("get_green_note_scene"), &GameplaySceneManager::get_green_note_scene);
    ClassDB::bind_method(D_METHOD("set_ghost_note_scene", "scene"), &GameplaySceneManager::set_ghost_note_scene);
    ClassDB::bind_method(D_METHOD("get_ghost_note_scene"), &GameplaySceneManager::get_ghost_note_scene);
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
    std::string songFileName = GraphiteGlobals::currentSongFileName;
    int32_t courseDifficulty = (GraphiteGlobals::modVisualOffsetCalibration || GraphiteGlobals::modAudioOffsetCalibration) ? 3 : 2;

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
        if (chart.courses[i].courseNumber == courseDifficulty)
        {
            courseIndex = i;
            break;
        }
    }
    if (courseIndex < 0)
    {
        UtilityFunctions::print("Course not found: ", std::to_string(courseDifficulty).c_str());
        return;
    }
    Course* targetCourse = &chart.courses[courseIndex];

    // Unfiltered offsets
    int64_t unfilteredVisualOffset = GraphiteGlobals::visualOffset;
    int64_t unfilteredAudioOffset = chart.defaultOffsetPicoseconds + GraphiteGlobals::audioOffset;
    int64_t unfilteredJudgementOffset = 0;

    // If calibration mods are enabled, override the offsets
    if (GraphiteGlobals::modVisualOffsetCalibration || GraphiteGlobals::modAudioOffsetCalibration)
    {
        unfilteredVisualOffset = 0;
        unfilteredAudioOffset = 0;
        unfilteredJudgementOffset = 0;
    }

    // Calculate effective offsets
    effectiveVisualOffset = unfilteredVisualOffset - unfilteredAudioOffset;
    effectiveAudioOffset = unfilteredAudioOffset - unfilteredAudioOffset;
    effectiveJudgementOffset = unfilteredJudgementOffset - unfilteredAudioOffset;
    JudgementThread::judgementOffset.store(effectiveJudgementOffset, std::memory_order_release);

    UtilityFunctions::print("Input Visual Offset: ", std::to_string(unfilteredVisualOffset).c_str(), " ps");
    UtilityFunctions::print("Input Audio Offset: ", std::to_string(unfilteredAudioOffset).c_str(), " ps");
    UtilityFunctions::print("Input Judgement Offset: ", std::to_string(unfilteredJudgementOffset).c_str(), " ps");
    UtilityFunctions::print("Output Visual Offset: ", std::to_string(effectiveVisualOffset).c_str(), " ps");
    UtilityFunctions::print("Output Audio Offset: ", std::to_string(effectiveAudioOffset).c_str(), " ps");
    UtilityFunctions::print("Output Judgement Offset: ", std::to_string(effectiveJudgementOffset).c_str(), " ps");

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

    yellowNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/YellowNote.tscn");
    if (yellowNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load YellowNote scene");
        return;
    }

    greenNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/GreenNote.tscn");
    if (greenNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load GreenNote scene");
        return;
    }

    ghostNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/GhostNote.tscn");
    if (ghostNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load GhostNote scene");
        return;
    }

    chart.activeCourseIndex = courseIndex;

    // Build the course under write guard so judgment thread can't read it before it's ready
    {
        LFProtectObjWriteGuard<Chart> guard(GraphiteGlobals::currentChart, true);
        *guard.objRef = std::move(chart);
        if (guard.objRef->activeCourseIndex < 0)
        {
            UtilityFunctions::print("Failed to find course in chart after move");
            return;
        }

        if (GraphiteGlobals::modVisualOffsetCalibration || GraphiteGlobals::modAudioOffsetCalibration)
        {
            guard.objRef->hitWindowAboutToBeOutOfRange *= 4;
            guard.objRef->hitWindowFuka *= 4;
            guard.objRef->hitWindowKa *= 4;
            guard.objRef->hitWindowRyou *= 4;
            guard.objRef->hitWindowChou *= 4;
        }

        Course* courseInChart = &guard.objRef->courses[guard.objRef->activeCourseIndex];

        std::vector<std::pair<int64_t, Node*>> sortedPrefabs;

        for (RedNote* note : courseInChart->redNotes)
        {
            Node* instance = redNoteScene->instantiate();
            RedNotePrefab* prefab = Object::cast_to<RedNotePrefab>(instance);
            if (prefab)
            {
                note->prefab = prefab;
                prefab->set_z_index(3);
                sortedPrefabs.push_back({note->timePicoseconds, prefab});
            }
        }
        for (BlueNote* note : courseInChart->blueNotes)
        {
            Node* instance = blueNoteScene->instantiate();
            BlueNotePrefab* prefab = Object::cast_to<BlueNotePrefab>(instance);
            if (prefab)
            {
                note->prefab = prefab;
                prefab->set_z_index(3);
                sortedPrefabs.push_back({note->timePicoseconds, prefab});
            }
        }
        for (YellowNote* note : courseInChart->yellowNotes)
        {
            Node* instance = yellowNoteScene->instantiate();
            YellowNotePrefab* prefab = Object::cast_to<YellowNotePrefab>(instance);
            if (prefab)
            {
                note->prefab = prefab;
                prefab->set_z_index(3);
                sortedPrefabs.push_back({note->timePicoseconds, prefab});
            }
        }
        for (GreenNote* note : courseInChart->greenNotes)
        {
            Node* instance = greenNoteScene->instantiate();
            GreenNotePrefab* prefab = Object::cast_to<GreenNotePrefab>(instance);
            if (prefab)
            {
                note->prefab = prefab;
                prefab->set_z_index(3);
                sortedPrefabs.push_back({note->timePicoseconds, prefab});
            }
        }
        for (GhostNote* note : courseInChart->ghostNotes)
        {
            Node* instance = ghostNoteScene->instantiate();
            GhostNotePrefab* prefab = Object::cast_to<GhostNotePrefab>(instance);
            if (prefab)
            {
                note->prefab = prefab;
                prefab->set_z_index(3);
                sortedPrefabs.push_back({note->timePicoseconds, prefab});
            }
        }

        std::sort(sortedPrefabs.begin(), sortedPrefabs.end(), [](const std::pair<int64_t, Node*>& a, const std::pair<int64_t, Node*>& b) { return a.first < b.first; });

        for (auto& entry : sortedPrefabs)
        {
            add_child(entry.second);
        }

        courseInChart->populateLanes();
    }

    UtilityFunctions::print("Spawned notes for course: ", std::to_string(courseDifficulty).c_str());

    // Read wave path from the chart via read guard
    std::string wavePath;
    {
        LFProtectObjReadGuard<Chart> chartGuard(GraphiteGlobals::currentChart);
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

    // Load the wave file - if visual offset calibration, use silence instead
    std::string audioPath = wavePath;
    if (GraphiteGlobals::modVisualOffsetCalibration)
    {
        audioPath = "GameplaySilence.ogg";
        UtilityFunctions::print("Visual offset calibration: using GameplaySilence.ogg");
    }
    if (!GraphiteGlobals::audioEngine.value().createAudioTrack(audioPath, -36, audioTrackHandle))
    {
        UtilityFunctions::print("Failed to load audio track: ", audioPath.c_str());
        return;
    }

    // Play the audio track
    GraphiteGlobals::audioEngine.value().playAudioTrack(audioTrackHandle);
    GraphiteGlobals::audioEngine.value().setTimedAudioTrack(audioTrackHandle);

    {
        LFProtectObjReadGuard<Chart> chartGuard(GraphiteGlobals::currentChart);
        if (chartGuard.objRef)
        {
            UtilityFunctions::print("Loaded song: ", chartGuard.objRef->title.c_str(), " | Wave: ", wavePath.c_str());
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

    LFProtectObjReadGuard<Chart> chartGuard(GraphiteGlobals::currentChart);
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
    if (GraphiteGlobals::audioEngine.value().getPositionForAudioTrack(cpuTimePs, trackPositionPs, outHandle))
    {
        for (RedNote* note : course->redNotes)
        {
            note->updatePosition(trackPositionPs, effectiveVisualOffset);
        }
        for (BlueNote* note : course->blueNotes)
        {
            note->updatePosition(trackPositionPs, effectiveVisualOffset);
        }
        for (YellowNote* note : course->yellowNotes)
        {
            note->updatePosition(trackPositionPs, effectiveVisualOffset);
        }
        for (GreenNote* note : course->greenNotes)
        {
            note->updatePosition(trackPositionPs, effectiveVisualOffset);
        }
        for (GhostNote* note : course->ghostNotes)
        {
            note->updatePosition(trackPositionPs, effectiveVisualOffset);
        }
    }

    for (RedNote* note : course->redNotes)
    {
        if (note->judged.load(std::memory_order_acquire))
        {
            if (note->grading == NoteGradings::Late_OutOfRange)
            {
                continue;
            }
            RedNotePrefab* prefab = note->prefab;
            if (prefab && prefab->is_inside_tree())
            {
                prefab->queue_free();
                note->prefab = nullptr;
            }
        }
    }
    for (BlueNote* note : course->blueNotes)
    {
        if (note->judged.load(std::memory_order_acquire))
        {
            if (note->grading == NoteGradings::Late_OutOfRange)
            {
                continue;
            }
            BlueNotePrefab* prefab = note->prefab;
            if (prefab && prefab->is_inside_tree())
            {
                prefab->queue_free();
                note->prefab = nullptr;
            }
        }
    }
    for (YellowNote* note : course->yellowNotes)
    {
        if (note->judged.load(std::memory_order_acquire))
        {
            if (note->grading == NoteGradings::Late_OutOfRange)
            {
                continue;
            }
            YellowNotePrefab* prefab = note->prefab;
            if (prefab && prefab->is_inside_tree())
            {
                prefab->queue_free();
                note->prefab = nullptr;
            }
        }
    }
    for (GreenNote* note : course->greenNotes)
    {
        if (note->judged.load(std::memory_order_acquire))
        {
            if (note->grading == NoteGradings::Late_OutOfRange)
            {
                continue;
            }
            GreenNotePrefab* prefab = note->prefab;
            if (prefab && prefab->is_inside_tree())
            {
                prefab->queue_free();
                note->prefab = nullptr;
            }
        }
    }

    if (!resultsScreenTriggered)
    {
        bool allJudged = true;
        for (RedNote* note : course->redNotes)
        {
            if (!note->judged.load(std::memory_order_acquire))
            {
                allJudged = false;
                break;
            }
        }
        if (allJudged)
        {
            for (BlueNote* note : course->blueNotes)
            {
                if (!note->judged.load(std::memory_order_acquire))
                {
                    allJudged = false;
                    break;
                }
            }
        }
        if (allJudged)
        {
            for (YellowNote* note : course->yellowNotes)
            {
                if (!note->judged.load(std::memory_order_acquire))
                {
                    allJudged = false;
                    break;
                }
            }
        }
        if (allJudged)
        {
            for (GreenNote* note : course->greenNotes)
            {
                if (!note->judged.load(std::memory_order_acquire))
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

void GameplaySceneManager::set_yellow_note_scene(Ref<PackedScene> scene)
{
    yellowNoteScene = scene;
}

Ref<PackedScene> GameplaySceneManager::get_yellow_note_scene() const
{
    return yellowNoteScene;
}

void GameplaySceneManager::set_green_note_scene(Ref<PackedScene> scene)
{
    greenNoteScene = scene;
}

Ref<PackedScene> GameplaySceneManager::get_green_note_scene() const
{
    return greenNoteScene;
}

void GameplaySceneManager::set_ghost_note_scene(Ref<PackedScene> scene)
{
    ghostNoteScene = scene;
}

Ref<PackedScene> GameplaySceneManager::get_ghost_note_scene() const
{
    return ghostNoteScene;
}
