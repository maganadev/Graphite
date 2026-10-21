#ifndef RegisterTypes_hpp
#define RegisterTypes_hpp

#ifdef _WIN32
#include <Windows.h>
#define DO_NOT_REORDER_ABOVE
#endif

#include <godot_cpp/core/class_db.hpp>
using namespace ::godot;

void initialize_module(ModuleInitializationLevel p_level);
void uninitialize_module(ModuleInitializationLevel p_level);

#endif