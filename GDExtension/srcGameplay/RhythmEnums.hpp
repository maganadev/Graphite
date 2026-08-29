#ifndef RhythmEnums_hpp
#define RhythmEnums_hpp
#include <cstdint>

enum class DrumButtons : uint8_t
{
    DrumBlueLeft,
    DrumRedLeft,
    DrumRedRight,
    DrumBlueRight,
};

enum class NoteTypes : uint8_t
{
    RedNoteSmall,
    BlueNoteSmall,
    RedNoteLarge,
    BlueNoteLarge,
    LongNote,
    BalloonNote,
};

enum class NoteGradings : uint8_t
{
    Ungraded,
    Early_OutOfRange,
    Early_AboutToBeOOR,
    Early_Fuka,
    Early_Ka,
    Early_Ryou,
    Early_Chou,
    CompletlelyPerfect,
    Late_Chou,
    Late_Ryou,
    Late_Ka,
    Late_Fuka,
    Late_AboutToBeOOR,
    Late_OutOfRange,
};
#endif
