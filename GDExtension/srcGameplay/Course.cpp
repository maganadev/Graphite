#include "Course.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

void courseLogError(const std::string& message)
{
    godot::UtilityFunctions::printerr(message.c_str());
}