/**************************************************************************/
/*  source_editor_plugin.h                                                */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             Metro Gaya System                          */
/*                        https://deadline-entertainment.com              */
/**************************************************************************/
/* Copyright (c) 2022-present Deadline Entertainment                      */
/**************************************************************************/

#ifndef SOURCE_EDITOR_PLUGIN_H
#define SOURCE_EDITOR_PLUGIN_H

#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "editor/editor_settings.h"
#include "editor/editor_translation_parser.h"
#include "editor/window_wrapper.h"
#include "scene/gui/dialogs.h"
#include <scene/gui/box_container.h>
#include <scene/gui/button.h>
#include <scene/gui/check_button.h>
#include <scene/gui/item_list.h>
#include <scene/gui/line_edit.h>
#include <scene/gui/option_button.h>
#include <scene/gui/panel_container.h>
#include <scene/gui/spin_box.h>
#include <scene/gui/split_container.h>
#include <scene/gui/texture_rect.h>
#include <scene/main/timer.h>

// =========================================================================

class SourceEditorPluginExtension : public RefCounted {
	GDCLASS(SourceEditorPluginExtension, RefCounted);
	friend class SourceEditor;

protected:
	Node *editor_page = nullptr;

public:
	virtual void reset_editor() = 0;
	virtual bool handles(Object *p_object) const = 0;
	virtual void edit(Object *p_object) = 0;
	virtual void edited_scene_changed() = 0;
	SourceEditorPluginExtension() {}
	~SourceEditorPluginExtension();
};

// =========================================================================

class SourceEditor : public PanelContainer {
	GDCLASS(SourceEditor, PanelContainer);
	enum {
		SOURCE_FILE_OPEN,
		SOURCE_FILE_CLOSE,
		TOGGLE_SOURCE_PANEL,
		WINDOW_SELECT_BASE = 100,
	};

private:
	bool initial_loading = true;

	static Ref<Control> source_editor_plugins[1024];
	static int source_editor_plugin_ctn;

	TabContainer *tab_container = nullptr;
	HBoxContainer *menu_hb = nullptr;
	MenuButton *file_menu = nullptr;
	MenuButton *edit_menu = nullptr;
	MenuButton *batch_tools_menu = nullptr;
	Timer *autosave_timer = nullptr;

	Button *make_floating = nullptr;

	HSplitContainer *source_split = nullptr;

	bool _sort_list_on_update = false;
	bool restoring_layout = false;
	static SourceEditor *source_editor;
	WindowWrapper *source_window_wrapper = nullptr;

	bool grab_focus_block = false;
	int file_dialog_option = -1;

	EditorFileDialog *file_dialog = nullptr;
	AcceptDialog *error_dialog = nullptr;
	ConfirmationDialog *erase_tab_confirm = nullptr;
	ScriptCreateDialog *script_create_dialog = nullptr;
	Tree *disk_changed_list = nullptr;
	ConfirmationDialog *disk_changed = nullptr;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void _menu_option(int p_option);
	void _window_changed(bool p_visible);
	void _tab_changed(int p_which);
	void _close_current_tab(bool p_save = true);
	void _close_discard_current_tab(const String &p_str);
	void _file_dialog_action(String p_file);
	void _autosave();
	void _update_autosave_timer();

	void set_window_layout(Ref<ConfigFile> p_layout);
	void get_window_layout(Ref<ConfigFile> p_layout);

	static SourceEditor *get_singleton() { return source_editor; }
	SourceEditor(WindowWrapper *p_wrapper);
	~SourceEditor();
	void register_extension(Control *p_extension);
	void unregister_extension(Control *p_extension);
};

// =========================================================================

class SourceEditorPlugin : public EditorPlugin {
	GDCLASS(SourceEditorPlugin, EditorPlugin);

	static SourceEditorPlugin *singleton;
	static Vector<Ref<SourceEditorPluginExtension>> extensions;

	SourceEditor *source_editor = nullptr;
	WindowWrapper *source_window_wrapper = nullptr;
	String last_editor;

protected:
	void _notification(int p_what);

public:
	_FORCE_INLINE_ static SourceEditorPlugin *get_singleton() { return singleton; }

	SourceEditor *get_spatial_editor() { return source_editor; }
	virtual String get_name() const override { return "Edit"; }
	bool has_main_screen() const override { return true; }
	virtual void make_visible(bool p_visible) override;
	void set_window_layout(Ref<ConfigFile> p_layout) override;
	void get_window_layout(Ref<ConfigFile> p_layout) override;
	virtual void edit(Object *p_object) override;
	virtual bool handles(Object *p_object) const override;

	virtual Dictionary get_state() const override;
	virtual const Ref<Texture2D> get_icon() const override;
	virtual void set_state(const Dictionary &p_state) override;
	virtual void clear() override;

	static void register_extension(Ref<SourceEditorPluginExtension> p_extension, bool p_at_front = false);
	static void unregister_extension(Ref<SourceEditorPluginExtension> p_extension);

	virtual void edited_scene_changed() override;

	virtual void shortcut_input(const Ref<InputEvent> &p_event) override;

	void _window_visibility_changed(bool p_visible);

	void _focus_another_editor();

	SourceEditorPlugin();
	~SourceEditorPlugin();
};

#endif // SOURCE_EDITOR_PLUGIN_H
