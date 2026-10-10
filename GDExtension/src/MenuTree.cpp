#include "MenuTree.hpp"
#include "GraphiteGlobals.hpp"
#include "SongDatabase.hpp"
#include <cstdint>
#include <string>
#include <vector>

void MenuTree::build(SongDatabase* db)
{
    database = db;
    nodes.clear();
    navStack.clear();
    currentNodeId = -1;
    currentItemIndex = 0;
    scrollOffset = 0;
    pendingSongIndex = -1;

    int32_t mainId = createMainMenu();
    navigateTo(mainId, 0);
}

int32_t MenuTree::addNode(MenuNode node)
{
    int32_t id = static_cast<int32_t>(nodes.size());
    nodes.push_back(node);
    return id;
}

int32_t MenuTree::createMainMenu()
{
    MenuNode node;
    node.parentNodeId = -1;

    MenuItem play;
    play.label = "Play";
    play.type = MIT_Submenu;
    play.targetNodeId = createFolderSelect();
    node.items.push_back(play);

    MenuItem settings;
    settings.label = "Settings";
    settings.type = MIT_Submenu;
    settings.targetNodeId = createSettingsMenu();
    node.items.push_back(settings);

    MenuItem exit;
    exit.label = "Exit";
    exit.type = MIT_Action;
    exit.actionData = ACT_Exit;
    node.items.push_back(exit);

    return addNode(node);
}

int32_t MenuTree::createSettingsMenu()
{
    MenuNode node;
    node.parentNodeId = -1;

    MenuItem visual;
    visual.label = "Visual Offset";
    visual.type = MIT_Action;
    visual.actionData = ACT_VisualCalibration;
    node.items.push_back(visual);

    MenuItem audio;
    audio.label = "Audio Offset";
    audio.type = MIT_Action;
    audio.actionData = ACT_AudioCalibration;
    node.items.push_back(audio);

    {
        int32_t speedNodeId = createPlaybackSpeedMenu();
        MenuItem speed;
        speed.label = "Playback Speed";
        speed.type = MIT_Submenu;
        speed.targetNodeId = speedNodeId;
        node.items.push_back(speed);
    }

    MenuItem back;
    back.label = "Back";
    back.type = MIT_Back;
    node.items.push_back(back);

    return addNode(node);
}

int32_t MenuTree::createPlaybackSpeedMenu()
{
    MenuNode node;
    node.parentNodeId = -1;

    auto addSpeed = [&](const std::string& label, double rate)
    {
        MenuItem item;
        item.label = label;
        item.type = MIT_Action;
        item.actionData = ACT_PlaybackSpeed;
        int32_t encodedRate = static_cast<int32_t>(rate * 100.0 + 0.5);
        item.targetNodeId = encodedRate;
        node.items.push_back(item);
    };

    addSpeed("0.5x (Slow)", 0.5);
    addSpeed("0.75x (Medium)", 0.75);
    addSpeed("1.0x (Normal)", 1.0);
    addSpeed("1.5x (Fast)", 1.5);

    MenuItem back;
    back.label = "Back";
    back.type = MIT_Back;
    node.items.push_back(back);

    return addNode(node);
}

int32_t MenuTree::createFolderSelect()
{
    MenuNode node;
    node.parentNodeId = -1;

    std::vector<int> allSongs = database->getAllSongs();
    int32_t allTarget = createSortMode(-1, allSongs);

    MenuItem all;
    all.label = "All";
    all.type = MIT_Submenu;
    all.targetNodeId = allTarget;
    node.items.push_back(all);

    for (int fi = 0; fi < static_cast<int>(database->folders.size()); fi++)
    {
        std::vector<int> folderSongs = database->getFolderSongs(fi);
        int32_t folderTarget = createSortMode(-1, folderSongs);

        MenuItem folder;
        folder.label = database->folders[fi].name;
        folder.type = MIT_Submenu;
        folder.targetNodeId = folderTarget;
        node.items.push_back(folder);
    }

    MenuItem back;
    back.label = "Back";
    back.type = MIT_Back;
    node.items.push_back(back);

    return addNode(node);
}

int32_t MenuTree::createSortMode(int32_t parentId, const std::vector<int>& songSubset)
{
    MenuNode node;
    node.parentNodeId = parentId;
    node.songSubset = songSubset;

    int32_t allTarget = createSongList(-1, songSubset, "All");
    int32_t levelTarget = createLevelMenu(-1, songSubset);

    std::vector<std::string> titleGroups = database->getTitleGroups(songSubset);
    std::vector<std::string> artistGroups = database->getArtistGroups(songSubset);
    int32_t titleTarget = createGroupMenu(-1, songSubset, titleGroups, "title");
    int32_t artistTarget = createGroupMenu(-1, songSubset, artistGroups, "artist");

    MenuItem all;
    all.label = "All";
    all.type = MIT_Submenu;
    all.targetNodeId = allTarget;
    node.items.push_back(all);

    MenuItem byLevel;
    byLevel.label = "Level Folder";
    byLevel.type = MIT_Submenu;
    byLevel.targetNodeId = levelTarget;
    node.items.push_back(byLevel);

    MenuItem byTitle;
    byTitle.label = "Title Folder";
    byTitle.type = MIT_Submenu;
    byTitle.targetNodeId = titleTarget;
    node.items.push_back(byTitle);

    MenuItem byArtist;
    byArtist.label = "Artist Folder";
    byArtist.type = MIT_Submenu;
    byArtist.targetNodeId = artistTarget;
    node.items.push_back(byArtist);

    MenuItem back;
    back.label = "Back";
    back.type = MIT_Back;
    node.items.push_back(back);

    return addNode(node);
}

int32_t MenuTree::createLevelMenu(int32_t parentId, const std::vector<int>& songSubset)
{
    MenuNode node;
    node.parentNodeId = parentId;
    node.songSubset = songSubset;

    std::vector<std::string> groups = database->getLevelGroups(songSubset);
    for (size_t i = 0; i < groups.size(); i++)
    {
        int levelNum = 10 - static_cast<int>(i);
        std::vector<int> filtered = database->getSongsByLevel(songSubset, static_cast<uint8_t>(levelNum));
        int32_t listId = createSongList(-1, filtered, groups[i]);

        MenuItem item;
        item.label = groups[i];
        item.type = MIT_Submenu;
        item.targetNodeId = listId;
        node.items.push_back(item);
    }

    MenuItem back;
    back.label = "Back";
    back.type = MIT_Back;
    node.items.push_back(back);

    return addNode(node);
}

int32_t MenuTree::createGroupMenu(int32_t parentId, const std::vector<int>& songSubset, const std::vector<std::string>& groups, const std::string& groupType)
{
    MenuNode node;
    node.parentNodeId = parentId;
    node.songSubset = songSubset;

    for (const std::string& g : groups)
    {
        std::vector<int> filtered = database->getSongsByGroup(songSubset, g);
        int32_t listId = createSongList(-1, filtered, g);

        MenuItem item;
        item.label = g;
        item.type = MIT_Submenu;
        item.targetNodeId = listId;
        node.items.push_back(item);
    }

    MenuItem back;
    back.label = "Back";
    back.type = MIT_Back;
    node.items.push_back(back);

    return addNode(node);
}

int32_t MenuTree::createSongList(int32_t parentId, const std::vector<int>& songSubset, const std::string& filterLabel)
{
    MenuNode node;
    node.parentNodeId = parentId;
    node.songSubset = songSubset;

    for (int idx : songSubset)
    {
        if (idx >= 0 && idx < static_cast<int>(database->songs.size()))
        {
            int32_t diffNodeId = createDifficultySelectForSong(-1, idx);
            MenuItem item;
            item.label = database->songs[idx].title;
            item.type = MIT_Submenu;
            item.targetNodeId = diffNodeId;
            item.actionData = idx;
            node.items.push_back(item);
        }
    }

    MenuItem back;
    back.label = "Back";
    back.type = MIT_Back;
    node.items.push_back(back);

    return addNode(node);
}

int32_t MenuTree::createDifficultySelectForSong(int32_t parentId, int songIndex)
{
    MenuNode node;
    node.parentNodeId = parentId;

    if (songIndex >= 0 && songIndex < static_cast<int>(database->songs.size()))
    {
        for (const CourseEntry& ce : database->songs[songIndex].courses)
        {
            MenuItem item;
            item.label = std::to_string(ce.level) + "* " + ce.name;
            item.type = MIT_Action;
            item.actionData = ce.courseId;
            node.items.push_back(item);
        }
    }

    MenuItem back;
    back.label = "Back";
    back.type = MIT_Back;
    node.items.push_back(back);

    return addNode(node);
}

void MenuTree::navigateTo(int32_t nodeId, int32_t itemIndex)
{
    if (nodeId < 0 || nodeId >= static_cast<int32_t>(nodes.size()))
    {
        return;
    }
    currentNodeId = nodeId;
    currentItemIndex = 0;
    scrollOffset = 0;

    if (itemIndex >= 0)
    {
        currentItemIndex = itemIndex;
    }

    int32_t count = static_cast<int32_t>(nodes[currentNodeId].items.size());
    if (count > 0 && currentItemIndex >= count)
    {
        currentItemIndex = count - 1;
    }
}

void MenuTree::navigateBack()
{
    if (navStack.empty())
    {
        return;
    }
    pendingSongIndex = -1;
    NavPosition prev = navStack.back();
    navStack.pop_back();
    navigateTo(prev.nodeId, prev.itemIndex);
    scrollOffset = prev.scrollOffset;
}

MenuItem* MenuTree::getCurrentItem()
{
    if (currentNodeId < 0 || currentNodeId >= static_cast<int32_t>(nodes.size()))
    {
        return nullptr;
    }
    MenuNode& node = nodes[currentNodeId];
    if (currentItemIndex < 0 || currentItemIndex >= static_cast<int32_t>(node.items.size()))
    {
        return nullptr;
    }
    return &node.items[currentItemIndex];
}

int32_t MenuTree::getCurrentItemCount()
{
    if (currentNodeId < 0 || currentNodeId >= static_cast<int32_t>(nodes.size()))
    {
        return 0;
    }
    return static_cast<int32_t>(nodes[currentNodeId].items.size());
}

std::string MenuTree::getCurrentItemLabel(int32_t index)
{
    if (currentNodeId < 0 || currentNodeId >= static_cast<int32_t>(nodes.size()))
    {
        return "";
    }
    MenuNode& node = nodes[currentNodeId];
    if (index < 0 || index >= static_cast<int32_t>(node.items.size()))
    {
        return "";
    }
    return node.items[index].label;
}

void MenuTree::moveUp()
{
    int32_t count = getCurrentItemCount();
    if (count <= 1)
    {
        return;
    }
    currentItemIndex--;
    if (currentItemIndex < 0)
    {
        currentItemIndex = count - 1;
        scrollOffset = std::max(0, count - 9);
    }
    else if (currentItemIndex < scrollOffset)
    {
        scrollOffset = currentItemIndex;
    }
}

void MenuTree::moveDown()
{
    int32_t count = getCurrentItemCount();
    if (count <= 1)
    {
        return;
    }
    currentItemIndex++;
    if (currentItemIndex >= count)
    {
        currentItemIndex = 0;
        scrollOffset = 0;
    }
    else if (currentItemIndex >= scrollOffset + 9)
    {
        scrollOffset = currentItemIndex - 8;
    }
}

void MenuTree::onEnter()
{
    MenuItem* item = getCurrentItem();
    if (!item)
    {
        return;
    }

    NavPosition pos;
    pos.nodeId = currentNodeId;
    pos.itemIndex = currentItemIndex;
    pos.scrollOffset = scrollOffset;

    if (item->type == MIT_Submenu)
    {
        if (item->actionData >= 0)
        {
            pendingSongIndex = item->actionData;
        }
        navStack.push_back(pos);
        navigateTo(item->targetNodeId, 0);
    }
    else if (item->type == MIT_Back)
    {
        pendingSongIndex = -1;
        navigateBack();
    }
    else if (item->type == MIT_Action)
    {
        if (item->actionData == ACT_Exit)
        {
            if (onExit)
            {
                onExit();
            }
        }
        else if (item->actionData == ACT_VisualCalibration || item->actionData == ACT_AudioCalibration)
        {
            if (onAction)
            {
                onAction(item->actionData);
            }
        }
        else if (item->actionData == ACT_PlaybackSpeed)
        {
            GraphiteGlobals::playbackRate = static_cast<double>(item->targetNodeId) / 100.0;
            navigateBack();
        }
        else if (item->actionData >= 0)
        {
            if (pendingSongIndex >= 0)
            {
                int32_t songIdx = pendingSongIndex;
                pendingSongIndex = -1;
                if (onPlaySongWithDifficulty)
                {
                    onPlaySongWithDifficulty(songIdx, item->actionData);
                }
            }
            else if (onPlaySong)
            {
                onPlaySong(item->actionData);
            }
        }
    }
}