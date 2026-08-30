#ifndef GreenNotePrefab_hpp
#define GreenNotePrefab_hpp

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

#include "GreenNote.hpp"

using namespace ::godot;

class GreenNotePrefab : public Node2D
{
    GDCLASS(GreenNotePrefab, Node2D)

protected:
    static void _bind_methods();

public:
    GreenNotePrefab();
    ~GreenNotePrefab();

    void setNote(const Note& note);
    const Note& getNote() const;

    bool isJudged() const;
    void setJudged(NoteGradings grading);
    NoteGradings getGrading() const;

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

    GreenNote* getNote();

private:
    GreenNote m_note;
};

#endif
