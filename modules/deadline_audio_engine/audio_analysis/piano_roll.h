#ifndef PIANO_ROLL_H
#define PIANO_ROLL_H

#include <scene/gui/panel_container.h>
#include <scene/gui/button.h>
#include <scene/gui/center_container.h>

// =================================================================
class PianoRollBar : public PanelContainer
{
	GDCLASS(PianoRollBar, PanelContainer);
	int note_playing = -1;
	void gui_input(const Ref<InputEvent>& p_event) override;
protected:
	static void _bind_methods();
	void _notification(int p_what);
	void _draw();
	void _gui_input(const Ref<InputEvent>& p_event);
public:
	PianoRollBar();
};

#endif // PIANO_ROLL_H
