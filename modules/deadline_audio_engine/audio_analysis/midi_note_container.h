#ifndef MIDI_NOTE_CONTAINER_H
#define MIDI_NOTE_CONTAINER_H

#include <scene/2d/area_2d.h>
#include <scene/2d/collision_shape_2d.h>
#include <scene/2d/line_2d.h>
#include <scene/2d/node_2d.h>
#include <scene/gui/box_container.h>
#include <scene/gui/scroll_container.h>
#include <scene/gui/button.h>
#include <scene/gui/center_container.h>
#include <scene/resources/segment_shape_2d.h>
// =================================================================
class MIDINoteNode : public Line2D
{
	GDCLASS(MIDINoteNode, Line2D);

	Control* left = nullptr;
	Control* middle = nullptr;
	Control* right = nullptr;
	CollisionShape2D* collision = nullptr;
	Area2D* area = nullptr;
	HBoxContainer* container = nullptr;

	int subdivision = 9;
	int velocity = 127;
	int note = 0;
	float begin = 0.0f;
	float length = 0.0f;

	bool changing_velocity = false;
	bool resizing = false;
	float resize_fix_point = 0.0f;
	float length_before_resize = 0.0f;
	float original_mouse_position = 0.0f;

protected:
	static void _bind_methods();
public:

protected:
	void _notification(int p_what);
public:

	void _on_left_gui_input(const Ref<InputEvent>& p_event);
	void _on_right_gui_input(const Ref<InputEvent>& p_event);
	void _on_middle_gui_input(const Ref<InputEvent>& p_event);

	void set_velocity(int p_velocity);
	int get_velocity() { return velocity; }
	void set_note(int p_note);
	int get_note() { return note; }
	void set_begin(float p_begin);
	float get_begin() { return begin; }
	void set_length(float p_length);
	float get_length() { return length; }
	void fix_point_for_resize(bool is_resizing_left, float original_position);
	void resize(float current_mouse_position);

	MIDINoteNode();
};

// =================================================================
class MIDINoteTrackContainer : public Node2D
{
	GDCLASS(MIDINoteTrackContainer, Node2D);
	MIDINoteNode* note_being_added = nullptr;
	int subdivisions = 9;
	int midi_note_idx = 0;
protected:
	static void _bind_methods() {}
	void _notification(int p_what);
public:
	Node2D* graph_area = nullptr;
	void _on_graph_spacer_gui_input(const Ref<InputEvent>& p_event);
	void update_y_position(int p_height);
	void update_x_positions(int p_width);
	void remove_notes();
	void add_notes(const PackedInt32Array& notes);
	PackedVector3Array get_notes();
	MIDINoteTrackContainer(int midi_note_idx = 0);
};



#endif // MIDI_NOTE_CONTAINER_H
