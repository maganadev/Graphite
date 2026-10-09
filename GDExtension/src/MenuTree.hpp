#ifndef MenuTree_hpp
#define MenuTree_hpp

#include "SongDatabase.hpp"
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

enum MenuItemType : uint8_t
{
    MIT_Back = 0,
    MIT_Submenu = 1,
    MIT_Action = 2,
};

enum ActionCode : int32_t
{
    ACT_PlayMusic = -1,
    ACT_Settings = -2,
    ACT_Exit = -3,
    ACT_PlaySong = -4,
    ACT_VisualCalibration = -5,
    ACT_AudioCalibration = -6,
};

struct MenuItem
{
    std::string label;
    MenuItemType type;
    int32_t targetNodeId; // for MIT_Submenu: node to navigate to
    int32_t actionData;   // for MIT_Action: song index or negative action code
};

struct MenuNode
{
    std::vector<MenuItem> items;
    int32_t parentNodeId = -1;
    std::vector<int> songSubset;
};

struct NavPosition
{
    int32_t nodeId;
    int32_t itemIndex;
    int32_t scrollOffset;
};

class MenuTree
{
public:
    std::vector<MenuNode> nodes;
    std::vector<NavPosition> navStack;

    int32_t currentNodeId = -1;
    int32_t currentItemIndex = 0;
    int32_t scrollOffset = 0;

    SongDatabase* database = nullptr;

    int32_t pendingSongIndex = -1;

    void build(SongDatabase* db);

    int32_t addNode(MenuNode node);

    void navigateTo(int32_t nodeId, int32_t itemIndex);

    void navigateBack();

    MenuItem* getCurrentItem();

    int32_t getCurrentItemCount();

    std::string getCurrentItemLabel(int32_t index);

    void moveUp();

    void moveDown();

    void onEnter();

    std::function<void(int32_t)> onPlaySong;
    std::function<void(int32_t, int32_t)> onPlaySongWithDifficulty;
    std::function<void()> onExit;
    std::function<void(int32_t)> onAction;

private:
    int32_t createMainMenu();
    int32_t createSettingsMenu();
    int32_t createFolderSelect();
    int32_t createSortMode(int32_t parentId, const std::vector<int>& songSubset);
    int32_t createLevelMenu(int32_t parentId, const std::vector<int>& songSubset);
    int32_t createGroupMenu(int32_t parentId, const std::vector<int>& songSubset, const std::vector<std::string>& groups, const std::string& groupType);
    int32_t createSongList(int32_t parentId, const std::vector<int>& songSubset, const std::string& filterLabel);
    int32_t createDifficultySelectForSong(int32_t parentId, int songIndex);
};

#endif