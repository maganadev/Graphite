#ifndef JsonKeys_hpp
#define JsonKeys_hpp

// JSON key constants matching TJAParser C# format
constexpr const char* JC_TITLE = "a";
constexpr const char* JC_SUBTITLE = "b";
constexpr const char* JC_WAVE = "c";
constexpr const char* JC_DEMOSTART_F = "d";
constexpr const char* JC_DEMOSTART_PS = "e";
constexpr const char* JC_DEFAULTBPM_F = "f";
constexpr const char* JC_DEFAULTBPM_D = "g";
constexpr const char* JC_DEFAULTOFFSET_F = "h";
constexpr const char* JC_DEFAULTOFFSET_PS = "i";
constexpr const char* JC_COURSES = "j";
constexpr const char* JC_COURSE = "k";
constexpr const char* JC_LEVEL = "l";
constexpr const char* JC_REDNOTES = "m";
constexpr const char* JC_BLUENOTES = "n";
constexpr const char* JC_BIGREDNOTES = "o";
constexpr const char* JC_BIGBLUENOTES = "p";
constexpr const char* JC_YELLOWNOTES = "q";
constexpr const char* JC_BIGYELLOWNOTES = "r";
constexpr const char* JC_GREENNOTES = "s";
constexpr const char* JC_MEASURES = "t";
constexpr const char* JC_STARTTIME_F = "u";
constexpr const char* JC_STARTTIME_PS = "v";
constexpr const char* JC_STOPTIME_F = "w";
constexpr const char* JC_STOPTIME_PS = "x";
constexpr const char* JC_BPMSCROLL_F = "y";
constexpr const char* JC_BPMSCROLL_D = "z";
constexpr const char* JC_GREENNOTEHITS = "0";
constexpr const char* JC_SCROLL = "1";
constexpr const char* JC_GOGO = "2";
constexpr const char* JC_BARLINEVISIBLE = "3";
constexpr const char* JC_TEXT = "4";

// Note type labels used for internal dispatch
constexpr const char* NT_RED = "red";
constexpr const char* NT_BLUE = "blue";
constexpr const char* NT_YELLOW = "yellow";
constexpr const char* NT_GREEN = "green";

#endif