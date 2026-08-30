#ifndef BlueNotePrefab_hpp
#define BlueNotePrefab_hpp

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

#include "BlueNote.hpp"

using namespace ::godot;

class BlueNotePrefab : public Node2D
{
    GDCLASS(BlueNotePrefab, Node2D)

protected:
    static void _bind_methods();

public:
    BlueNotePrefab();
    ~BlueNotePrefab();

    void setNote(const Note& note);
    const Note& getNote() const;

    bool isJudged() const;
    void setJudged(NoteGradings grading);
    NoteGradings getGrading() const;

    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

    BlueNote* getNote();

private:
    BlueNote m_note;
};

#endif
