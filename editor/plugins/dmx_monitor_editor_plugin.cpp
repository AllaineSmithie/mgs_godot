/*************************************************************************/
/*  dmx_monitor_editor_plugin.cpp                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "dmx_monitor_editor_plugin.h"

#include "core/config/project_settings.h"
#include "editor/editor_scale.h"
#include "editor/editor_settings.h"

#include "scene/gui/separator.h"
#include "scene/main/window.h"
#include "servers/rendering_server.h"


void DMXMonitor::_select_universe_by_name(const String& p_universename)
{
	int idx = -1;
	for (int i = 0; i < universes->get_item_count(); i++) {
		if (universes->get_item_text(i) == p_universename) {
			idx = i;
			break;
		}
	}

	ERR_FAIL_COND(idx == -1);

	universes->select(idx);
	_dmx_universe_selected(idx);
}

void DMXMonitor::_dmx_universe_selected(int p_which)
{
	if (updating) {
		return;
	}
	// when selecting an animation, the idea is that the only interesting behavior
	// ui-wise is that it should play/blend the next one if currently playing
	int current = -1;
	if (universes->get_selected() >= 0 && universes->get_selected() < universes->get_item_count()) {
		current = universes->get_selected();
	}
	
	if (current > -1) {
		universe_display->set_assigned_dmx_universe(current);
	}
	else {
		universe_display->set_assigned_dmx_universe(0);
	}


	//EditorNode::get_singleton()->update_keying();
}

void DMXMonitor::_play_pressed()
{
}

void DMXMonitor::_stop_pressed()
{
}

void DMXMonitor::_list_changed()
{
}

void DMXMonitor::_update_universe()
{
}

void DMXMonitor::_update_monitor()
{
}

void DMXMonitor::_dmx_universe_changed(int idx)
{
}

void DMXMonitor::_unhandled_key_input(const Ref<InputEvent>& p_ev)
{
}

void DMXMonitor::_editor_visibility_changed()
{
}

DMXMonitor::~DMXMonitor()
{
}

void DMXMonitor::_bind_methods()
{
}

DMXMonitorUniverse* DMXMonitor::get_universe_display() const
{
	return universe_display;
}

Dictionary DMXMonitor::get_state() const {
	Dictionary d;

	d["visible"] = is_visible_in_tree();
	if (EditorNode::get_singleton()->get_edited_scene() && is_visible_in_tree() && universe_display) {
		d["universe_display"] = EditorNode::get_singleton()->get_edited_scene()->get_path_to(universe_display);
		d["universe"] = universe_display->get_assigned_dmx_universe();
		d["universe_display_state"] = universe_display->get_state();
	}

	return d;
}
void DMXMonitor::set_state(const Dictionary& p_state) {
	if (!p_state.has("visible") || !p_state["visible"]) {
		return;
	}
	if (!EditorNode::get_singleton()->get_edited_scene()) {
		return;
	}

	if (p_state.has("universe_display")) {
		Node* n = EditorNode::get_singleton()->get_edited_scene()->get_node(p_state["player"]);
		if (Object::cast_to<DMXMonitorUniverse>(n) && EditorNode::get_singleton()->get_editor_selection()->is_selected(n)) {
			universe_display = Object::cast_to<DMXMonitorUniverse>(n);
			_update_monitor();
			editor->make_bottom_panel_item_visible(this);
			set_process(true);

			if (p_state.has("universe")) {
				int universeidx = p_state["universe"];
				if (universeidx > -1)
				{
					universes->select(universeidx);
					_dmx_universe_selected(universeidx);
				}
			}
		}
	}

	if (p_state.has("universe_display_state")) {
		universe_display->set_state(p_state["universe_display_state"]);
	}
}

DMXMonitor* DMXMonitor::singleton = nullptr;

DMXMonitor::DMXMonitor(EditorNode* p_editor, DMXMonitorEditorPlugin* p_plugin)
{
	editor = p_editor;
	plugin = p_plugin;
	singleton = this;

	updating = false;

	set_focus_mode(FOCUS_ALL);

	universe_display = nullptr;

	HBoxContainer* hb = memnew(HBoxContainer);
	add_child(hb);

	stop = memnew(Button);
	stop->set_toggle_mode(true);
	hb->add_child(stop);
	stop->set_tooltip_text(TTR("Stop DMX updates. (S)"));

	play = memnew(Button);
	play->set_tooltip_text(TTR("Continously update DMX values (Shift+D)"));
	hb->add_child(play);


	update_fps = memnew(SpinBox);
	hb->add_child(update_fps);
	update_fps->set_custom_minimum_size(Size2(80, 0) * EDSCALE);
	update_fps->set_stretch_ratio(2);
	update_fps->set_step(1.0);
	update_fps->set_tooltip_text(TTR("Update-rate (per second)."));

	hb->add_child(memnew(VSeparator));


	universes = memnew(OptionButton);
	hb->add_child(universes);
	universes->set_h_size_flags(SIZE_EXPAND_FILL);
	universes->set_tooltip_text(TTR("Display list of all known universes."));
	universes->set_clip_text(true);


	hb->add_child(memnew(VSeparator));


	pin = memnew(Button);
	pin->set_flat(true);
	pin->set_toggle_mode(true);
	pin->set_tooltip_text(TTR("Pin DMX Monitor"));
	hb->add_child(pin);
	pin->connect("pressed", callable_mp(this, &DMXMonitor::_pin_pressed));


	name_title = memnew(Label(TTR("DMX Universe:")));
	hb->add_child(name_title);

	play->connect("pressed", callable_mp(this, &DMXMonitor::_play_pressed));
	stop->connect("pressed", callable_mp(this, &DMXMonitor::_stop_pressed));

	last_active = false;

	set_process_unhandled_key_input(true);


}

void DMXMonitorEditorPlugin::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			set_force_draw_over_forwarding_enabled();
		} break;
	}
}

void DMXMonitorEditorPlugin::edit(Object *p_object) {
	
}

bool DMXMonitorEditorPlugin::handles(Object *p_object) const {
	return true;//p_object->is_class("DMX");
}

void DMXMonitorEditorPlugin::make_visible(bool p_visible) {
	if (p_visible) {
		editor->make_bottom_panel_item_visible(dmx_monitor);
		dmx_monitor->set_process(true);
	}
}

DMXMonitorEditorPlugin::DMXMonitorEditorPlugin(EditorNode *p_node) {
	editor = p_node;
	dmx_monitor = memnew(DMXMonitor(editor, this));
	//dmx_monitor->set_undo_redo(EditorNode::get_undo_redo());
	editor->add_bottom_panel_item(TTR("DMX Monitor"), dmx_monitor);
}

DMXMonitorEditorPlugin::~DMXMonitorEditorPlugin() {
}

void DMXMonitorUniverse::_dmx_values_update()
{
}

int DMXMonitorUniverse::_get_track_selected()
{
	return selected_track;
}

void DMXMonitorUniverse::_update_tracks()
{
}

void DMXMonitorUniverse::_track_remove_request(int p_track)
{
}

void DMXMonitorUniverse::_track_grab_focus(int p_track)
{
}

void DMXMonitorUniverse::_add_track(int p_type)
{
}

void DMXMonitorUniverse::_root_removed()
{
}

PropertyInfo DMXMonitorUniverse::_find_hint_for_track(int p_idx, NodePath& r_base_path, Variant* r_current_val)
{
	return PropertyInfo();
}

void DMXMonitorUniverse::_clear_selection(bool p_update)
{
}

void DMXMonitorUniverse::_bind_methods()
{
}

void DMXMonitorUniverse::set_assigned_dmx_universe(int p_dmx_index)
{
	selected_track = p_dmx_index;
}

Array DMXMonitorUniverse::get_assigned_dmx_universe() const
{
	return Array();
}

Dictionary DMXMonitorUniverse::get_state() const
{
	return Dictionary();
}

void DMXMonitorUniverse::set_state(const Dictionary& p_state)
{
}

DMXMonitorUniverse::DMXMonitorUniverse()
{
}

DMXMonitorUniverse::~DMXMonitorUniverse()
{
}
