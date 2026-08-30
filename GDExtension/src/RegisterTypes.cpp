#include <Windows.h>
#define DO_NOT_REORDER_ABOVE
#include "RegisterTypes.hpp"
#include "DebugLauncherSceneManager.hpp"
#include "GameManager.hpp"
#include "GameplaySceneManager.hpp"
#include "RedNotePrefab.hpp"
#include "BlueNotePrefab.hpp"
#include "YellowNotePrefab.hpp"
#include "GreenNotePrefab.hpp"
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
using namespace ::godot;

void initialize_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }

    GDREGISTER_RUNTIME_CLASS(DebugLauncherSceneManager);
    GDREGISTER_RUNTIME_CLASS(GameManager);
    GDREGISTER_RUNTIME_CLASS(GameplaySceneManager);
    GDREGISTER_RUNTIME_CLASS(RedNotePrefab);
    GDREGISTER_RUNTIME_CLASS(BlueNotePrefab);
    GDREGISTER_RUNTIME_CLASS(YellowNotePrefab);
    GDREGISTER_RUNTIME_CLASS(GreenNotePrefab);
}

void uninitialize_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
}

extern "C"
{
    // Initialization.
    GDExtensionBool GDE_EXPORT libGraphiteGDExtensionInit(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_module);
        init_obj.register_terminator(uninitialize_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}