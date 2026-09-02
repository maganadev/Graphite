#ifndef ResultsScreenSceneManager_hpp
#define ResultsScreenSceneManager_hpp

#include <godot_cpp/classes/sprite2d.hpp>

#include "GameManager.hpp"

using namespace ::godot;

class ResultsScreenSceneManager : public Sprite2D
{
    GDCLASS(ResultsScreenSceneManager, Sprite2D)

protected:
    static void _bind_methods();

public:
    ResultsScreenSceneManager();
    ~ResultsScreenSceneManager();
    void _ready() override;
    void _process(double delta) override;
};

#endif