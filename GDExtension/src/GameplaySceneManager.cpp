#include "GameplaySceneManager.hpp"
#include "Barline.hpp"
#include "BarlinePrefab.hpp"
#include "BigBlueNote.hpp"
#include "BigBlueNotePrefab.hpp"
#include "BigGhostNote.hpp"
#include "BigGhostNotePrefab.hpp"
#include "BigGreenNote.hpp"
#include "BigGreenNotePrefab.hpp"
#include "BigRedNote.hpp"
#include "BigRedNotePrefab.hpp"
#include "BigYellowNote.hpp"
#include "BigYellowNotePrefab.hpp"
#include "BlueNote.hpp"
#include "BlueNotePrefab.hpp"
#include "Course.hpp"
#include "GhostNote.hpp"
#include "GhostNotePrefab.hpp"
#include "GraphiteGlobals.hpp"
#include "GreenNote.hpp"
#include "GreenNotePrefab.hpp"
#include "RedNote.hpp"
#include "RedNotePrefab.hpp"
#include "TimingOSSingletons.hpp"
#include "TjauParser.hpp"
#include "YellowNote.hpp"
#include "YellowNotePrefab.hpp"
#include <algorithm>
#include <filesystem>
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
    ClassDB::bind_method(D_METHOD("set_barline_scene", "scene"), &GameplaySceneManager::set_barline_scene);
    ClassDB::bind_method(D_METHOD("get_barline_scene"), &GameplaySceneManager::get_barline_scene);
    ClassDB::bind_method(D_METHOD("set_big_red_note_scene", "scene"), &GameplaySceneManager::set_big_red_note_scene);
    ClassDB::bind_method(D_METHOD("get_big_red_note_scene"), &GameplaySceneManager::get_big_red_note_scene);
    ClassDB::bind_method(D_METHOD("set_big_blue_note_scene", "scene"), &GameplaySceneManager::set_big_blue_note_scene);
    ClassDB::bind_method(D_METHOD("get_big_blue_note_scene"), &GameplaySceneManager::get_big_blue_note_scene);
    ClassDB::bind_method(D_METHOD("set_big_yellow_note_scene", "scene"), &GameplaySceneManager::set_big_yellow_note_scene);
    ClassDB::bind_method(D_METHOD("get_big_yellow_note_scene"), &GameplaySceneManager::get_big_yellow_note_scene);
    ClassDB::bind_method(D_METHOD("set_big_green_note_scene", "scene"), &GameplaySceneManager::set_big_green_note_scene);
    ClassDB::bind_method(D_METHOD("get_big_green_note_scene"), &GameplaySceneManager::get_big_green_note_scene);
    ClassDB::bind_method(D_METHOD("set_big_ghost_note_scene", "scene"), &GameplaySceneManager::set_big_ghost_note_scene);
    ClassDB::bind_method(D_METHOD("get_big_ghost_note_scene"), &GameplaySceneManager::get_big_ghost_note_scene);
}

GameplaySceneManager::GameplaySceneManager()
{
    //
}

GameplaySceneManager::~GameplaySceneManager()
{
    //
}

void GameplaySceneManager::grabObjects()
{
    hitCounter = Object::cast_to<Node2D>(get_node_or_null("../HitCounterParent"));
    if (hitCounter == nullptr)
    {
        UtilityFunctions::print("Failed to find HitCounterParent node");
    }

    hitSpamCounterLabel = Object::cast_to<Label>(get_node_or_null("../HitCounterParent/Label"));
    if (hitSpamCounterLabel == nullptr)
    {
        UtilityFunctions::print("Failed to find HitCounterParent Label");
    }

    p1NotesParent = Object::cast_to<Node2D>(get_node_or_null("../P1NotesParent"));
    if (p1NotesParent == nullptr)
    {
        UtilityFunctions::print("Failed to find P1NotesParent node");
    }

    p1BarlinesParent = Object::cast_to<Node2D>(get_node_or_null("../P1BarlinesParent"));
    if (p1BarlinesParent == nullptr)
    {
        UtilityFunctions::print("Failed to find P1BarlinesParent node");
    }

    comboCounterLabel = Object::cast_to<Label>(get_node_or_null("../ComboCounter"));
    if (comboCounterLabel == nullptr)
    {
        UtilityFunctions::print("Failed to find ComboCounter node");
    }
}

bool GameplaySceneManager::loadChart(ReadyContext& ctx)
{
    ctx.songFileName = GraphiteGlobals::currentSongFileName;

    // Wipe any contents left over from a previous run; the parser appends to the
    // chart it is given rather than resetting it.
    ctx.chart->reset();

    // Capture the current mod settings into the chart proxies before parsing so
    // the parser and gameplay read them from the chart instead of the globals.
    ctx.chart->playbackRate = GraphiteGlobals::playbackRate;
    ctx.chart->userScrollModifier = GraphiteGlobals::userScrollModifier;
    if (GraphiteGlobals::modVisualOffsetCalibration || GraphiteGlobals::modAudioOffsetCalibration)
    {
        ctx.chart->playbackRate = 1.0;
    }

    // Parse the clean UTF-8 .tjau chart directly into the shared chart object.
    std::string parseError;
    if (!parseTjauFile(ctx.songFileName, *ctx.chart, parseError))
    {
        UtilityFunctions::print("Failed to parse chart file: ", ctx.songFileName.c_str(), " | ", parseError.c_str());
        return false;
    }

    int32_t courseIndex = -1;
    for (int32_t i = 0; i < static_cast<int32_t>(ctx.chart->courses.size()); ++i)
    {
        if (ctx.chart->courses[i].courseNumber == GraphiteGlobals::difficulty)
        {
            courseIndex = i;
            break;
        }
    }
    if (courseIndex < 0)
    {
        UtilityFunctions::print("Course not found: ", std::to_string(GraphiteGlobals::difficulty).c_str());
        return false;
    }
    ctx.chart->activeCourseIndex = courseIndex;

    const Course& measuresCourse = ctx.chart->courses[courseIndex];
    UtilityFunctions::print("Measures for course ", std::to_string(GraphiteGlobals::difficulty).c_str(), ": ", std::to_string(measuresCourse.measures.size()).c_str());
    for (const Measure& measure : measuresCourse.measures)
    {
        UtilityFunctions::print("  Measure ", std::to_string(measure.measureIndex).c_str(), " | scroll x", std::to_string(measure.lastScrollMultiplier).c_str(), " | BPM ", std::to_string(measure.lastBpm).c_str());
    }
    return true;
}

void GameplaySceneManager::calculateOffsets(const ReadyContext& ctx)
{
    int64_t unfilteredVisualOffset = GraphiteGlobals::visualOffset;
    int64_t unfilteredAudioOffset = GraphiteGlobals::audioOffset;
    if (ctx.chart->playbackRate != 1.0)
    {
        unfilteredAudioOffset += static_cast<int64_t>(static_cast<double>(ctx.chart->defaultOffsetPicoseconds) / ctx.chart->playbackRate);
    }
    else
    {
        unfilteredAudioOffset += ctx.chart->defaultOffsetPicoseconds;
    }
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
}

bool GameplaySceneManager::loadNoteScenes()
{
    redNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/RedNote.tscn");
    if (redNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load RedNote scene");
        return false;
    }

    blueNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/BlueNote.tscn");
    if (blueNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load BlueNote scene");
        return false;
    }

    yellowNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/YellowNote.tscn");
    if (yellowNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load YellowNote scene");
        return false;
    }

    greenNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/GreenNote.tscn");
    if (greenNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load GreenNote scene");
        return false;
    }

    ghostNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/GhostNote.tscn");
    if (ghostNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load GhostNote scene");
        return false;
    }

    barlineScene = ResourceLoader::get_singleton()->load("res://Prefabs/Barline.tscn");
    if (barlineScene.is_null())
    {
        UtilityFunctions::print("Failed to load Barline scene");
        return false;
    }

    bigRedNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/BigRedNote.tscn");
    if (bigRedNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load BigRedNote scene");
        return false;
    }

    bigBlueNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/BigBlueNote.tscn");
    if (bigBlueNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load BigBlueNote scene");
        return false;
    }

    bigYellowNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/BigYellowNote.tscn");
    if (bigYellowNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load BigYellowNote scene");
        return false;
    }

    bigGreenNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/BigGreenNote.tscn");
    if (bigGreenNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load BigGreenNote scene");
        return false;
    }

    bigGhostNoteScene = ResourceLoader::get_singleton()->load("res://Prefabs/BigGhostNote.tscn");
    if (bigGhostNoteScene.is_null())
    {
        UtilityFunctions::print("Failed to load BigGhostNote scene");
        return false;
    }

    return true;
}

bool GameplaySceneManager::buildChartObject(ReadyContext& ctx)
{
    if (ctx.chart->activeCourseIndex < 0)
    {
        UtilityFunctions::print("Failed to find course in chart");
        return false;
    }

    Course* courseInChart = &ctx.chart->courses[ctx.chart->activeCourseIndex];

    // Increase hit windows if calibration mods are enabled
    if (GraphiteGlobals::modVisualOffsetCalibration || GraphiteGlobals::modAudioOffsetCalibration)
    {
        ctx.chart->hitWindowAboutToBeOutOfRange *= 2;
        ctx.chart->hitWindowFuka *= 2;
        ctx.chart->hitWindowKa *= 2;
        ctx.chart->hitWindowRyou *= 2;
        ctx.chart->hitWindowChou *= 2;
    }

    // Make scrolling notes slower if audio offset calibration mod is enabled
    if (GraphiteGlobals::modAudioOffsetCalibration)
    {
        auto slowScroll = [](auto* note) { note->scrollBPM /= 8.0; };
        for (auto* note : courseInChart->redNotes)
        {
            slowScroll(note);
        }
        for (auto* note : courseInChart->blueNotes)
        {
            slowScroll(note);
        }
        for (auto* note : courseInChart->yellowNotes)
        {
            slowScroll(note);
        }
        for (auto* note : courseInChart->greenNotes)
        {
            slowScroll(note);
        }
        for (auto* note : courseInChart->ghostNotes)
        {
            slowScroll(note);
        }
        for (auto* note : courseInChart->barlineNotes)
        {
            slowScroll(note);
        }
        for (auto* note : courseInChart->bigRedNotes)
        {
            slowScroll(note);
        }
        for (auto* note : courseInChart->bigBlueNotes)
        {
            slowScroll(note);
        }
        for (auto* note : courseInChart->bigYellowNotes)
        {
            slowScroll(note);
        }
        for (auto* note : courseInChart->bigGreenNotes)
        {
            slowScroll(note);
        }
        for (auto* note : courseInChart->bigGhostNotes)
        {
            slowScroll(note);
        }
    }

    std::vector<std::pair<int64_t, Node*>> sortedPrefabs;
    std::vector<std::pair<int64_t, Node*>> sortedBarlinePrefabs;
    for (RedNote* note : courseInChart->redNotes)
    {
        Node* instance = redNoteScene->instantiate();
        RedNotePrefab* prefab = Object::cast_to<RedNotePrefab>(instance);
        if (prefab)
        {
            note->prefab = prefab;
            sortedPrefabs.push_back({note->startTimePicoseconds, prefab});
        }
    }
    for (BlueNote* note : courseInChart->blueNotes)
    {
        Node* instance = blueNoteScene->instantiate();
        BlueNotePrefab* prefab = Object::cast_to<BlueNotePrefab>(instance);
        if (prefab)
        {
            note->prefab = prefab;
            sortedPrefabs.push_back({note->startTimePicoseconds, prefab});
        }
    }
    for (YellowNote* note : courseInChart->yellowNotes)
    {
        Node* instance = yellowNoteScene->instantiate();
        YellowNotePrefab* prefab = Object::cast_to<YellowNotePrefab>(instance);
        if (prefab)
        {
            note->prefab = prefab;
            sortedPrefabs.push_back({note->startTimePicoseconds, prefab});
        }
    }
    for (GreenNote* note : courseInChart->greenNotes)
    {
        Node* instance = greenNoteScene->instantiate();
        GreenNotePrefab* prefab = Object::cast_to<GreenNotePrefab>(instance);
        if (prefab)
        {
            note->prefab = prefab;
            sortedPrefabs.push_back({note->startTimePicoseconds, prefab});
        }
    }
    for (GhostNote* note : courseInChart->ghostNotes)
    {
        Node* instance = ghostNoteScene->instantiate();
        GhostNotePrefab* prefab = Object::cast_to<GhostNotePrefab>(instance);
        if (prefab)
        {
            note->prefab = prefab;
            sortedPrefabs.push_back({note->startTimePicoseconds, prefab});
        }
    }
    for (Barline* note : courseInChart->barlineNotes)
    {
        Node* instance = barlineScene->instantiate();
        BarlinePrefab* prefab = Object::cast_to<BarlinePrefab>(instance);
        if (prefab)
        {
            note->prefab = prefab;
            sortedBarlinePrefabs.push_back({note->startTimePicoseconds, prefab});
        }
    }
    for (BigRedNote* note : courseInChart->bigRedNotes)
    {
        Node* instance = bigRedNoteScene->instantiate();
        BigRedNotePrefab* prefab = Object::cast_to<BigRedNotePrefab>(instance);
        if (prefab)
        {
            note->prefab = prefab;
            sortedPrefabs.push_back({note->startTimePicoseconds, prefab});
        }
    }
    for (BigBlueNote* note : courseInChart->bigBlueNotes)
    {
        Node* instance = bigBlueNoteScene->instantiate();
        BigBlueNotePrefab* prefab = Object::cast_to<BigBlueNotePrefab>(instance);
        if (prefab)
        {
            note->prefab = prefab;
            sortedPrefabs.push_back({note->startTimePicoseconds, prefab});
        }
    }
    for (BigYellowNote* note : courseInChart->bigYellowNotes)
    {
        Node* instance = bigYellowNoteScene->instantiate();
        BigYellowNotePrefab* prefab = Object::cast_to<BigYellowNotePrefab>(instance);
        if (prefab)
        {
            note->prefab = prefab;
            sortedPrefabs.push_back({note->startTimePicoseconds, prefab});
        }
    }
    for (BigGreenNote* note : courseInChart->bigGreenNotes)
    {
        Node* instance = bigGreenNoteScene->instantiate();
        BigGreenNotePrefab* prefab = Object::cast_to<BigGreenNotePrefab>(instance);
        if (prefab)
        {
            note->prefab = prefab;
            sortedPrefabs.push_back({note->startTimePicoseconds, prefab});
        }
    }
    for (BigGhostNote* note : courseInChart->bigGhostNotes)
    {
        Node* instance = bigGhostNoteScene->instantiate();
        BigGhostNotePrefab* prefab = Object::cast_to<BigGhostNotePrefab>(instance);
        if (prefab)
        {
            note->prefab = prefab;
            sortedPrefabs.push_back({note->startTimePicoseconds, prefab});
        }
    }

    if (p1NotesParent == nullptr || p1BarlinesParent == nullptr)
    {
        UtilityFunctions::print("Failed to attach notes/barlines: P1NotesParent or P1BarlinesParent missing");
        return false;
    }

    std::sort(sortedPrefabs.begin(), sortedPrefabs.end(), [](const std::pair<int64_t, Node*>& a, const std::pair<int64_t, Node*>& b) { return a.first > b.first; });
    std::sort(sortedBarlinePrefabs.begin(), sortedBarlinePrefabs.end(), [](const std::pair<int64_t, Node*>& a, const std::pair<int64_t, Node*>& b) { return a.first > b.first; });

    for (auto& entry : sortedPrefabs)
    {
        p1NotesParent->add_child(entry.second);
    }

    for (auto& entry : sortedBarlinePrefabs)
    {
        p1BarlinesParent->add_child(entry.second);
    }

    courseInChart->populateLanes();

    size_t totalNotes = courseInChart->redNotes.size() + courseInChart->blueNotes.size() + courseInChart->yellowNotes.size() + courseInChart->greenNotes.size() + courseInChart->ghostNotes.size() + courseInChart->barlineNotes.size() + courseInChart->bigRedNotes.size() + courseInChart->bigBlueNotes.size() + courseInChart->bigYellowNotes.size() + courseInChart->bigGreenNotes.size() + courseInChart->bigGhostNotes.size();
    UtilityFunctions::print("Spawned ", std::to_string(totalNotes).c_str(), " notes for course: ", std::to_string(GraphiteGlobals::difficulty).c_str());

    ctx.wavePath = ctx.chart->wave;
    return true;
}

bool GameplaySceneManager::loadAndPlayAudio(const ReadyContext& ctx)
{
    std::filesystem::path audioFilePath = ctx.wavePath;
    if (!ctx.wavePath.empty())
    {
        audioFilePath = std::filesystem::path(ctx.songFileName).parent_path() / ctx.wavePath;
    }

    // Free any old existing song
    GraphiteGlobals::audioEngine.value().freeAudioTrack(GraphiteGlobals::audioTrackHandle);
    GraphiteGlobals::audioEngine.value().flushDeletedAudio();

    // Load new song
    if (!GraphiteGlobals::audioEngine.value().createAudioTrack(audioFilePath.string(), -36, GraphiteGlobals::audioTrackHandle, ctx.chart->playbackRate))
    {
        UtilityFunctions::print("Failed to load audio track: ", audioFilePath.string().c_str());
        return false;
    }

    // Play the audio track
    GraphiteGlobals::audioEngine.value().playAudioTrack(GraphiteGlobals::audioTrackHandle);
    GraphiteGlobals::audioEngine.value().setTimedAudioTrack(GraphiteGlobals::audioTrackHandle);

    return true;
}

void GameplaySceneManager::markGameplayActive(const ReadyContext& ctx)
{
    ctx.chart->gameplayActive = true;
    UtilityFunctions::print("Loaded song: ", ctx.chart->title.c_str(), " | Wave: ", ctx.wavePath.c_str());
}

void GameplaySceneManager::_ready()
{
    ReadyContext ctx;

    grabObjects();

    LFProtectObjWriteGuardLooping<Chart> guard(GraphiteGlobals::currentChart, true);
    ctx.chart = guard.objRef;

    if (!loadChart(ctx))
    {
        return;
    }

    calculateOffsets(ctx);

    if (!loadNoteScenes())
    {
        return;
    }

    if (!buildChartObject(ctx))
    {
        return;
    }

    if (!loadAndPlayAudio(ctx))
    {
        return;
    }

    markGameplayActive(ctx);
}

void GameplaySceneManager::_exit_tree()
{
    //
}

void GameplaySceneManager::getFrameTime(ProcessContext& ctx)
{
    ctx.cpuTimePs = TimingOSSingletons::cpuTimer.GetValue();
    uint64_t outHandle = 0;
    if (!GraphiteGlobals::audioEngine.value().getPositionForAudioTrack(ctx.cpuTimePs, ctx.trackPositionPs, outHandle))
    {
        // TODO : Handle audio track position retrieval failure
    }
}

void GameplaySceneManager::gradeAbandonedNotes(const ProcessContext& ctx)
{
    JudgementThread::abandonedCheckQueue.try_enqueue(ctx.cpuTimePs);
    JudgementThread::signal();
}

bool GameplaySceneManager::handleKeyPresses()
{
    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::Back].timesPressedSinceLastFrame > 0)
    {
        UtilityFunctions::print("Back action detected, loading ResultsScreen");
        {
            LFProtectObjWriteGuardLooping<Chart> guard(GraphiteGlobals::currentChart, true);
            guard.objRef->gameplayActive = false;
        }
        get_tree()->change_scene_to_file("res://Scenes/ResultsScreen.tscn");
        return true;
    }
    return false;
}

void GameplaySceneManager::updateNotePositions(const ProcessContext& ctx, const Chart* chart)
{
    const Course* course = &chart->courses[chart->activeCourseIndex];
    int64_t correctedPositionPs = ctx.trackPositionPs - effectiveVisualOffset;
    double userScrollModifier = chart->userScrollModifier;

    for (RedNote* note : course->redNotes)
    {
        note->updatePosition(correctedPositionPs, userScrollModifier);
    }
    for (BlueNote* note : course->blueNotes)
    {
        note->updatePosition(correctedPositionPs, userScrollModifier);
    }
    for (YellowNote* note : course->yellowNotes)
    {
        note->updatePosition(correctedPositionPs, userScrollModifier);
    }
    for (GreenNote* note : course->greenNotes)
    {
        note->updatePosition(correctedPositionPs, userScrollModifier);
    }
    for (GhostNote* note : course->ghostNotes)
    {
        note->updatePosition(correctedPositionPs, userScrollModifier);
    }
    for (Barline* note : course->barlineNotes)
    {
        note->updatePosition(correctedPositionPs, userScrollModifier);
    }
    for (BigRedNote* note : course->bigRedNotes)
    {
        note->updatePosition(correctedPositionPs, userScrollModifier);
    }
    for (BigBlueNote* note : course->bigBlueNotes)
    {
        note->updatePosition(correctedPositionPs, userScrollModifier);
    }
    for (BigYellowNote* note : course->bigYellowNotes)
    {
        note->updatePosition(correctedPositionPs, userScrollModifier);
    }
    for (BigGreenNote* note : course->bigGreenNotes)
    {
        note->updatePosition(correctedPositionPs, userScrollModifier);
    }
    for (BigGhostNote* note : course->bigGhostNotes)
    {
        note->updatePosition(correctedPositionPs, userScrollModifier);
    }
}

void GameplaySceneManager::updateHitCounter(const ProcessContext& ctx, const Chart* chart)
{
    if (!hitCounter)
    {
        return;
    }

    const Course* course = &chart->courses[chart->activeCourseIndex];

    bool hitSpamAvailable = false;
    HittableNote* currentSpamNote = nullptr;
    int64_t gradedSongPositionPs = ctx.trackPositionPs - effectiveJudgementOffset;
    for (size_t i = 0; i < course->hitSpamNoteList.size(); ++i)
    {
        HittableNote* note = course->hitSpamNoteList.getAt(i);
        if (note->finishedJudging.load(std::memory_order_acquire))
        {
            continue;
        }
        NoteGradings grading{NoteGradings::Ungraded};
        int64_t offtime = 0;
        note->getWhatGradingWouldBe(gradedSongPositionPs, chart, grading, offtime);
        if (grading == NoteGradings::Early_OutOfRange || grading == NoteGradings::Late_OutOfRange)
        {
            continue;
        }
        if (currentSpamNote == nullptr)
        {
            currentSpamNote = note;
        }
        if (grading == NoteGradings::CompletelyPerfect)
        {
            hitSpamAvailable = true;
            break;
        }
    }
    hitCounter->set_modulate(Color(1, 1, 1, hitSpamAvailable ? 1.0f : 0.0f));
    if (hitSpamCounterLabel)
    {
        int64_t count = currentSpamNote ? currentSpamNote->getSpamHitsCount() : 0;
        hitSpamCounterLabel->set_text(String::num_int64(count));
    }
}

void GameplaySceneManager::updateComboCounter(const Chart* chart)
{
    if (!comboCounterLabel)
    {
        return;
    }

    int32_t combo = chart->combo.load(std::memory_order_relaxed);
    if (combo < 10)
    {
        comboCounterLabel->set_text("");
    }
    else
    {
        comboCounterLabel->set_text(String::num_int64(combo));
    }
}

void GameplaySceneManager::removeJudgedNotes(const Chart* chart)
{
    const Course* course = &chart->courses[chart->activeCourseIndex];

    for (RedNote* note : course->redNotes)
    {
        if (note->finishedJudging.load(std::memory_order_acquire))
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
        if (note->finishedJudging.load(std::memory_order_acquire))
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
        if (note->finishedJudging.load(std::memory_order_acquire))
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
        if (note->finishedJudging.load(std::memory_order_acquire))
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
    for (BigRedNote* note : course->bigRedNotes)
    {
        if (note->finishedJudging.load(std::memory_order_acquire))
        {
            if (note->grading == NoteGradings::Late_OutOfRange)
            {
                continue;
            }
            BigRedNotePrefab* prefab = note->prefab;
            if (prefab && prefab->is_inside_tree())
            {
                prefab->queue_free();
                note->prefab = nullptr;
            }
        }
    }
    for (BigBlueNote* note : course->bigBlueNotes)
    {
        if (note->finishedJudging.load(std::memory_order_acquire))
        {
            if (note->grading == NoteGradings::Late_OutOfRange)
            {
                continue;
            }
            BigBlueNotePrefab* prefab = note->prefab;
            if (prefab && prefab->is_inside_tree())
            {
                prefab->queue_free();
                note->prefab = nullptr;
            }
        }
    }
    for (BigYellowNote* note : course->bigYellowNotes)
    {
        if (note->finishedJudging.load(std::memory_order_acquire))
        {
            if (note->grading == NoteGradings::Late_OutOfRange)
            {
                continue;
            }
            BigYellowNotePrefab* prefab = note->prefab;
            if (prefab && prefab->is_inside_tree())
            {
                prefab->queue_free();
                note->prefab = nullptr;
            }
        }
    }
    for (BigGreenNote* note : course->bigGreenNotes)
    {
        if (note->finishedJudging.load(std::memory_order_acquire))
        {
            if (note->grading == NoteGradings::Late_OutOfRange)
            {
                continue;
            }
            BigGreenNotePrefab* prefab = note->prefab;
            if (prefab && prefab->is_inside_tree())
            {
                prefab->queue_free();
                note->prefab = nullptr;
            }
        }
    }
}

void GameplaySceneManager::allJudgedCheck()
{
    bool allJudged = true;
    {
        LFProtectObjReadGuard<Chart> chartGuard(GraphiteGlobals::currentChart);
        if (!chartGuard.objRef)
        {
            return;
        }

        if (chartGuard.objRef->activeCourseIndex < 0)
        {
            return;
        }

        const Course* course = &chartGuard.objRef->courses[chartGuard.objRef->activeCourseIndex];

        for (RedNote* note : course->redNotes)
        {
            if (!note->finishedJudging.load(std::memory_order_acquire))
            {
                allJudged = false;
                break;
            }
        }
        if (allJudged)
        {
            for (BlueNote* note : course->blueNotes)
            {
                if (!note->finishedJudging.load(std::memory_order_acquire))
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
                if (!note->finishedJudging.load(std::memory_order_acquire))
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
                if (!note->finishedJudging.load(std::memory_order_acquire))
                {
                    allJudged = false;
                    break;
                }
            }
        }
        if (allJudged)
        {
            for (BigRedNote* note : course->bigRedNotes)
            {
                if (!note->finishedJudging.load(std::memory_order_acquire))
                {
                    allJudged = false;
                    break;
                }
            }
        }
        if (allJudged)
        {
            for (BigBlueNote* note : course->bigBlueNotes)
            {
                if (!note->finishedJudging.load(std::memory_order_acquire))
                {
                    allJudged = false;
                    break;
                }
            }
        }
        if (allJudged)
        {
            for (BigYellowNote* note : course->bigYellowNotes)
            {
                if (!note->finishedJudging.load(std::memory_order_acquire))
                {
                    allJudged = false;
                    break;
                }
            }
        }
        if (allJudged)
        {
            for (BigGreenNote* note : course->bigGreenNotes)
            {
                if (!note->finishedJudging.load(std::memory_order_acquire))
                {
                    allJudged = false;
                    break;
                }
            }
        }
    }
    if (allJudged)
    {
        resultsScreenTriggered = true;
        {
            LFProtectObjWriteGuardLooping<Chart> guard(GraphiteGlobals::currentChart, true);
            guard.objRef->gameplayActive = false;
        }
        get_tree()->change_scene_to_file("res://Scenes/ResultsScreen.tscn");
    }
}

void GameplaySceneManager::_process(double delta)
{
    ProcessContext ctx;

    getFrameTime(ctx);
    gradeAbandonedNotes(ctx);

    if (handleKeyPresses())
    {
        return;
    }

    {
        LFProtectObjReadGuard<Chart> chartGuard(GraphiteGlobals::currentChart);
        if (!chartGuard.objRef)
        {
            return;
        }

        if (chartGuard.objRef->activeCourseIndex < 0)
        {
            return;
        }

        updateNotePositions(ctx, chartGuard.objRef);
        updateHitCounter(ctx, chartGuard.objRef);
        updateComboCounter(chartGuard.objRef);
        removeJudgedNotes(chartGuard.objRef);
    }

    allJudgedCheck();
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

void GameplaySceneManager::set_barline_scene(Ref<PackedScene> scene)
{
    barlineScene = scene;
}

Ref<PackedScene> GameplaySceneManager::get_barline_scene() const
{
    return barlineScene;
}

void GameplaySceneManager::set_big_red_note_scene(Ref<PackedScene> scene)
{
    bigRedNoteScene = scene;
}

Ref<PackedScene> GameplaySceneManager::get_big_red_note_scene() const
{
    return bigRedNoteScene;
}

void GameplaySceneManager::set_big_blue_note_scene(Ref<PackedScene> scene)
{
    bigBlueNoteScene = scene;
}

Ref<PackedScene> GameplaySceneManager::get_big_blue_note_scene() const
{
    return bigBlueNoteScene;
}

void GameplaySceneManager::set_big_yellow_note_scene(Ref<PackedScene> scene)
{
    bigYellowNoteScene = scene;
}

Ref<PackedScene> GameplaySceneManager::get_big_yellow_note_scene() const
{
    return bigYellowNoteScene;
}

void GameplaySceneManager::set_big_green_note_scene(Ref<PackedScene> scene)
{
    bigGreenNoteScene = scene;
}

Ref<PackedScene> GameplaySceneManager::get_big_green_note_scene() const
{
    return bigGreenNoteScene;
}

void GameplaySceneManager::set_big_ghost_note_scene(Ref<PackedScene> scene)
{
    bigGhostNoteScene = scene;
}

Ref<PackedScene> GameplaySceneManager::get_big_ghost_note_scene() const
{
    return bigGhostNoteScene;
}
