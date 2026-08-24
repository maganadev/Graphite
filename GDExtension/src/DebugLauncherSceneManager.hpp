#ifndef DebugLauncherSceneManager_hpp
#define DebugLauncherSceneManager_hpp

#include <godot_cpp/classes/sprite2d.hpp>

#include "Globals.hpp"

using namespace ::godot;

class DebugLauncherSceneManager : public Sprite2D
{
    GDCLASS(DebugLauncherSceneManager, Sprite2D)

protected:
    static void _bind_methods();

public:
    DebugLauncherSceneManager();
    ~DebugLauncherSceneManager();
    void _ready() override;
    void _process(double delta) override;
};

#endif