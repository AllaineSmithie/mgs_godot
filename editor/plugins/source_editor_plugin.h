#pragma once


#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "editor/editor_settings.h"
#include "editor/editor_translation_parser.h"
#include <scene/gui/panel_container.h>
#include <scene/gui/box_container.h>
#include <scene/gui/check_button.h>
#include <scene/gui/button.h>
#include <scene/gui/option_button.h>
#include <scene/gui/spin_box.h>
#include <scene/gui/line_edit.h>
#include <scene/gui/split_container.h>
#include <scene/gui/texture_rect.h>
#include <scene/main/timer.h>

/*
class SlotItemDescription : public PanelContainer
{
	GDCLASS(SlotItemDescription, PanelContainer);
	TextureRect* m_icon = nullptr;
	OptionButton* m_input_menu = nullptr;
	OptionButton* m_input_channel = nullptr;
	OptionButton* m_input_value_1 = nullptr;
	SpinBox* m_input_value_2 = nullptr;
	Button* m_input_apply = nullptr;
	OptionButton* m_output_menu = nullptr;
	OptionButton* m_output_channel = nullptr;
	OptionButton* m_output_value_1 = nullptr;
	SpinBox* m_output_value_2 = nullptr;
	Button* m_output_apply = nullptr;
	VBoxContainer* m_logic_vbox = nullptr;
	LineEdit* m_name_edit = nullptr;
public:
	SlotItemDescription();
	void configure(Dictionary& slotInfo);
protected:
};

class SourceEditor;

class SlotItem : public PanelContainer
{
	GDCLASS(SlotItem, PanelContainer);

	friend class ShowEditor;
protected:

	Button* m_selected = nullptr;
	CheckButton* m_enabled = nullptr;
	TextureRect* m_input_icon = nullptr;
	TextureRect* m_output_icon = nullptr;
	Label* m_slot_title = nullptr;
	Label* m_input_title = nullptr;
	Label* m_output_title = nullptr;
	Label* m_script_title = nullptr;
	Label* m_animation_title = nullptr;
	LinkButton* author = nullptr;
	Label* price = nullptr;

	RID m_slot_rid;

	void _onClicked();
	void _onEnabled();

protected:
	//void _notification(int p_what);
	static void _bind_methods();

public:
	void configure(const String& p_name,
		RID p_rid,
		const String& p_input_interface_type_name,
		const String& p_output_interface_type_name,
		const String& p_input_interface_name,
		const String& p_output_interface_name,
		const String& p_script_title,
		const String& p_animation_title,
		const bool active
	);
	SlotItem();
};

*/


// =========================================================================

class SourceEditorPluginExtension : public RefCounted
{
	GDCLASS(SourceEditorPluginExtension, RefCounted);
	friend class SourceEditor;
protected:
	Node* editor_page = nullptr;
public:
	virtual void reset_editor() = 0;
	virtual bool handles(Object* p_object) const = 0;
	virtual void edit(Object* p_object) = 0;
	virtual void edited_scene_changed() = 0;
	SourceEditorPluginExtension(){}
	~SourceEditorPluginExtension();
};

// =========================================================================

class SourceEditor: public PanelContainer
{
	GDCLASS(SourceEditor, PanelContainer);
private:
	static SourceEditor* singleton;
	bool initial_loading = true;

	TabContainer* m_header_bar = nullptr;
protected:

	static void _bind_methods();
	void _notification(int p_what);

public:
	static SourceEditor* get_singleton();
	SourceEditor();
	~SourceEditor();
	void register_extension(SourceEditorPluginExtension* p_extension);
	void unregister_extension(SourceEditorPluginExtension* p_extension);
};

// =========================================================================

class SourceEditorPlugin : public EditorPlugin {
	GDCLASS(SourceEditorPlugin, EditorPlugin);

	SourceEditor* _source_editor = nullptr;
	static SourceEditorPlugin* singleton;
	static Vector<Ref<SourceEditorPluginExtension>> extensions;

protected:
	void _notification(int p_what);
public:
	_FORCE_INLINE_ static SourceEditorPlugin* get_singleton() { return singleton; }

	SourceEditor* get_spatial_editor() { return _source_editor; }
	virtual String get_name() const override { return "Edit"; }
	bool has_main_screen() const override { return true; }
	virtual void make_visible(bool p_visible) override;
	virtual void edit(Object* p_object) override;
	virtual bool handles(Object* p_object) const override;

	virtual Dictionary get_state() const override;
	virtual const Ref<Texture2D> get_icon() const override;
	virtual void set_state(const Dictionary& p_state) override;
	virtual void clear() override;

	static void register_extension(Ref<SourceEditorPluginExtension> p_extension, bool p_at_front = false);
	static void unregister_extension(Ref<SourceEditorPluginExtension> p_extension);

	virtual void edited_scene_changed() override;

	virtual void shortcut_input(const Ref<InputEvent>& p_event) override;

	SourceEditorPlugin();
	~SourceEditorPlugin();
};


