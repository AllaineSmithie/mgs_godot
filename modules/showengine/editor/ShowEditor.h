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

class ShowEditor;

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


class ShowEditor	: public PanelContainer
{
	GDCLASS(ShowEditor, PanelContainer);
	static ShowEditor* singleton;
	bool initial_loading = true;

public:
	static ShowEditor* get_singleton();
	ShowEditor();
	~ShowEditor();
	void clear();

	enum SearchCategory
	{
		ALL,
		FILTER_INPUTS,
		FILTER_OUTPUTS,
		FILTER_MAX
	};

	enum ShowItemsPerPage
	{
		ITEMS_PER_PAGE_ALL,
		ITEMS_PER_PAGE_25,
		ITEMS_PER_PAGE_50,
		ITEMS_PER_PAGE_100,
		ITEMS_PER_PAGE_MAX
	};

	enum SortOrder {
		SORT_UPDATED,
		SORT_UPDATED_REVERSE,
		SORT_NAME,
		SORT_NAME_REVERSE,
		SORT_COST,
		SORT_COST_REVERSE,
		SORT_MAX
	};


protected:

	static void _bind_methods();
	void _notification(int p_what);

	void _search_text_changed(const String& p_text);
	void _filter_debounce_timer_timeout();
	void _rerun_search(int p_ignore);
	void _search(int p_page = 0);
	void _filter_connection_type_changed(int p_repository_id);
	void _slots_changed();
	void _onAddSlotClicked();
	void _onRemoveSlotClicked();
	void _onInterfaceManagerClicked();
	void _onSlotListChanged();

private:


	HBoxContainer* _make_pages(int p_page, int p_page_count, int p_page_len, int p_total_items, int p_current_items);
	void _update_connection_types();
	//void _update_asset_items_columns();
	void _onSlotSelected(const RID id);
	void _onSlotEnabled(const RID p_rid, const bool enabled);
	bool _sort_updated(const Variant& first, const Variant& second);
	bool _sort_updated_reversed(const Variant& first, const Variant& second);
	bool _sort_names(const Variant& first, const Variant& second);
	bool _sort_names_reversed(const Variant& first, const Variant& second);

	String m_filter_sortorder;
	String m_filter_category;
	String m_filter_connection_type;

	Label* m_library_info = nullptr;
	Label* m_library_error_label = nullptr;
	Label* m_error_label = nullptr;

	TextureRect * m_error_tr = nullptr;

	HBoxContainer* m_asset_header_bar = nullptr;
	Button* m_add_connection = nullptr;
	Button* m_interface_manager = nullptr;
	Button* m_remove_connection = nullptr;

	RID m_selected_slot_id;
	unsigned m_page_index_to_show = 0;
	ShowItemsPerPage m_items_per_page = ITEMS_PER_PAGE_25;

	VBoxContainer* m_items = nullptr;
	//SlotItemDescription* m_items_description = nullptr;
	HBoxContainer* m_connection_bottom_page = nullptr;
	HBoxContainer* m_connection_top_page = nullptr;
	HBoxContainer* m_error_hb = nullptr;
	VBoxContainer* m_slots = nullptr;
	VBoxContainer* m_slotManager = nullptr;
	VBoxContainer* m_library_vb = nullptr;
	VBoxContainer* m_library_error = nullptr;
	PanelContainer* m_library_scroll_bg = nullptr;
	ScrollContainer* m_library_scroll = nullptr;
	OptionButton* m_sort = nullptr;
	OptionButton* m_categories = nullptr;
	OptionButton * m_type = nullptr;

	LineEdit* m_filter = nullptr;
	Timer* m_filter_debounce_timer = nullptr;

	EditorSelection* editor_selection = nullptr;

	static const char* sort_key[SORT_MAX];
	static const char* sort_text[SORT_MAX];
	static const char* filter_key[FILTER_MAX];
	static const char* filter_text[FILTER_MAX];
};



class ShowEditorPlugin : public EditorPlugin {
	GDCLASS(ShowEditorPlugin, EditorPlugin);

	ShowEditor* _show_editor = nullptr;

public:
	ShowEditor* get_spatial_editor() { return _show_editor; }
	virtual String get_name() const override { return "Show Editor"; }
	bool has_main_screen() const override { return true; }
	virtual void make_visible(bool p_visible) override;
	virtual void edit(Object* p_object) override;
	virtual bool handles(Object* p_object) const override;

	virtual Dictionary get_state() const override;
	virtual const Ref<Texture2D> get_icon() const override;
	virtual void set_state(const Dictionary& p_state) override;
	virtual void clear() override { _show_editor->clear(); }

	virtual void edited_scene_changed() override;

	virtual void shortcut_input(const Ref<InputEvent>& p_event) override;

	ShowEditorPlugin();
	~ShowEditorPlugin();
};


