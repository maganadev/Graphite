#include "MenuSceneManager.hpp"
#include "../../RhythmInput/RhythmInput/RhythmInputEngine.hpp"
#include "MenuTree.hpp"
#include "SettingsFile.hpp"
#include "SongDatabase.hpp"
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/engine.hpp>
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

    spacers.resize(SPACER_COUNT);
    for (int32_t i = 0; i < SPACER_COUNT; i++)
    {
        String path = String("MenuElements/Spacer") + String::num_int64(i);
        spacers[i] = parent->get_node<Control>(NodePath(path));
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
    database.loadOrBuild(GraphiteGlobals::songFolder);

    tree.onPlaySong = [this](int32_t songIndex) -> void
    {
        if (songIndex >= 0 && songIndex < static_cast<int32_t>(this->database.songs.size()))
        {
            GraphiteGlobals::modVisualOffsetCalibration = false;
            GraphiteGlobals::modAudioOffsetCalibration = false;
            saveMenuState();
            GraphiteGlobals::currentSongFileName = this->database.songs[songIndex].chartPath;
            UtilityFunctions::print("Playing: ", this->database.songs[songIndex].title.c_str());
            get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        }
    };

    tree.onPlaySongWithDifficulty = [this](int32_t songIndex, int32_t difficulty) -> void
    {
        if (songIndex >= 0 && songIndex < static_cast<int32_t>(this->database.songs.size()))
        {
            GraphiteGlobals::modVisualOffsetCalibration = false;
            GraphiteGlobals::modAudioOffsetCalibration = false;
            this->tree.navigateBack();
            saveMenuState();
            GraphiteGlobals::difficulty = difficulty;
            GraphiteGlobals::currentSongFileName = this->database.songs[songIndex].chartPath;
            UtilityFunctions::print("Playing: ", this->database.songs[songIndex].title.c_str(), " (difficulty ", std::to_string(difficulty).c_str(), ")");
            get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        }
    };

    tree.onExit = [this]() -> void { UtilityFunctions::print("Exit requested"); };

    tree.onAction = [this](int32_t actionData) -> void
    {
        if (actionData == ACT_VisualCalibration)
        {
            saveMenuState();
            UtilityFunctions::print("VisualCalibration selected, loading GameplayScene with VisualCalibration.tjau");
            GraphiteGlobals::modVisualOffsetCalibration = true;
            GraphiteGlobals::modAudioOffsetCalibration = false;
            GraphiteGlobals::difficulty = 0;
            GraphiteGlobals::currentSongFileName = "OutsideAssets/AutoCalibration/VisualCalibration.tjau";
            get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        }
        else if (actionData == ACT_AudioCalibration)
        {
            saveMenuState();
            UtilityFunctions::print("AudioCalibration selected, loading GameplayScene with AudioCalibration.tjau");
            GraphiteGlobals::modVisualOffsetCalibration = false;
            GraphiteGlobals::modAudioOffsetCalibration = true;
            GraphiteGlobals::difficulty = 0;
            GraphiteGlobals::currentSongFileName = "OutsideAssets/AutoCalibration/AudioCalibration.tjau";
            get_tree()->change_scene_to_file("res://Scenes/GameplayScene.tscn");
        }
        else if (actionData == ACT_FullscreenToggle)
        {
            DisplayServer* displayServer = DisplayServer::get_singleton();
            if (displayServer)
            {
                displayServer->window_set_mode(GraphiteGlobals::fullscreen ? DisplayServer::WINDOW_MODE_FULLSCREEN : DisplayServer::WINDOW_MODE_WINDOWED);
            }

            SettingsFile mainSettingsFile("main_settings.json");
            mainSettingsFile.load();
            mainSettingsFile.set<bool>("Fullscreen", GraphiteGlobals::fullscreen);
            mainSettingsFile.save();
        }
        else if (actionData == ACT_PlaybackSpeedUp || actionData == ACT_PlaybackSpeedDown || actionData == ACT_ScrollSpeedUp || actionData == ACT_ScrollSpeedDown)
        {
            SettingsFile mainSettingsFile("main_settings.json");
            mainSettingsFile.load();
            mainSettingsFile.set<double>("PlaybackRate", GraphiteGlobals::playbackRate);
            mainSettingsFile.set<double>("ScrollSpeed", GraphiteGlobals::userScrollModifier);
            mainSettingsFile.save();
        }
        else if (actionData == ACT_FpsCapToggle)
        {
            Engine* engine = Engine::get_singleton();
            if (engine)
            {
                engine->set_max_fps(GraphiteGlobals::fpsCapEnabled ? GraphiteGlobals::fpsCap : 0);
            }

            SettingsFile mainSettingsFile("main_settings.json");
            mainSettingsFile.load();
            mainSettingsFile.set<int32_t>("FpsCap", GraphiteGlobals::fpsCap);
            mainSettingsFile.set<bool>("FpsCapEnabled", GraphiteGlobals::fpsCapEnabled);
            mainSettingsFile.save();
        }
        else if (actionData == ACT_VsyncToggle)
        {
            DisplayServer* displayServer = DisplayServer::get_singleton();
            if (displayServer)
            {
                displayServer->window_set_vsync_mode(GraphiteGlobals::vsync ? DisplayServer::VSYNC_ENABLED : DisplayServer::VSYNC_DISABLED);
            }

            SettingsFile mainSettingsFile("main_settings.json");
            mainSettingsFile.load();
            mainSettingsFile.set<bool>("Vsync", GraphiteGlobals::vsync);
            mainSettingsFile.save();
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
    }

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::DrumRimRight].timesPressedSinceLastFrame > 0)
    {
        tree.moveDown();
    }

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::DrumCenterLeft].timesPressedSinceLastFrame > 0 || RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::DrumCenterRight].timesPressedSinceLastFrame > 0)
    {
        tree.onEnter();
    }

    if (RhythmInput::RhythmInputEngine::gameActions[GameActionIndices::Back].timesPressedSinceLastFrame > 0)
    {
        tree.navigateBack();
    }

    rebuildVisibleWindow();
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
        for (Control* spacer : spacers)
        {
            if (spacer)
            {
                spacer->set_visible(false);
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

    std::vector<int32_t> slotIndices(VISIBLE_SLOTS, 0);
    for (int32_t i = 0; i < VISIBLE_SLOTS; i++)
    {
        int32_t idx = (selectedIndex - CENTER_SLOT + i) % count;
        if (idx < 0)
        {
            idx += count;
        }
        slotIndices[i] = idx;

        if (slotLabels[i])
        {
            slotLabels[i]->set_text(items[idx].c_str());
        }
    }

    for (int32_t i = 0; i < SPACER_COUNT; i++)
    {
        if (!spacers[i])
        {
            continue;
        }

        bool atLogicalEnd = count > 1 && slotIndices[i] == count - 1;
        spacers[i]->set_visible(atLogicalEnd);
    }
}