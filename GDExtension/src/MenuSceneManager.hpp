#ifndef MenuSceneManager_hpp
#define MenuSceneManager_hpp

#include <godot_cpp/classes/sprite2d.hpp>

#include "GameManager.hpp"

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
};

#endif