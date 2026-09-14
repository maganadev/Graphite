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
    YellowNote,
    GreenNote,
    RedNoteLarge,
    BlueNoteLarge,
    GhostNote,
};

enum class NoteGradings : uint8_t
{
    Ungraded,
    Early_OutOfRange,
    Early_AboutToBeOutOfRange,
    Early_Fuka,
    Early_Ka,
    Early_Ryou,
    Early_Chou,
    CompletelyPerfect,
    Late_Chou,
    Late_Ryou,
    Late_Ka,
    Late_Fuka,
    Late_AboutToBeOutOfRange,
    Late_OutOfRange,
};
#endif
