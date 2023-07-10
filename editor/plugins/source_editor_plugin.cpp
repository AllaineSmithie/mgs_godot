
#include "source_editor_plugin.h"
#include "editor/editor_command_palette.h"
#include "editor/editor_scale.h"
#include "editor/gui/editor_file_dialog.h"
#include <assert.h>
#include <core/os/time.h>
#include <editor/plugins/animation_player_editor_plugin.h>
#include <scene/gui/menu_button.h>
#include <scene/gui/separator.h>
#include <scene/gui/tab_container.h>
#include <scene/scene_string_names.h>

SourceEditor *SourceEditor::source_editor = nullptr;

void SourceEditor::_menu_option(int p_option) {
	switch (p_option) {
		case SOURCE_FILE_OPEN: {
			file_dialog->set_file_mode(EditorFileDialog::FILE_MODE_OPEN_FILE);
			file_dialog->set_access(EditorFileDialog::ACCESS_FILESYSTEM);
			file_dialog_option = SOURCE_FILE_OPEN;

			List<String> extensions;
			ResourceLoader::get_recognized_extensions_for_type("Resource", &extensions);
			file_dialog->clear_filters();
			for (int i = 0; i < extensions.size(); i++) {
				file_dialog->add_filter("*." + extensions[i], extensions[i].to_upper());
			}

			file_dialog->popup_file_dialog();
			file_dialog->set_title(TTR("Open File"));
			return;
		} break;
	}
}

void SourceEditor::_window_changed(bool p_visible) {
	make_floating->set_visible(!p_visible);
}

void SourceEditor::_tab_changed(int p_which) {
	if (tab_container->get_tab_count() && tab_container->get_current_tab() >= 0) {
		/*ScriptEditorBase* se = _get_current_editor();
		if (se) {
			se->enable_editor(this);

			if (!grab_focus_block && is_visible_in_tree()) {
				se->ensure_focus();
			}
		}*/
	}
}

void SourceEditor::_file_dialog_action(String p_file) {
}

void SourceEditor::_autosave() {
}

void SourceEditor::set_window_layout(Ref<ConfigFile> p_layout) {
}

void SourceEditor::get_window_layout(Ref<ConfigFile> p_layout) {
}

void SourceEditor::_update_autosave_timer() {
	if (!autosave_timer->is_inside_tree()) {
		return;
	}

	float autosave_time = EDITOR_GET("text_editor/behavior/files/autosave_interval_secs");
	if (autosave_time > 0) {
		autosave_timer->set_wait_time(autosave_time);
		autosave_timer->start();
	} else {
		autosave_timer->stop();
	}
}

void SourceEditor::_close_current_tab(bool p_save) {
	//_close_tab(tab_container->get_current_tab(), p_save);
}

void SourceEditor::_close_discard_current_tab(const String &p_str) {
	//_close_tab(tab_container->get_current_tab(), false);
	erase_tab_confirm->hide();
}

SourceEditor::SourceEditor(WindowWrapper *p_wrapper) {
	source_window_wrapper = p_wrapper;
	ERR_FAIL_COND(source_editor != nullptr);

	// BG Stylebox
	StyleBoxFlat *panelstylebox = memnew(StyleBoxFlat);
	panelstylebox->set_bg_color(Color(0.10, 0.10, 0.10));
	panelstylebox->set_border_width(Side::SIDE_LEFT, 0);
	panelstylebox->set_border_width(Side::SIDE_RIGHT, 0);
	panelstylebox->set_border_width(Side::SIDE_TOP, 0);
	Color hot_color = get_theme_color(SNAME("accent_color"), SNAME("Editor"));
	panelstylebox->set_border_color(hot_color);
	add_theme_style_override("panel", panelstylebox);

	set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);

	VBoxContainer *library_main = memnew(VBoxContainer);
	add_child(library_main);

	menu_hb = memnew(HBoxContainer);
	library_main->add_child(menu_hb);

	tab_container = memnew(TabContainer);
	library_main->add_child(tab_container);
	tab_container->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	tab_container->set_h_size_flags(SIZE_EXPAND_FILL);
	tab_container->set_v_size_flags(SIZE_EXPAND_FILL);
	tab_container->add_theme_style_override("panel", panelstylebox);
	tab_container->set_custom_minimum_size(Size2(200, 0) * EDSCALE);
	tab_container->set_tabs_visible(false);

	set_process_input(true);
	set_process_shortcut_input(true);

	file_menu = memnew(MenuButton);
	file_menu->set_text(TTR("File"));
	file_menu->set_switch_on_hover(true);
	file_menu->set_shortcut_context(this);
	menu_hb->add_child(file_menu);

	file_menu->get_popup()->add_shortcut(ED_SHORTCUT("source_editor/open", TTR("Open...")), SOURCE_FILE_OPEN);

	batch_tools_menu = memnew(MenuButton);
	batch_tools_menu->set_text(TTR("Batch Processing"));
	batch_tools_menu->set_switch_on_hover(true);
	batch_tools_menu->set_shortcut_context(this);
	batch_tools_menu->get_popup()->connect("id_pressed", callable_mp(this, &SourceEditor::_menu_option));
	menu_hb->add_child(batch_tools_menu);

	menu_hb->add_spacer();

	if (p_wrapper->is_window_available()) {
		make_floating = memnew(ScreenSelect);
		make_floating->set_flat(true);
		make_floating->set_tooltip_text(TTR("Make the script editor floating."));
		make_floating->connect("request_open_in_screen", callable_mp(source_window_wrapper, &WindowWrapper::enable_window_on_screen).bind(true));

		menu_hb->add_child(make_floating);
		p_wrapper->connect("window_visibility_changed", callable_mp(this, &SourceEditor::_window_changed));
	}

	menu_hb->add_child(memnew(VSeparator));

	tab_container->connect("tab_changed", callable_mp(this, &SourceEditor::_tab_changed));

	erase_tab_confirm = memnew(ConfirmationDialog);
	erase_tab_confirm->set_ok_button_text(TTR("Save"));
	erase_tab_confirm->add_button(TTR("Discard"), DisplayServer::get_singleton()->get_swap_cancel_ok(), "discard");
	erase_tab_confirm->connect("confirmed", callable_mp(this, &SourceEditor::_close_current_tab).bind(true));
	erase_tab_confirm->connect("custom_action", callable_mp(this, &SourceEditor::_close_discard_current_tab));
	add_child(erase_tab_confirm);

	file_dialog_option = -1;
	file_dialog = memnew(EditorFileDialog);
	add_child(file_dialog);
	file_dialog->connect("file_selected", callable_mp(this, &SourceEditor::_file_dialog_action));

	error_dialog = memnew(AcceptDialog);
	add_child(error_dialog);

	source_editor = this;

	autosave_timer = memnew(Timer);
	autosave_timer->set_one_shot(false);
	autosave_timer->connect(SceneStringNames::get_singleton()->tree_entered, callable_mp(this, &SourceEditor::_update_autosave_timer));
	autosave_timer->connect("timeout", callable_mp(this, &SourceEditor::_autosave));
	add_child(autosave_timer);

	grab_focus_block = false;

	add_theme_style_override("panel", EditorNode::get_singleton()->get_gui_base()->get_theme_stylebox(SNAME("ScriptEditorPanel"), SNAME("EditorStyles")));

	// TO DO -> Add Detailed Connection Description
	//m_description = nullptr;

	set_process(true);
	set_process_shortcut_input(true); // Global shortcuts since there is no main element to be focused.

	// Signals
	//ShowEngine::get_singleton()->connect("slot_list_changed", callable_mp(this, &SourceEditor::_onSlotListChanged));
}
SourceEditor::~SourceEditor() {
	ERR_FAIL_COND(source_editor != this);
	source_editor = nullptr;
}

void SourceEditor::_bind_methods() {
}

void SourceEditor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			add_theme_style_override("panel", get_theme_stylebox(SNAME("bg"), SNAME("AssetLib")));
		} break;
		case NOTIFICATION_ENTER_TREE:
		case NOTIFICATION_THEME_CHANGED: {
		} break;

		case NOTIFICATION_VISIBILITY_CHANGED: {
			if (is_visible()) {
				/*EditorNode::get_singleton()->remove_bottom_panel_item(AnimationPlayerEditor::get_singleton());
				register_extension(AnimationPlayerEditor::get_singleton());*/
			} else {
				/*
				unregister_extension(AnimationPlayerEditor::get_singleton());
				EditorNode::get_singleton()->add_bottom_panel_item(TTR("Animation"), AnimationPlayerEditor::get_singleton());*/
			}
		} break;

		case NOTIFICATION_PROCESS: {
			// To Do
			// Process Input/Output Signal visualization

		} break;

		case NOTIFICATION_RESIZED: {
			//_update_asset_items_columns();
		} break;

		case EditorSettings::NOTIFICATION_EDITOR_SETTINGS_CHANGED: {
			//_update_repository_options();
		} break;
	}
}

void SourceEditor::register_extension(Control *p_extension) {
	ERR_FAIL_COND(p_extension == nullptr);
	// ERR_FAIL_COND_MSG(p_extension->editor_page == nullptr, "Editor extension has no editor page");

	for (auto i = 0; i < tab_container->get_tab_count(); ++i) {
		Control *child = tab_container->get_tab_control(i);
		if (child == p_extension) {
			// do nothing on double assignment
			return;
		}
	}
	tab_container->add_child(p_extension);
	tab_container->set_current_tab(tab_container->get_tab_count() - 1);
}

void SourceEditor::unregister_extension(Control *p_extension) {
	for (auto i = 0; i < tab_container->get_tab_count(); ++i) {
		Control *child = tab_container->get_tab_control(i);
		if (child == p_extension) {
			tab_container->remove_child(child);
		}
	}
	const int current_tab = tab_container->get_tab_count() > 0 ? tab_container->get_tab_count() - 1 : 0;
	tab_container->set_current_tab(current_tab);
}

//VBoxContainer* get_main_screen_control();

// ==================================================================================================================

void SourceEditorPlugin::_window_visibility_changed(bool p_visible) {
	_focus_another_editor();
	if (p_visible) {
		source_editor->add_theme_style_override("panel", source_editor->get_theme_stylebox("SourceEditorPanelFloating", "EditorStyles"));
	} else {
		source_editor->add_theme_style_override("panel", source_editor->get_theme_stylebox("SourceEditorPanel", "EditorStyles"));
	}
}

void SourceEditorPlugin::_focus_another_editor() {
	if (source_window_wrapper->get_window_enabled()) {
		ERR_FAIL_COND(last_editor.is_empty());
		EditorInterface::get_singleton()->set_main_screen_editor(last_editor);
	}
}
Vector<Ref<SourceEditorPluginExtension>> SourceEditorPlugin::extensions;

SourceEditorPlugin *SourceEditorPlugin::singleton = nullptr;
SourceEditorPlugin::SourceEditorPlugin() {
	singleton = this;

	source_window_wrapper = memnew(WindowWrapper);
	source_window_wrapper->set_window_title(TTR("Source Editor - Metro Gaya System"));
	source_window_wrapper->set_margins_enabled(true);

	source_editor = memnew(SourceEditor(source_window_wrapper));
	source_editor->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	Ref<Shortcut> make_floating_shortcut = ED_SHORTCUT_AND_COMMAND("source_editor/make_floating", TTR("Make Floating"));
	source_window_wrapper->set_wrapped_control(source_editor, make_floating_shortcut);

	EditorNode::get_singleton()->get_main_screen_control()->add_child(source_window_wrapper);
	source_window_wrapper->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	source_window_wrapper->hide();
	source_window_wrapper->connect("window_visibility_changed", callable_mp(this, &SourceEditorPlugin::_window_visibility_changed));

	// Panel Setup
	//const auto mainscreencontrols = EditorNode::get_singleton()->get_main_screen_control();
	//mainscreencontrols->add_child(source_editor);

	source_editor->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	source_editor->hide();

	// =========================================
	// Editor Settings
	// Interfaces Maneger

	// EditorSettings::get_singleton()->set_setting(Variant::INT, PROPERTY_HINT_ENUM,, 0, display_scale_hint_string, PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_RESTART_IF_CHANGED)
}

SourceEditorPlugin::~SourceEditorPlugin() {
	// Panel Setup
	const auto mainscreencontrols = EditorNode::get_singleton()->get_main_screen_control();
	mainscreencontrols->remove_child(source_editor);
	memdelete(source_editor);
}

void SourceEditorPlugin::shortcut_input(const Ref<InputEvent> &p_event) {
	ERR_FAIL_COND(p_event.is_null());
	Ref<InputEventKey> k = p_event;
	if ((k.is_valid() && k->is_pressed() && !k->is_echo()) || Object::cast_to<InputEventShortcut>(*p_event)) {
		if (ED_IS_SHORTCUT("editor/source_editor", p_event))
			EditorNode::get_singleton()->editor_select(get_index());
	}
}

void SourceEditorPlugin::_notification(int p_what) {
	/*
	switch (p_what) {
	case NOTIFICATION_INTERNAL_PROCESS: {
	} break;
	}*/
}

void SourceEditorPlugin::make_visible(bool p_visible) {
	if (p_visible && source_editor) {
		source_window_wrapper->show();
		source_editor->show();
	} else {
		source_window_wrapper->hide();
		source_editor->hide();
	}
}

void SourceEditorPlugin::set_window_layout(Ref<ConfigFile> p_layout) {
	source_editor->set_window_layout(p_layout);

	if (EDITOR_GET("interface/multi_window/restore_windows_on_load") && source_window_wrapper->is_window_available() && p_layout->has_section_key("SourceEditor", "window_rect")) {
		source_window_wrapper->restore_window_from_saved_position(
				p_layout->get_value("SourceEditor", "window_rect", Rect2i()),
				p_layout->get_value("SourceEditor", "window_screen", -1),
				p_layout->get_value("SourceEditor", "window_screen_rect", Rect2i()));
	} else {
		source_window_wrapper->set_window_enabled(false);
	}
}

void SourceEditorPlugin::get_window_layout(Ref<ConfigFile> p_layout) {
	source_editor->get_window_layout(p_layout);

	if (source_window_wrapper->get_window_enabled()) {
		p_layout->set_value("SourceEditor", "window_rect", source_window_wrapper->get_window_rect());
		int screen = source_window_wrapper->get_window_screen();
		p_layout->set_value("SourceEditor", "window_screen", screen);
		p_layout->set_value("SourceEditor", "window_screen_rect", DisplayServer::get_singleton()->screen_get_usable_rect(screen));

	} else {
		if (p_layout->has_section_key("SourceEditor", "window_rect")) {
			p_layout->erase_section_key("SourceEditor", "window_rect");
		}
		if (p_layout->has_section_key("SourceEditor", "window_screen")) {
			p_layout->erase_section_key("SourceEditor", "window_screen");
		}
		if (p_layout->has_section_key("ScriptEditor", "window_screen_rect")) {
			p_layout->erase_section_key("ScriptEditor", "window_screen_rect");
		}
	}
}

void SourceEditorPlugin::edit(Object *p_object) {
	for (auto ext : extensions) {
		if (ext->handles(p_object)) {
			ext->edit(p_object);
			return;
		}
	}
}

bool SourceEditorPlugin::handles(Object *p_object) const {
	for (auto ext : extensions) {
		if (ext->handles(p_object))
			return true;
	}
	return false;
}

inline Dictionary SourceEditorPlugin::get_state() const {
	return Dictionary();
}

const Ref<Texture2D> SourceEditorPlugin::get_icon() const {
	return Ref<Texture2D>();
}

inline void SourceEditorPlugin::set_state(const Dictionary &p_state) {
}

void SourceEditorPlugin::clear() {
	extensions.clear();
}

void SourceEditorPlugin::register_extension(Ref<SourceEditorPluginExtension> p_extension, bool p_at_front) {
	/*ERR_FAIL_COND(p_extension.is_null());
	if (p_at_front) {
		extensions.insert(0, p_extension);
	}
	else {
		extensions.push_back(p_extension);
	}*/
}

void SourceEditorPlugin::unregister_extension(Ref<SourceEditorPluginExtension> p_extension) {
	/*ERR_FAIL_COND(p_extension.is_null());
	extensions.erase(p_extension);
	*/
}

inline void SourceEditorPlugin::edited_scene_changed() {
	for (auto e : extensions)
		e->edited_scene_changed();
}

SourceEditorPluginExtension::~SourceEditorPluginExtension() {
	if (editor_page != nullptr)
		memdelete(editor_page);
}
