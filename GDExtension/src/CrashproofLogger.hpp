#ifndef CrashproofLogger_hpp
#define CrashproofLogger_hpp

#include <cstdio>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <mutex>

class CrashproofLogger
{
public:
    static void open(const godot::String& p_path)
    {
        godot::String abs = godot::ProjectSettings::get_singleton()->globalize_path(p_path);
        std::lock_guard<std::mutex> lock(mtx());
        if (file())
            fclose(file());
        file() = fopen(abs.utf8().get_data(), "a");
        if (file())
            setvbuf(file(), nullptr, _IONBF, 0); // unbuffered
    }

    static void write(const godot::String& p_msg)
    {
        godot::UtilityFunctions::print(p_msg);
        std::lock_guard<std::mutex> lock(mtx());
        if (file())
        {
            fprintf(file(), "%s\n", p_msg.utf8().get_data());
            fflush(file()); // safe against process crash
        }
    }

private:
    static FILE*& file()
    {
        static FILE* f = nullptr;
        return f;
    }
    static std::mutex& mtx()
    {
        static std::mutex m;
        return m;
    }
};

#endif
