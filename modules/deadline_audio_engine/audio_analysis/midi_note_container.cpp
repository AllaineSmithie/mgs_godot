#include "midi_note_container.h"

// ===========================================================================
// MIDI Note Node
void MIDINoteNode::_bind_methods()
{

}

void MIDINoteNode::_notification(int p_what)
{
	switch (p_what)
	{
	case NOTIFICATION_READY:
	{

		set_width(subdivision);
		if (container)
		{
			container->set_position(Point2(container->get_position().x, -subdivision / 2.0));
			container->set_size(Vector2(container->get_size().x, subdivision));
		}
		if (collision)
		{
			SegmentShape2D* segment_shape = memnew(SegmentShape2D);
			collision->set_shape(segment_shape);
		}
		// we need to update children with these :
		set_length(length);
		set_velocity(velocity);
	}
	default:
		break;
	}
}

void MIDINoteNode::_on_left_gui_input(const Ref<InputEvent>& p_event)
{
	Ref<InputEventMouseButton> event_mouse_button = p_event;
	if (event_mouse_button.is_valid())
	{
		if (event_mouse_button->get_button_index() == MouseButton::RIGHT)
			queue_free();
		if (event_mouse_button->get_button_index() == MouseButton::LEFT)
		{
			resizing = event_mouse_button->is_pressed();
			if (event_mouse_button->is_pressed())
				fix_point_for_resize(true, event_mouse_button->get_global_position().x);
		}
	}
	Ref<InputEventMouseMotion> event_mouse_motion = p_event;
	if (event_mouse_motion.is_valid() && resizing)
		resize(event_mouse_motion->get_global_position().x);
}

void MIDINoteNode::_on_right_gui_input(const Ref<InputEvent>& p_event)
{
	Ref<InputEventMouseButton> event_mouse_button = p_event;
	if (event_mouse_button.is_valid())
	{
		if (event_mouse_button->get_button_index() == MouseButton::RIGHT)
			queue_free();
		if (event_mouse_button->get_button_index() == MouseButton::LEFT)
		{
			resizing = event_mouse_button->is_pressed();
			if (event_mouse_button->is_pressed())
				fix_point_for_resize(false, event_mouse_button->get_global_position().x);
		}
	}
	Ref<InputEventMouseMotion> event_mouse_motion = p_event;
	if (event_mouse_motion.is_valid() && resizing)
		resize(event_mouse_motion->get_global_position().x);
}

void MIDINoteNode::_on_middle_gui_input(const Ref<InputEvent>& p_event)
{
	Ref<InputEventMouseButton> event_mouse_button = p_event;
	if (event_mouse_button.is_valid())
	{
		if (event_mouse_button->get_button_index() == MouseButton::RIGHT)
			queue_free();
		if (event_mouse_button->get_button_index() == MouseButton::LEFT)
			changing_velocity = event_mouse_button->is_pressed();
	}
	Ref<InputEventMouseMotion> event_mouse_motion = p_event;
	if (event_mouse_motion.is_valid() && changing_velocity)
		set_velocity(velocity - Math::ceil(event_mouse_motion->get_relative().y));
}

void MIDINoteNode::set_velocity(int p_velocity)
{
	velocity = p_velocity;
	Color dc = get_default_color();
	dc.a = velocity + 128;
	if (middle)
		middle->set_tooltip_text(String::num(velocity, 0));
}

void MIDINoteNode::set_note(int p_note)
{
	note = p_note;
	/*Point2 _position = get_position();
	_position.y = (127.5 - note) * subdivision;
	set_position(_position);*/
}

void MIDINoteNode::set_begin(float p_begin)
{
	begin = p_begin;
	Point2 _position = get_position();
	_position.x = begin;
	set_position(_position);
}

void MIDINoteNode::set_length(float p_length)
{
	length = p_length >= 3 ? p_length : 3.0;
	PackedVector2Array points = get_points();

	Vector2 point1 = points[1];
	point1.x = length;
	points.set(1, point1);
	if (container && collision)
	{
		container->set_size(Size2(length, container->get_size().y));
		//Ref<Shape2D> shape = collision->get_shape();
		//shape->
		//collision->set_shape(); shape.b = Vector2(length, 0)]
	}
}

void MIDINoteNode::fix_point_for_resize(bool is_resizing_left, float original_position)
{
	resize_fix_point = is_resizing_left ? begin + length : begin;
	original_mouse_position = original_position;
	length_before_resize = is_resizing_left ? -length : length;
}

void MIDINoteNode::resize(float current_mouse_position)
{
	float mouse_delta = (current_mouse_position - original_mouse_position) / get_global_transform().get_scale().x + length_before_resize;
	if (mouse_delta > 0)
	{
		set_begin(resize_fix_point);
		set_length(mouse_delta);
	}
	else
	{
		set_begin(resize_fix_point + mouse_delta);
		set_length(-mouse_delta);
	}
}

MIDINoteNode::MIDINoteNode()
{
	PackedVector2Array points = get_points();
	points.append(Vector2(0, 0));
	points.append(Vector2(1, 0));
	set_points(points);
	set_default_color(Color(1.0, 0.784314, 0.501961, 1.0));
	
	container = memnew(HBoxContainer);
	container->set_offset(SIDE_RIGHT, 40.0);
	container->set_offset(SIDE_BOTTOM, 40.0);
	add_child(container);

	left = memnew(Control);
	left->set_offset(SIDE_RIGHT, 13.0);
	left->set_offset(SIDE_BOTTOM, 40.0);
	left->set_default_cursor_shape(Control::CURSOR_HSIZE);
	add_child(left);

	middle = memnew(Control);
	middle->set_offset(SIDE_LEFT, 13.0);
	middle->set_offset(SIDE_RIGHT, 26.0);
	middle->set_offset(SIDE_BOTTOM, 40.0);
	middle->set_default_cursor_shape(Control::CURSOR_POINTING_HAND);
	add_child(middle);

	right = memnew(Control);
	right->set_offset(SIDE_LEFT, 13.0);
	right->set_offset(SIDE_RIGHT, 40.0);
	right->set_offset(SIDE_BOTTOM, 40.0);
	right->set_default_cursor_shape(Control::CURSOR_HSIZE);
	add_child(right);

	area = memnew(Area2D);
	area->set_position(Vector2(-3.0, 0.0));
	add_child(area);

	collision = memnew(CollisionShape2D);
	area->add_child(collision);
}

PackedVector3Array MIDINoteTrackContainer::get_notes()
{
	//these go to midi_saver.gd
	PackedVector3Array notes;
	for (auto i = 0; i < get_child_count(); ++i)
	{
		MIDINoteNode* prnote = Object::cast_to<MIDINoteNode>(get_child(i));
		notes.append(Vector3(prnote->get_begin(), prnote->get_note(), prnote->get_velocity()));
		notes.append(Vector3(prnote->get_begin() + prnote->get_length(), prnote->get_note(), 0));
	}
	return notes;
}

void MIDINoteTrackContainer::add_notes(const PackedInt32Array& p_notes)
{
	ERR_FAIL_COND(p_notes.size() % 4 != 0);
	//these come from spectrum_analyzer.cpp
	for (auto i = 0; i < p_notes.size() / 4; ++i)
	{
		const int midi_note = p_notes[4 * i + 2];
		if (midi_note_idx != midi_note)
			continue;
		MIDINoteNode* note = memnew(MIDINoteNode);
		note->set_begin(p_notes[4 * i]);
		note->set_length(p_notes[4 * i + 1] - note->get_begin());
		note->set_note(midi_note);
		note->set_velocity(p_notes[4 * i + 3]);
		add_child(note);
	}
}

void MIDINoteTrackContainer::remove_notes()
{
	for (auto i = 0; i < get_child_count(); ++i)
	{
		MIDINoteNode* prnote = Object::cast_to<MIDINoteNode>(get_child(i));
		if (prnote)
			prnote->queue_free();
	}
}

void MIDINoteTrackContainer::_notification(int p_what)
{
	
}

void MIDINoteTrackContainer::_on_graph_spacer_gui_input(const Ref<InputEvent>& p_event)
{
	if (!graph_area)
		return;

	Ref<InputEventMouseButton> event_mouse_button = p_event;
	if (event_mouse_button.is_valid() && (event_mouse_button->get_button_index() == MouseButton::LEFT))
	{
		if (event_mouse_button->is_pressed())
		{
			Vector2 note_position = event_mouse_button->get_position() / graph_area->get_scale();
			MIDINoteNode* note = memnew(MIDINoteNode);
			note->set_note(midi_note_idx);
			note->set_begin(note_position.x);
			note->fix_point_for_resize(false, event_mouse_button->get_global_position().x);
			note_being_added = note;
			add_child(note);
		}
		else
		{
			note_being_added = nullptr;
		}
	}
}

void MIDINoteTrackContainer::update_y_position(int p_height)
{
	Vector2 pos = get_position();
	pos.y = (double)p_height / 127.0 * midi_note_idx;
	set_position(pos);
	for (auto i = 0; i < get_child_count(); ++i)
	{
		MIDINoteNode* prnote = Object::cast_to<MIDINoteNode>(get_child(i));
		Vector2 midi_note_pos = prnote->get_position();
		midi_note_pos.y = 0;
		prnote->set_position(midi_note_pos);
		subdivisions = (double)p_height / 128.0;
		prnote->set_width(subdivisions);
	}
}

void MIDINoteTrackContainer::update_x_positions(int p_width)
{
	Vector2 pos = get_position();
	pos.x = (double)p_width / 127.0 * midi_note_idx;
	set_position(pos);
	for (auto i = 0; i < get_child_count(); ++i)
	{
		MIDINoteNode* prnote = Object::cast_to<MIDINoteNode>(get_child(i));
		Vector2 midi_note_pos = prnote->get_position();
		midi_note_pos.y = 0;
		prnote->set_position(midi_note_pos);
	}
}

MIDINoteTrackContainer::MIDINoteTrackContainer(int p_midi_note_idx)
{
	midi_note_idx = p_midi_note_idx;
}
