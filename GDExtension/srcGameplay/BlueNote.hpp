#ifndef BlueNote_hpp
#define BlueNote_hpp

#include <atomic>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

#include "RhythmEnums.hpp"
#include "TJACourse.hpp"

using namespace ::godot;

class BlueNote : public Node2D
{
    GDCLASS(BlueNote, Node2D)

    static constexpr double SCROLL_SPEED = 1600.0;
    static constexpr double LANE_Y = 386.0;
    static constexpr double HITZONE_CENTER_X = 618.0;

protected:
    static void _bind_methods();

public:
    BlueNote();
    ~BlueNote();

    void setEvent(const TJAEvent& event);
    const TJAEvent& getEvent() const;
    void updatePosition(int64_t songPositionPicoseconds, int64_t visualOffsetPicoseconds);

    bool isJudged() const;
    void setJudged(NoteGradings grading);
    NoteGradings getGrading() const;

private:
    TJAEvent m_event;
    std::atomic<bool> m_judged{false};
    NoteGradings m_grading{NoteGradings::Ungraded};
};

#endif