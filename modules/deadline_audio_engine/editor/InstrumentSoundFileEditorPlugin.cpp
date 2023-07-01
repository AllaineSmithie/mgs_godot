/**************************************************************************/
/*  version_control_editor_plugin.cpp                                     */
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

#include "InstrumentSoundFileEditorPlugin.h"

#include "core/config/project_settings.h"
#include "core/os/keyboard.h"
#include "core/os/time.h"
#include "editor/editor_file_system.h"
#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "editor/editor_settings.h"
#include "editor/filesystem_dock.h"
#include "editor/plugins/script_editor_plugin.h"
#include "scene/gui/separator.h"


InstrumentSoundFileEditorPlugin* InstrumentSoundFileEditorPlugin::singleton = nullptr;

//
void InstrumentSoundFileEditorPlugin::_bind_methods()
{
	// No binds required so far.
}



InstrumentSoundFileEditorPlugin* InstrumentSoundFileEditorPlugin::get_singleton() {
	return singleton ? singleton : memnew(InstrumentSoundFileEditorPlugin);
}

void InstrumentSoundFileEditorPlugin::register_editor()
{
	EditorNode::get_singleton()->add_control_to_dock(EditorNode::DOCK_SLOT_RIGHT_UL, mic_positions_dock);

	mic_positions_dock_button = EditorNode::get_singleton()->add_bottom_panel_item(TTR("Mic Group"), mic_positions_dock);
}

void InstrumentSoundFileEditorPlugin::shut_down()
{
	if (!EditorVCSInterface::get_singleton()) {
		return;
	}

	
	EditorNode::get_singleton()->remove_control_from_dock(mic_positions_dock);

}

InstrumentSoundFileEditorPlugin::InstrumentSoundFileEditorPlugin()
{
	singleton = this;

	mic_positions_dock = memnew(VBoxContainer);
	mic_positions_dock->set_visible(false);
	mic_positions_dock->set_name(TTR("Mic Groups"));

}

InstrumentSoundFileEditorPlugin::~InstrumentSoundFileEditorPlugin()
{
	shut_down();
	memdelete(mic_positions_dock);
}
