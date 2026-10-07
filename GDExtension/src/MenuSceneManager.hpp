#ifndef MenuSceneManager_hpp
#define MenuSceneManager_hpp

#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <string>
#include <vector>

#include "GameManager.hpp"
#include "MenuTree.hpp"
#include "SongDatabase.hpp"

using namespace ::godot;

class MenuSceneManager : public Sprite2D
{
    GDCLASS(MenuSceneManager, Sprite2D)

protected:
    static void _bind_methods();

public:
    MenuSceneManager();
    ~MenuSceneManager();
    void _ready() override;
    void _process(double delta) override;

private:
    void rebuildVisibleWindow();
    void buildMenuTree();
    void saveMenuState();
    void restoreMenuState();

    MenuTree tree;
    SongDatabase database;

    std::vector<std::string> items;
    int32_t selectedIndex{0};
    std::vector<Label*> slotLabels;
    static constexpr int32_t VISIBLE_SLOTS = 9;
    static constexpr int32_t CENTER_SLOT = VISIBLE_SLOTS / 2;
};

#endif