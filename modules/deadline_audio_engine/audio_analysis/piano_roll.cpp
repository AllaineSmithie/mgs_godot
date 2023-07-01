#include "piano_roll.h"

void PianoRollBar::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("note_pressed", PropertyInfo(Variant::INT, "note_playing"), PropertyInfo(Variant::INT, "velocity")));
	ADD_SIGNAL(MethodInfo("zoom", PropertyInfo(Variant::BOOL, "zoom_in")));
}

void PianoRollBar::_notification(int p_what)
{
	switch (p_what)
	{
	case NOTIFICATION_ENTER_TREE:
	{
		break;
	}
	case NOTIFICATION_DRAW:
	{
		_draw();
		break;
	}
	default:
		break;
	}
}

void PianoRollBar::gui_input(const Ref<InputEvent>& p_event)
{
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_null())
		return;

	if (mb->get_button_index() != MouseButton::WHEEL_UP && mb->get_button_index() != MouseButton::WHEEL_DOWN)
		return;


	if (/*mb->is_command_or_control_pressed() || */ mb->is_shift_pressed())
	{
		const bool zoom_in = mb->get_button_index() == MouseButton::WHEEL_UP ? true : false;
		//zoom_slider->gui_input(p_event);
		emit_signal("zoom", zoom_in);
	}
}

void PianoRollBar::_draw()
{
	if (get_size().y == 0)
		return;
	Ref<Font> font = get_theme_font("font", "Label");
	float one_key_height = get_size().y / 128.0;

	for (auto note = 0; note < 128; ++note)
	{
		Vector2 begin = Vector2(0, one_key_height * (127 - note));
		Vector2 end = Vector2(get_size().x, one_key_height * (127 - note));
		Vector2 offset = Vector2(0, one_key_height * 0.5);
		//if it's a black key
		switch (note % 12)
		{
		case 0:
			draw_string(font, end * Vector2(0.25, 1) + offset + Vector2(0, 5), "C" + String::num(note / 12 - 3, 0), HORIZONTAL_ALIGNMENT_CENTER, -1.0f, 12, Color(0.05, 0.05, 0.05));
			break;
		case 1:
		case 3: 
		case 6:
		case 8:
		case 10:
			draw_line(begin + offset, (end + offset) * Vector2(0.75, 1), Color(0.05, 0.05, 0.05), one_key_height);
			draw_line(begin + offset, end + offset, Color(0.05, 0.05, 0.05));
			break;
		case 4:
		case 11:
			draw_line(begin, end, Color(0.05, 0.05, 0.05));
		default:
			break;
		}
	}
}

void PianoRollBar::_gui_input(const Ref<InputEvent>& p_event)
{
	Ref<InputEventMouseButton> event_mouse_button = p_event;
	if (event_mouse_button.is_valid() && event_mouse_button->get_button_index() == MouseButton::LEFT)
	{
		if (event_mouse_button->is_pressed())
		{
			note_playing = 127 - int(128 * event_mouse_button->get_position().y / get_size().y);
			emit_signal("note_pressed", note_playing, 127);
		}
		else
		{
			emit_signal("note_pressed", note_playing, 0);
		}
	}
}

PianoRollBar::PianoRollBar()
{
}
