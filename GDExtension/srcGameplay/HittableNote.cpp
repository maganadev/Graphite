#include "HittableNote.hpp"
#include "../src/GraphiteGlobals.hpp"
#include "Fraction.hpp"

HittableNote::HittableNote()
{
}

HittableNote::~HittableNote()
{
}

Lanes HittableNote::getLane(NoteTypes noteType)
{
    switch (noteType)
    {
    case NoteTypes::BlueNoteSmall:
    case NoteTypes::BlueNoteLarge:
        return Lanes::Blue;
    default:
        return Lanes::Red;
    }
}

bool HittableNote::isOneShotNote(NoteTypes noteType)
{
    switch (noteType)
    {
    case NoteTypes::RedNoteSmall:
    case NoteTypes::RedNoteLarge:
    case NoteTypes::BlueNoteSmall:
    case NoteTypes::BlueNoteLarge:
        return true;
    default:
        return false;
    }
}

bool HittableNote::isSpamNote(NoteTypes noteType)
{
    switch (noteType)
    {
    case NoteTypes::YellowNoteSmall:
    case NoteTypes::GreenNoteSmall:
    case NoteTypes::YellowNoteLarge:
    case NoteTypes::GreenNoteLarge:
        return true;
    default:
        return false;
    }
}
