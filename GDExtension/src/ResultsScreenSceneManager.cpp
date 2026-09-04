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
    LFProtectObjReadGuard<Chart> chartGuard(GraphiteGlobals::currentChart);
    if (!chartGuard.objRef || chartGuard.objRef->activeCourseIndex < 0)
    {
        UtilityFunctions::print("ResultsScreen: no active course found");
        return;
    }

    const Course* course = &chartGuard.objRef->courses[chartGuard.objRef->activeCourseIndex];

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

    constexpr int BAR_COUNT = 45;
    constexpr int64_t WINDOW_PS = 110000000000LL;
    std::vector<int> histogram(BAR_COUNT, 0);

    auto countNote = [&](auto* note) {
        if (!note || !note->isJudged()) return;
        NoteGradings g = note->getGrading();
        if (isChou(g)) chouCount++;
        else if (isRyou(g)) ryouCount++;
        else if (isKa(g)) kaCount++;
        else if (isFuka(g)) fukaCount++;

        int64_t off = note->getPicosecondsOff();
        if (off < -WINDOW_PS) off = -WINDOW_PS;
        if (off > WINDOW_PS) off = WINDOW_PS;
        int bucket = static_cast<int>((off + WINDOW_PS) * BAR_COUNT / (2 * WINDOW_PS));
        if (bucket < 0) bucket = 0;
        if (bucket >= BAR_COUNT) bucket = BAR_COUNT - 1;
        histogram[bucket]++;
    };

    for (auto* note : course->redNotes) countNote(note);
    for (auto* note : course->blueNotes) countNote(note);
    for (auto* note : course->yellowNotes) countNote(note);
    for (auto* note : course->greenNotes) countNote(note);

    auto setLabelText = [this](const String& name, const String& text) {
        Label* label = get_node<Label>(NodePath(String("..") + "/" + name));
        if (label)
            label->set_text(text);
    };

    setLabelText("ChouCount", String::num_int64(chouCount));
    setLabelText("RyouCount", String::num_int64(ryouCount));
    setLabelText("KaCount", String::num_int64(kaCount));
    setLabelText("FukaCount", String::num_int64(fukaCount));

    int maxCount = 1;
    for (int c : histogram)
        if (c > maxCount) maxCount = c;

    constexpr double MAX_HEIGHT = 400.0;

    Node2D* colorbars = get_node<Node2D>(NodePath(String("..") + "/Colorbars"));
    if (!colorbars)
    {
        UtilityFunctions::print("ResultsScreen: Colorbars node not found");
        return;
    }

    for (int i = 0; i < BAR_COUNT; i++)
    {
        String barName = String("ColorRect") + String::num_int64(i);
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