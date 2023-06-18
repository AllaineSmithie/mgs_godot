/**************************************************************************/
/*  dmx_monitor_editor_plugin.h                                           */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef DMX_MONITOR_EDITOR_PLUGIN_H
#define DMX_MONITOR_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
//#include "scene/dmx/dmx_monitor.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/option_button.h"
#include "scene/gui/slider.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/texture_button.h"

class DMXMonitorUniverse : public VBoxContainer {
	GDCLASS(DMXMonitorUniverse, VBoxContainer);
public:
	Node *root = nullptr;

	MenuButton *edit = nullptr;

	PanelContainer *main_panel = nullptr;
	HScrollBar *hscroll = nullptr;
	ScrollContainer *scroll = nullptr;
	VBoxContainer *track32_vbox = nullptr;
	Label *info_message = nullptr;
	HSlider *zoom = nullptr;
	TextureRect *zoom_icon = nullptr;

	Vector<Label> dmx_tracks;

	bool value_changing_awaiting_update;
	void _dmx_values_update();
	int _get_track_selected();
	void _update_tracks();

	void _track_remove_request(int p_track);
	void _track_grab_focus(int p_track);

	//UndoRedo* undo_redo;

	void _add_track(int p_type);
	int adding_track_type;
	NodePath adding_track_path;

	void _root_removed();

	PropertyInfo _find_hint_for_track(int p_idx, NodePath &r_base_path, Variant *r_current_val = nullptr);

	void _clear_selection(bool p_update = false);

	//selection
	int selected_track;

protected:
	static void _bind_methods();
	//void _notification(int p_what);

public:

	void set_assigned_dmx_universe(int p_dmx_index);
	Array get_assigned_dmx_universe() const;
	
	Dictionary get_state() const;
	void set_state(const Dictionary &p_state);

		
	DMXMonitorUniverse();
	~DMXMonitorUniverse();
};

class DMXMonitorEditorPlugin;
class DMXMonitorUniverse;

class DMXMonitor : public VBoxContainer {
	GDCLASS(DMXMonitor, VBoxContainer);

	EditorNode *editor;
	DMXMonitorEditorPlugin *plugin = nullptr;

	OptionButton *universes = nullptr;
	Button *stop = nullptr;
	Button *play = nullptr;

	Button *pin = nullptr;
	SpinBox *update_fps = nullptr;
	Label *name_title = nullptr;
	bool last_active;

	int current_universe;

	bool updating;
	
	DMXMonitorUniverse * universe_display = nullptr;
		
	void _select_universe_by_name(const String& p_universename);
	void _dmx_universe_selected(int p_which);

	void _play_pressed();
	void _stop_pressed();

	void _list_changed();
	void _update_universe();
	void _update_monitor();

	void _dmx_universe_changed(int idx);

	void _unhandled_key_input(const Ref<InputEvent> &p_ev);

	void _editor_visibility_changed();

	~DMXMonitor();

protected:
	static void _bind_methods();

public:
	DMXMonitorUniverse *get_universe_display() const;
	static DMXMonitor *singleton;

	void _pin_pressed();
	bool is_pinned() const { return pin->is_pressed(); }
	void unpin() { pin->set_pressed(false); }
	Dictionary get_state() const;
	void set_state(const Dictionary &p_state);

//	UndoRedo *undo_redo;
//	void set_undo_redo(UndoRedo *p_undo_redo) { undo_redo = p_undo_redo; }
	
	DMXMonitor(EditorNode *p_editor, DMXMonitorEditorPlugin *p_plugin);
};

class DMXMonitorEditorPlugin : public EditorPlugin {
	GDCLASS(DMXMonitorEditorPlugin, EditorPlugin);

	DMXMonitor *dmx_monitor = nullptr;
	EditorNode *editor = nullptr;

protected:
	void _notification(int p_what);

public:
	virtual Dictionary get_state() const override { return Dictionary();/* anim_editor->get_state(); */}
	virtual void set_state(const Dictionary &p_state) override { /*anim_editor->set_state(p_state);*/ }

	virtual String get_name() const override { return "DMXMonitor"; }
	bool has_main_screen() const override { return false; }
	virtual void edit(Object *p_object) override;
	virtual bool handles(Object *p_object) const override;
	virtual void make_visible(bool p_visible) override;

	virtual void forward_canvas_force_draw_over_viewport(Control *p_overlay) override { /*anim_editor->forward_force_draw_over_viewport(p_overlay);*/ }
	virtual void forward_3d_force_draw_over_viewport(Control *p_overlay) override { /*anim_editor->forward_force_draw_over_viewport(p_overlay); */}

	DMXMonitorEditorPlugin(EditorNode *p_node);
	~DMXMonitorEditorPlugin();
};

#endif // DMX_MONITOR_EDITOR_PLUGIN_H
