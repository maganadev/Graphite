#include "MenuSceneManager.hpp"
#include "MenuTree.hpp"
#include "SongDatabase.hpp"
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"

void MenuSceneManager::_bind_methods()
{
}

MenuSceneManager::MenuSceneManager()
{
}

MenuSceneManager::~MenuSceneManager()
{
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

    buildMenuTree();
}

void MenuSceneManager::buildMenuTree()
{
    database.loadOrBuild("Songs");

    tree.onPlaySong = [this](int32_t songIndex) -> void
    {
        if (songIndex >= 0 && songIndex < static_cast<int32_t>(this->database.songs.size()))
        {
            GraphiteGlobals::currentSongFileName = this->database.songs[songIndex].chartPath;
            UtilityFunctions::print("Playing: ", this->database.songs[songIndex].title.c_str());
            get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        }
    };

    tree.onExit = [this]() -> void
    {
        UtilityFunctions::print("Exit requested");
    };

    tree.build(&database);
    rebuildVisibleWindow();
}

void MenuSceneManager::_process(double delta)
{
    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::DrumRimLeft].timesPressedSinceLastFrame > 0)
    {
        tree.moveUp();
        rebuildVisibleWindow();
    }

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::DrumRimRight].timesPressedSinceLastFrame > 0)
    {
        tree.moveDown();
        rebuildVisibleWindow();
    }

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::DrumCenterLeft].timesPressedSinceLastFrame > 0 ||
        RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::DrumCenterRight].timesPressedSinceLastFrame > 0)
    {
        tree.onEnter();
        rebuildVisibleWindow();
    }

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::Back].timesPressedSinceLastFrame > 0)
    {
        tree.navigateBack();
        rebuildVisibleWindow();
    }
}

void MenuSceneManager::rebuildVisibleWindow()
{
    int32_t count = tree.getCurrentItemCount();
    items.clear();
    items.reserve(count);
    for (int32_t i = 0; i < count; i++)
    {
        items.push_back(tree.getCurrentItemLabel(i));
    }

    if (count == 0)
    {
        for (int32_t i = 0; i < VISIBLE_SLOTS; i++)
        {
            if (slotLabels[i])
            {
                slotLabels[i]->set_text("");
            }
        }
        return;
    }

    selectedIndex = tree.currentItemIndex;
    if (selectedIndex < 0)
    {
        selectedIndex = 0;
    }
    if (selectedIndex >= count)
    {
        selectedIndex = count - 1;
    }

    for (int32_t i = 0; i < VISIBLE_SLOTS; i++)
    {
        if (!slotLabels[i])
        {
            continue;
        }

        int32_t idx = (selectedIndex - CENTER_SLOT + i) % count;
        if (idx < 0)
        {
            idx += count;
        }
        slotLabels[i]->set_text(items[idx].c_str());
    }
}