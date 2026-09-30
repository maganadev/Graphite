#include "MenuSceneManager.hpp"
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void MenuSceneManager::_bind_methods()
{
    //
}

MenuSceneManager::MenuSceneManager()
{
    items = {"Apple", "Banana", "Carrot", "Dog", "Cat", "Otter", "Keyboard", "Mouse", "Monitor", "Piano", "Guitar", "Drums", "Flute", "Trumpet", "Violin", "Cello", "Harp", "Saxophone", "Clarinet", "Trombone"};
}

MenuSceneManager::~MenuSceneManager()
{
    //
}

void MenuSceneManager::_ready()
{
    Node* parent = get_parent();
    if (!parent)
    {
        UtilityFunctions::print("MenuSceneManager: no parent");
        return;
    }

    slotLabels.resize(VISIBLE_SLOTS);
    for (int32_t i = 0; i < VISIBLE_SLOTS; i++)
    {
        String path = String("MenuElements/Element") + String::num_int64(i) + "/MarginContainer/Background/MarginContainer/Background/Label";
        slotLabels[i] = parent->get_node<Label>(NodePath(path));
    }

    rebuildVisibleWindow();
}

void MenuSceneManager::_process(double delta)
{
    int32_t n = static_cast<int32_t>(items.size());

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::DrumRimLeft].timesPressedSinceLastFrame > 0)
    {
        selectedIndex = (selectedIndex - 1 + n) % n;
        rebuildVisibleWindow();
    }

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::DrumRimRight].timesPressedSinceLastFrame > 0)
    {
        selectedIndex = (selectedIndex + 1) % n;
        rebuildVisibleWindow();
    }

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::Enter].timesPressedSinceLastFrame > 0)
    {
        UtilityFunctions::print("Selected: ", items[selectedIndex].c_str());
    }
}

void MenuSceneManager::rebuildVisibleWindow()
{
    int32_t n = static_cast<int32_t>(items.size());

    for (int32_t i = 0; i < VISIBLE_SLOTS; i++)
    {
        if (!slotLabels[i])
        {
            continue;
        }

        int32_t idx = (selectedIndex - CENTER_SLOT + i) % n;
        if (idx < 0)
        {
            idx += n;
        }
        slotLabels[i]->set_text(items[idx].c_str());
    }
}