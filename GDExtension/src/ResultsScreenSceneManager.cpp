#include "ResultsScreenSceneManager.hpp"
#include "GameManager.hpp"
#include "JudgementThread.hpp"
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/color_rect.hpp>
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
    LFProtectObjReadGuard<Chart> chartGuard(GameManager::currentChart);
    if (!chartGuard.objRef || chartGuard.objRef->activeCourseIndex < 0)
    {
        UtilityFunctions::print("ResultsScreen: no active course found");
        return;
    }

    const Course* course = &chartGuard.objRef->courses[chartGuard.objRef->activeCourseIndex];

    // Gather all judged notes from both lanes
    struct JudgedNote {
        NoteGradings grading;
        int64_t picosecondsOff;
    };
    std::vector<JudgedNote> judgedNotes;

    auto collectNotes = [&](const auto& notes) {
        for (auto* note : notes)
        {
            if (note && note->isJudged())
            {
                judgedNotes.push_back({note->getGrading(), note->getPicosecondsOff()});
            }
        }
    };

    collectNotes(course->redNotes);
    collectNotes(course->blueNotes);
    collectNotes(course->yellowNotes);
    collectNotes(course->greenNotes);

    if (judgedNotes.empty())
    {
        UtilityFunctions::print("ResultsScreen: no judged notes");
        return;
    }

    // Count by grading category
    auto isChou = [](NoteGradings g) {
        return g == NoteGradings::CompletlelyPerfect || g == NoteGradings::Early_Chou || g == NoteGradings::Late_Chou;
    };
    auto isRyou = [](NoteGradings g) {
        return g == NoteGradings::Early_Ryou || g == NoteGradings::Late_Ryou;
    };
    auto isKa = [](NoteGradings g) {
        return g == NoteGradings::Early_Ka || g == NoteGradings::Late_Ka;
    };
    auto isFuka = [](NoteGradings g) {
        return g == NoteGradings::Early_Fuka || g == NoteGradings::Late_Fuka;
    };

    int chouCount = 0, ryouCount = 0, kaCount = 0, fukaCount = 0;
    for (const auto& jn : judgedNotes)
    {
        if (isChou(jn.grading)) chouCount++;
        else if (isRyou(jn.grading)) ryouCount++;
        else if (isKa(jn.grading)) kaCount++;
        else if (isFuka(jn.grading)) fukaCount++;
    }

    // Update labels
    auto setLabelText = [this](const String& name, const String& text) {
        Label* label = get_node<Label>(NodePath(String("..") + "/" + name));
        if (label)
            label->set_text(text);
    };

    setLabelText("ChouCount", String::num_int64(chouCount));
    setLabelText("RyouCount", String::num_int64(ryouCount));
    setLabelText("KaCount", String::num_int64(kaCount));
    setLabelText("FukaCount", String::num_int64(fukaCount));

    // Build histogram: 44 bars, time window ±110ms
    constexpr int BAR_COUNT = 44;
    constexpr int64_t WINDOW_PS = 110000000000LL;
    std::vector<int> histogram(BAR_COUNT, 0);

    for (const auto& jn : judgedNotes)
    {
        int64_t off = jn.picosecondsOff;
        if (off < -WINDOW_PS) off = -WINDOW_PS;
        if (off > WINDOW_PS) off = WINDOW_PS;
        int bucket = static_cast<int>((off + WINDOW_PS) * BAR_COUNT / (2 * WINDOW_PS));
        if (bucket < 0) bucket = 0;
        if (bucket >= BAR_COUNT) bucket = BAR_COUNT - 1;
        histogram[bucket]++;
    }

    int maxCount = 1;
    for (int c : histogram)
        if (c > maxCount) maxCount = c;

    constexpr double MAX_HEIGHT = 400.0;

    // Update color bars
    Node2D* colorbars = get_node<Node2D>(NodePath(String("..") + "/Colorbars"));
    if (!colorbars)
    {
        UtilityFunctions::print("ResultsScreen: Colorbars node not found");
        return;
    }

    for (int i = 0; i < BAR_COUNT; i++)
    {
        String barName = String("ColorRect") + String::num_int64(i + 2);
        ColorRect* bar = colorbars->get_node<ColorRect>(NodePath(barName));
        if (!bar)
            continue;

        double heightRatio = static_cast<double>(histogram[i]) / static_cast<double>(maxCount);
        double barHeight = heightRatio * MAX_HEIGHT;
        bar->set_offset(SIDE_BOTTOM, static_cast<float>(barHeight));
    }
}

void ResultsScreenSceneManager::_process(double delta)
{
    //
}