#ifndef JsonKeys_hpp
#define JsonKeys_hpp

// --- Top-level chart keys ---
static constexpr const char* JC_TITLE = "A";
static constexpr const char* JC_SUBTITLE = "s";
static constexpr const char* JC_TITLEJA = "g";
static constexpr const char* JC_TITLEZH = "k";
static constexpr const char* JC_SUBTITLEJA = "Z";
static constexpr const char* JC_SUBTITLEZH = "u";
static constexpr const char* JC_WAVE = "i";
static constexpr const char* JC_DEMOSTART_F = "M";
static constexpr const char* JC_DEFAULTBPM_F = "W";
static constexpr const char* JC_DEFAULTOFFSET_F = "n";
static constexpr const char* JC_COURSES = "C";

// --- Per-course keys ---
static constexpr const char* JC_COURSE = "E";
static constexpr const char* JC_LEVEL = "V";
static constexpr const char* JC_SCOREINIT = "y";
static constexpr const char* JC_SCOREDIFF = "B";
static constexpr const char* JC_BALLOON = "O";

// --- Note array keys ---
static constexpr const char* JC_REDNOTES = "p";
static constexpr const char* JC_BLUENOTES = "j";
static constexpr const char* JC_YELLOWNOTES = "Y";
static constexpr const char* JC_GREENNOTES = "J";
static constexpr const char* JC_GHOSTNOTES = "f";
static constexpr const char* JC_BIGREDNOTES = "H";
static constexpr const char* JC_BIGBLUENOTES = "o";
static constexpr const char* JC_BIGYELLOWNOTES = "a";
static constexpr const char* JC_BIGGREENNOTES = "Q";
static constexpr const char* JC_BIGGHOSTNOTES = "R";

// --- Note object keys ---
static constexpr const char* JC_STARTTIME_F = "l";
static constexpr const char* JC_STOPTIME_F = "m";
static constexpr const char* JC_BPMSCROLL_F = "T";
static constexpr const char* JC_GREENNOTEHITS = "v";

// --- Measure keys ---
static constexpr const char* JC_MEASURES = "F";
static constexpr const char* JC_SCROLL = "h";
static constexpr const char* JC_GOGO = "N";
static constexpr const char* JC_BARLINE = "x";
static constexpr const char* JC_TEXT = "z";

// --- Note type identifiers (internal, not JSON keys) ---
static constexpr const char* NT_RED = "red";
static constexpr const char* NT_BLUE = "blue";
static constexpr const char* NT_YELLOW = "yellow";
static constexpr const char* NT_GREEN = "green";
static constexpr const char* NT_GHOST = "ghost";

#endif
