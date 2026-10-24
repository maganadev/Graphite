#include "ResultsScreenSceneManager.hpp"
#include "GameManager.hpp"
#include "JudgementThread.hpp"
#include "SettingsFile.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <godot_cpp/classes/color_rect.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void ResultsScreenSceneManager::_bind_methods()
{
    //
}

ResultsScreenSceneManager::ResultsScreenSceneManager()
{
    //
}

ResultsScreenSceneManager::~ResultsScreenSceneManager()
{
    //
}

void ResultsScreenSceneManager::_ready()
{
    LFProtectObjReadGuardLooping<Chart> chartGuard(GraphiteGlobals::currentChart);
    if (chartGuard.objRef->activeCourseIndex < 0)
    {
        UtilityFunctions::print("ResultsScreen: no active course found");
        return;
    }

    const Course* course = &chartGuard.objRef->courses[chartGuard.objRef->activeCourseIndex];

    auto isChou = [](NoteGradings g) { return g == NoteGradings::CompletelyPerfect || g == NoteGradings::Early_Chou || g == NoteGradings::Late_Chou; };
    auto isRyou = [](NoteGradings g) { return g == NoteGradings::Early_Ryou || g == NoteGradings::Late_Ryou; };
    auto isKa = [](NoteGradings g) { return g == NoteGradings::Early_Ka || g == NoteGradings::Late_Ka; };
    auto isFuka = [](NoteGradings g) { return g == NoteGradings::Early_Fuka || g == NoteGradings::Late_Fuka; };

    int chouCount = 0;
    int ryouCount = 0;
    int kaCount = 0;
    int fukaCount = 0;

    constexpr int BAR_COUNT = 25;
    const int64_t WINDOW_PS = chartGuard.objRef->hitWindowFuka;
    std::vector<int> histogram(BAR_COUNT, 0);

    auto countNote = [&](auto* note)
    {
        if (!note || !note->finishedJudging.load(std::memory_order_acquire))
        {
            return;
        }

        int64_t rawOff = note->picosecondsOff;
        if (rawOff < -WINDOW_PS || rawOff > WINDOW_PS)
        {
            return;
        }

        NoteGradings g = note->grading;
        if (isChou(g))
        {
            chouCount++;
        }
        else if (isRyou(g))
        {
            ryouCount++;
        }
        else if (isKa(g))
        {
            kaCount++;
        }
        else if (isFuka(g))
        {
            fukaCount++;
        }

        int bucket = static_cast<int>((rawOff + WINDOW_PS) * BAR_COUNT / (2 * WINDOW_PS));
        if (bucket < 0)
        {
            bucket = 0;
        }
        if (bucket >= BAR_COUNT)
        {
            bucket = BAR_COUNT - 1;
        }
        histogram[bucket]++;
    };

    for (auto* note : course->redNotes)
    {
        countNote(note);
    }
    for (auto* note : course->blueNotes)
    {
        countNote(note);
    }
    for (auto* note : course->bigRedNotes)
    {
        countNote(note);
    }
    for (auto* note : course->bigBlueNotes)
    {
        countNote(note);
    }

    // Collect off-times for calibration mods, then discard outliers before averaging
    std::vector<int64_t> offTimes;
    auto collectOff = [&](auto* note)
    {
        if (!note || !note->finishedJudging.load(std::memory_order_acquire))
        {
            return;
        }
        int64_t off = note->picosecondsOff;
        if (off < -WINDOW_PS || off > WINDOW_PS)
        {
            return;
        }
        offTimes.push_back(off);
    };
    for (auto* note : course->redNotes)
    {
        collectOff(note);
    }
    for (auto* note : course->blueNotes)
    {
        collectOff(note);
    }
    for (auto* note : course->bigRedNotes)
    {
        collectOff(note);
    }
    for (auto* note : course->bigBlueNotes)
    {
        collectOff(note);
    }

    if (!offTimes.empty())
    {
        auto meanOf = [](const std::vector<int64_t>& values)
        {
            int64_t sum = 0;
            for (int64_t v : values)
            {
                sum += v;
            }
            return sum / static_cast<int64_t>(values.size());
        };

        // Sort the samples and keep only the middle 40%, discarding the lowest
        // 30% and highest 30% as outliers
        std::vector<int64_t> sorted = offTimes;
        std::sort(sorted.begin(), sorted.end());

        const size_t total = sorted.size();
        const size_t trim = total * 3 / 10;
        const size_t begin = trim;
        const size_t end = total - trim;
        std::vector<int64_t> filtered(sorted.begin() + begin, sorted.begin() + end);

        int64_t averageOff = meanOf(filtered);

        UtilityFunctions::print("Offset calibration: kept ", std::to_string(filtered.size()).c_str(), " of ", std::to_string(offTimes.size()).c_str(), " samples, average ", std::to_string(averageOff).c_str(), " ps");

        if (GraphiteGlobals::modVisualOffsetCalibration)
        {
            GraphiteGlobals::visualOffset = -averageOff;
            UtilityFunctions::print("Visual offset calibration: average off-time ", std::to_string(averageOff).c_str(), " ps, new visual offset ", std::to_string(GraphiteGlobals::visualOffset).c_str(), " ps");
            SettingsFile mainSettingsFile("main_settings.json");
            mainSettingsFile.load();
            mainSettingsFile.set<int64_t>("VisualOffset", GraphiteGlobals::visualOffset);
            mainSettingsFile.save();
            GraphiteGlobals::modVisualOffsetCalibration = false;
        }

        if (GraphiteGlobals::modAudioOffsetCalibration)
        {
            GraphiteGlobals::audioOffset = -averageOff;
            UtilityFunctions::print("Audio offset calibration: average off-time ", std::to_string(averageOff).c_str(), " ps, new audio offset ", std::to_string(GraphiteGlobals::audioOffset).c_str(), " ps");
            SettingsFile mainSettingsFile("main_settings.json");
            mainSettingsFile.load();
            mainSettingsFile.set<int64_t>("AudioOffset", GraphiteGlobals::audioOffset);
            mainSettingsFile.save();
            GraphiteGlobals::modAudioOffsetCalibration = false;
        }
    }

    auto setLabelText = [this](const String& name, const String& text)
    {
        Label* label = get_node<Label>(NodePath(String("..") + "/" + name));
        if (label)
        {
            label->set_text(text);
        }
    };

    setLabelText("ChouCount", String::num_int64(chouCount));
    setLabelText("RyouCount", String::num_int64(ryouCount));
    setLabelText("KaCount", String::num_int64(kaCount));
    setLabelText("FukaCount", String::num_int64(fukaCount));

    int maxCount = 1;
    for (int c : histogram)
    {
        if (c > maxCount)
        {
            maxCount = c;
        }
    }

    constexpr double MAX_HEIGHT = 420.0;

    Node2D* colorbars = get_node<Node2D>(NodePath(String("..") + "/Colorbars"));
    if (!colorbars)
    {
        UtilityFunctions::print("ResultsScreen: Colorbars node not found");
        return;
    }

    for (int i = 0; i < BAR_COUNT; i++)
    {
        String barName = String("Bar") + String::num_int64(i);
        ColorRect* bar = colorbars->get_node<ColorRect>(NodePath(barName));
        if (!bar)
        {
            continue;
        }

        double heightRatio = static_cast<double>(histogram[i]) / static_cast<double>(maxCount);
        double barHeight = heightRatio * MAX_HEIGHT;
        bar->set_offset(SIDE_BOTTOM, static_cast<float>(barHeight));
    }
}

void ResultsScreenSceneManager::_process(double delta)
{
    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::Back].timesPressedSinceLastFrame > 0)
    {
        get_tree()->change_scene_to_file("res://Scenes/MenuScene.tscn");
    }
}