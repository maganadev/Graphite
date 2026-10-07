#include "MenuSceneManager.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"
#include "MenuTree.hpp"
#include "SongDatabase.hpp"
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

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

void MenuSceneManager::saveMenuState()
{
    GraphiteGlobals::menuCurrentNodeId = tree.currentNodeId;
    GraphiteGlobals::menuCurrentItemIndex = tree.currentItemIndex;
    GraphiteGlobals::menuScrollOffset = tree.scrollOffset;

    GraphiteGlobals::menuNavNodeIds.clear();
    GraphiteGlobals::menuNavItemIndices.clear();
    GraphiteGlobals::menuNavScrollOffsets.clear();
    for (const NavPosition& pos : tree.navStack)
    {
        GraphiteGlobals::menuNavNodeIds.push_back(pos.nodeId);
        GraphiteGlobals::menuNavItemIndices.push_back(pos.itemIndex);
        GraphiteGlobals::menuNavScrollOffsets.push_back(pos.scrollOffset);
    }
}

void MenuSceneManager::restoreMenuState()
{
    if (GraphiteGlobals::menuCurrentNodeId < 0)
    {
        return;
    }

    tree.navigateTo(GraphiteGlobals::menuCurrentNodeId, GraphiteGlobals::menuCurrentItemIndex);
    tree.scrollOffset = GraphiteGlobals::menuScrollOffset;

    tree.navStack.clear();
    for (size_t i = 0; i < GraphiteGlobals::menuNavNodeIds.size(); i++)
    {
        NavPosition pos;
        pos.nodeId = GraphiteGlobals::menuNavNodeIds[i];
        pos.itemIndex = GraphiteGlobals::menuNavItemIndices[i];
        pos.scrollOffset = GraphiteGlobals::menuNavScrollOffsets[i];
        tree.navStack.push_back(pos);
    }

    GraphiteGlobals::menuCurrentNodeId = -1;
    GraphiteGlobals::menuNavNodeIds.clear();
    GraphiteGlobals::menuNavItemIndices.clear();
    GraphiteGlobals::menuNavScrollOffsets.clear();
}

void MenuSceneManager::buildMenuTree()
{
    database.loadOrBuild("Songs");

    tree.onPlaySong = [this](int32_t songIndex) -> void
    {
        if (songIndex >= 0 && songIndex < static_cast<int32_t>(this->database.songs.size()))
        {
            saveMenuState();
            GraphiteGlobals::currentSongFileName = this->database.songs[songIndex].chartPath;
            UtilityFunctions::print("Playing: ", this->database.songs[songIndex].title.c_str());
            get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        }
    };

    tree.onExit = [this]() -> void { UtilityFunctions::print("Exit requested"); };

    tree.onAction = [this](int32_t actionData) -> void
    {
        if (actionData == ACT_VisualCalibration)
        {
            saveMenuState();
            UtilityFunctions::print("VisualCalibration selected, loading GameplayScene with VisualCalibration.tjap");
            GraphiteGlobals::modVisualOffsetCalibration = true;
            GraphiteGlobals::modAudioOffsetCalibration = false;
            GraphiteGlobals::difficulty = 0;
            GraphiteGlobals::currentSongFileName = "AutoCalibration/VisualCalibration.tjap";
            get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        }
        else if (actionData == ACT_AudioCalibration)
        {
            saveMenuState();
            UtilityFunctions::print("AudioCalibration selected, loading GameplayScene with AudioCalibration.tjap");
            GraphiteGlobals::modVisualOffsetCalibration = false;
            GraphiteGlobals::modAudioOffsetCalibration = true;
            GraphiteGlobals::difficulty = 0;
            GraphiteGlobals::currentSongFileName = "AutoCalibration/AudioCalibration.tjap";
            get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        }
    };

    tree.build(&database);
    restoreMenuState();
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

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::DrumCenterLeft].timesPressedSinceLastFrame > 0 || RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::DrumCenterRight].timesPressedSinceLastFrame > 0)
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