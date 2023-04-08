
#include "ShowEditor.h"
#include "../ShowEngine.h"
#include <scene/gui/menu_button.h>
#include <core/os/time.h>
#include <scene/gui/separator.h>
#include <assert.h>
#include "editor/editor_scale.h"

#include "../digishow/DigiShow/digishow_common.h"
#include "../digishow/DigiShow/digishow_slot.h"

ShowEditor* ShowEditor::singleton = nullptr;

const PackedStringArray m_interfaceTypes =
{
	 "Midi",
	 "Rioc",
	 "Modbus",
	 "Smartlight",
	 "DMX",
	 "OSC",
	 "Artnet/sACN",
	 "Remote",
	 "Launch",
	 "InputKey",
	 "Metronome",
	 "Audio",
	 "Screen"
};
const Vector<int> m_interfaceTypesSortIdx =
{
	 0,
	 4,
	 9,
	 10,
	 11,
	 12,
	 5,
	 6,
	 7,
	 3,
	 1,
	 2,
	 8
};


const char* ShowEditor::sort_key[SORT_MAX] = {
	"updated",
	"updated",
	"name",
	"name",
	"cost",
	"cost",
};

const char* ShowEditor::sort_text[SORT_MAX] = {
	TTRC("Newest Updated"),
	TTRC("Oldest Updated"),
	TTRC("Name (A-Z)"),
	TTRC("Name (Z-A)"),
	TTRC("ID (0-X)"), // "cost" stores the SPDX license name in the Godot Asset Library.
	TTRC("ID (X-0)"), // "cost" stores the SPDX license name in the Godot Asset Library.
};

const char* ShowEditor::filter_key[FILTER_MAX] = {
	"all",
	"inputs",
	"outputs",
};

const char* ShowEditor::filter_text[FILTER_MAX] = {
	TTRC("All"),
	TTRC("Inputs Only"),
	TTRC("Outputs Only")
};


// ============================================================================================================================
SlotItemDescription::SlotItemDescription()
{
	auto stylebox = get_theme_stylebox(SNAME("panel"), SNAME("Tree"));
	if (stylebox->has_method("set_bg_color"))
		stylebox->call("set_bg_color", Color(0.24, 0.24, 0.24, 1.0));

	add_theme_style_override("panel", stylebox);


	// Input

	VBoxContainer* vbox = memnew(VBoxContainer);
	add_child(vbox);

	PanelContainer* name_pc = memnew(PanelContainer);
	name_pc->add_child(memnew(Label("Slot Name:")));
	name_pc->add_child(memnew(VSeparator));

	m_name_edit = memnew(LineEdit);
	m_name_edit->set_text("Enter Slot Name");
	name_pc->add_child(m_name_edit);

	vbox->add_child(name_pc);

	MarginContainer* input_mc = memnew(MarginContainer);
	vbox->add_child(input_mc);
	PanelContainer* input_pc = memnew(PanelContainer);
	input_mc->add_child(input_pc);
	MarginContainer* input_mc2 = memnew(MarginContainer);
	input_pc->add_child(input_mc2);
	HBoxContainer* hbox = memnew(HBoxContainer);
	input_mc2->add_child(hbox);

	m_icon = memnew(TextureRect);
	m_icon->set_custom_minimum_size(Size2(80, 60));

	hbox->add_child(m_icon);
	hbox->add_theme_constant_override("separation", 15 * EDSCALE);

	VBoxContainer* input_vbox = memnew(VBoxContainer);
	hbox->add_child(input_vbox);
	HBoxContainer* input_hbox = memnew(HBoxContainer);
	input_vbox->add_child(input_hbox);

	input_hbox->add_child(memnew(Label("Input:")));
	m_input_menu = memnew(OptionButton);
	input_hbox->add_child(m_input_menu);

	for (auto i = 0; i < m_interfaceTypes.size(); ++i)
	{
		m_input_menu->add_item(m_interfaceTypes[m_interfaceTypesSortIdx[i]]);
	}
	m_input_menu->set_h_size_flags(SIZE_EXPAND);

	const auto input_hbox2 = memnew(HBoxContainer);
	input_vbox->add_child(input_hbox2);

	input_hbox2->add_child(memnew(Label("Channel:")));
	m_input_channel = memnew(OptionButton);
	m_input_channel->add_item("All");
	for (auto i = 0; i < 16; ++i)
	{
		m_input_channel->add_item(String::num(i, 0));
	}
	input_hbox2->add_child(m_input_channel);

	input_hbox2->add_child(memnew(Label("Value 1:")));
	m_input_value_1 = memnew(OptionButton);
	m_input_value_1->add_item("All");
	for (auto i = 0; i < 128; ++i)
	{
		m_input_value_1->add_item(String::num(i, 0));
	}
	input_hbox2->add_child(m_input_value_1);

	input_hbox2->add_child(memnew(Label("Value 2:")));
	m_input_value_2 = memnew(SpinBox);
	m_input_value_2->set_min(0);
	m_input_value_2->set_max(128);
	input_hbox2->add_child(m_input_value_2);


	VBoxContainer* input_apply_vbox = memnew(VBoxContainer);
	input_apply_vbox->set_alignment(BoxContainer::ALIGNMENT_END);
	hbox->add_child(input_apply_vbox);
	m_input_apply = memnew(Button("Apply"));


	// Logic

	MarginContainer* logic_mc = memnew(MarginContainer);
	logic_mc->set_v_size_flags(SIZE_EXPAND_FILL);
	vbox->add_child(logic_mc);
	PanelContainer* logic_pc = memnew(PanelContainer);
	logic_mc->add_child(logic_pc);
	MarginContainer* logic_mc2 = memnew(MarginContainer);
	logic_pc->add_child(logic_mc2);


	m_logic_vbox = memnew(VBoxContainer);
	m_logic_vbox->set_alignment(BoxContainer::ALIGNMENT_CENTER);
	logic_mc2->add_child(m_logic_vbox);

	m_logic_vbox->add_child(memnew(Label("Signal mapping transformation options")));
	const auto sublabel = memnew(Label("Please select signal endpoints of both source and destination"));
	sublabel->add_theme_color_override("font_color", Color(0.47, 0.47, 0.47, 1.0));
	m_logic_vbox->add_child(sublabel);


	// Output

	MarginContainer* output_mc = memnew(MarginContainer);
	vbox->add_child(output_mc);
	PanelContainer* output_pc = memnew(PanelContainer);
	output_mc->add_child(output_pc);
	MarginContainer* output_mc2 = memnew(MarginContainer);
	output_pc->add_child(output_mc2);
	hbox = memnew(HBoxContainer);
	output_mc2->add_child(hbox);

	m_icon = memnew(TextureRect);
	m_icon->set_custom_minimum_size(Size2(80, 60));

	hbox->add_child(m_icon);
	hbox->add_theme_constant_override("separation", 15 * EDSCALE);

	VBoxContainer* output_vbox = memnew(VBoxContainer);
	hbox->add_child(output_vbox);
	HBoxContainer* output_hbox = memnew(HBoxContainer);
	output_vbox->add_child(output_hbox);

	output_hbox->add_child(memnew(Label("Output:")));
	m_output_menu = memnew(OptionButton);
	output_hbox->add_child(m_output_menu);

	for (auto i = 0; i < m_interfaceTypes.size(); ++i)
	{
		m_output_menu->add_item(m_interfaceTypes[m_interfaceTypesSortIdx[i]]);
	}
	m_output_menu->set_h_size_flags(SIZE_EXPAND);

	const auto output_hbox2 = memnew(HBoxContainer);
	output_vbox->add_child(output_hbox2);

	output_hbox2->add_child(memnew(Label("Channel:")));
	m_output_channel = memnew(OptionButton);
	m_output_channel->add_item("All");
	for (auto i = 0; i < 16; ++i)
	{
		m_output_channel->add_item(String::num(i, 0));
	}
	output_hbox2->add_child(m_output_channel);

	output_hbox2->add_child(memnew(Label("Value 1:")));
	m_output_value_1 = memnew(OptionButton);
	m_output_value_1->add_item("All");
	for (auto i = 0; i < 128; ++i)
	{
		m_output_value_1->add_item(String::num(i, 0));
	}
	output_hbox2->add_child(m_output_value_1);

	output_hbox2->add_child(memnew(Label("Value 2:")));
	m_output_value_2 = memnew(SpinBox);
	m_output_value_2->set_min(0);
	m_output_value_2->set_max(128);
	output_hbox2->add_child(m_output_value_2);


}

void SlotItemDescription::configure(Dictionary& info)
{
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
	
	const Dictionary& source		= info["sourceInterface"];
	const Dictionary& destination	= info["destinationInterface"];


	const String name				= info["name"];

	const int sourcetype			= source["type"];
	const int sourcemode			= source["mode"];           // interface mode id
	const bool sourceoutput			= source["output"];			// can output signal
	const bool sourceinput			= source["input"];			// can input signal
	const String sourcelabel		= source["label"];			// a label describes the interface

	const int destinationtype		= destination["type"];
	const int destinationmode		= destination["mode"];       // interface mode id
	const bool destinationoutput	= destination["output"];     // can output signal
	const bool destinationinput		= destination["input"];      // can input signal
	const String destinationlabel	= destination["label"];      // a label describes the interface

	m_icon->set_texture(get_theme_icon(StringName(name), SNAME("EditorIcons")));

	//info["inputSignal"     ] = m_slotInfo.inputSignal;
	//info["outputSignal"    ] = m_slotInfo.outputSignal;
	info["inputRange"      ];
	info["outputRange"     ];

	info["inputLow"        ];
	info["inputHigh"       ];
	info["outputLow"       ];
	info["outputHigh"      ];
	info["inputInverted"   ];
	info["outputInverted"  ];
	info["outputLowAsZero" ];

	info["envelopeAttack"  ];
	info["envelopeHold"    ];
	info["envelopeDecay"   ];
	info["envelopeSustain" ];
	info["envelopeRelease" ];
	info["envelopeInDelay" ];
	info["envelopeOutDelay"];

	info["outputSmoothing" ];
	info["outputInterval"  ];

}

// ============================================================================================================================

void SlotItem::configure(const String& p_name,
	RID p_rid,
	const String& p_input_interface_type_name,
	const String& p_output_interface_type_name,
	const String& p_input_interface_name,
	const String& p_output_interface_name,
	const String& p_script_title,
	const String& p_animation_title,
	const bool active)
{
	m_slot_title->set_text(p_name);
	m_slot_rid = p_rid;

	m_input_title->set_text(p_input_interface_name);
	m_output_title->set_text(p_output_interface_name);
	m_script_title->set_text(p_script_title);
	m_animation_title->set_text(p_animation_title);
	m_input_icon = memnew(TextureRect);
	m_output_icon = memnew(TextureRect);
	if (!(p_input_interface_type_name.is_empty() || p_input_interface_type_name == "<null>") && get_theme_icon(StringName(p_input_interface_type_name), SNAME("EditorIcons")).is_valid())
		m_input_icon->set_texture(EditorNode::get_singleton()->get_class_icon(StringName(p_input_interface_type_name), SNAME("EditorIcons")));
	else
		m_input_icon->set_texture(EditorNode::get_singleton()->get_class_icon(SNAME("Node"), SNAME("EditorIcons")));

	if (!(p_output_interface_type_name.is_empty() || p_output_interface_type_name == "<null>") && get_theme_icon(StringName(p_output_interface_type_name), SNAME("EditorIcons")).is_valid())
		m_output_icon->set_texture(EditorNode::get_singleton()->get_class_icon(StringName(p_output_interface_type_name), SNAME("EditorIcons")));
	else
		m_output_icon->set_texture(EditorNode::get_singleton()->get_class_icon(SNAME("Node"), SNAME("EditorIcons")));

	m_enabled->set_pressed_no_signal(active);
}


void SlotItem::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("slot_selected", PropertyInfo(Variant::INT, "id")));
	ADD_SIGNAL(MethodInfo("slot_enabled", PropertyInfo(Variant::INT, "id"), PropertyInfo(Variant::BOOL, "enabled")));
}

void SlotItem::_onClicked()
{
	emit_signal("slot_selected", m_slot_rid);
}

void SlotItem::_onEnabled()
{

}

SlotItem::SlotItem()
{
	//set_mouse_filter(MOUSE_FILTER_IGNORE);


	Ref<StyleBoxEmpty> border;
	border.instantiate();
	border->set_content_margin_all(5 * EDSCALE);
	add_theme_style_override("panel", border);

	auto button_mc = memnew(MarginContainer);
	button_mc->add_theme_constant_override("margin_top", 22);
	button_mc->add_theme_constant_override("margin_right", 2);
	add_child(button_mc);

	m_selected = memnew(Button);
	button_mc->add_child(m_selected);
	m_selected->connect("pressed", callable_mp(this, &SlotItem::_onClicked));
	//m_enabled = memnew(CheckButton);
	//m_enabled->add_theme_constant_override("check_v_offset", 6);
	//m_enabled->set_icon(get_theme_icon("icon_default_project_icon"));
	//m_enabled->set_mouse_filter(MOUSE_FILTER_PASS);
	//m_enabled->set_focus_mode(FOCUS_NONE);
	//m_enabled->set_expand_icon(true);
	//button_mc->add_child(m_enabled);

	auto selectbutton_mc = memnew(HBoxContainer);
	selectbutton_mc->set_mouse_filter(MOUSE_FILTER_IGNORE);
	selectbutton_mc->set_alignment(HBoxContainer::ALIGNMENT_END);
	selectbutton_mc->add_theme_constant_override("margin_right", 15);
	//selectbutton_mc->add_theme_constant_override("margin_right", 55);
	selectbutton_mc->set("layout_mode", LAYOUT_MODE_ANCHORS);
	selectbutton_mc->set("anchors_preset", PRESET_CENTER_LEFT);
	selectbutton_mc->set_v_size_flags(SIZE_EXPAND_FILL);
	m_selected->add_child(selectbutton_mc);

	m_enabled = memnew(CheckButton);
	m_enabled->set_focus_mode(FOCUS_NONE);
	m_enabled->set_icon(get_theme_icon("MoveRight"));
	//m_enabled->add_theme_constant_override("check_v_offset", 6);
	m_enabled->connect("pressed", callable_mp(this, &SlotItem::_onEnabled));
	selectbutton_mc->add_child(m_enabled);

	VBoxContainer* vb = memnew(VBoxContainer);
	vb->set_mouse_filter(MOUSE_FILTER_IGNORE);
	// avoid hidden possibility to click enabled/disabled
	vb->add_theme_constant_override("separation", 0);
	add_child(vb);

	m_slot_title = memnew(Label("Connection: Unnamed"));
	m_slot_title->set_mouse_filter(MOUSE_FILTER_STOP);
	vb->add_child(m_slot_title);

	auto main_mc = memnew(MarginContainer);
	main_mc->add_theme_constant_override("margin_left", 100);
	main_mc->add_theme_constant_override("margin_right", 80);
	main_mc->set_mouse_filter(MOUSE_FILTER_IGNORE);
	vb->add_child(main_mc);

	vb->add_child(memnew(HSeparator));

	HBoxContainer* hb = memnew(HBoxContainer);
	// Add some spacing to visually separate the icon from the asset details.
	hb->add_theme_constant_override("separation", 15 * EDSCALE);
	hb->set_mouse_filter(MOUSE_FILTER_IGNORE);
	main_mc->add_child(hb);

	// Input
	MarginContainer* inputmargin = memnew(MarginContainer);
	inputmargin->add_theme_constant_override("margin_left", 8);
	inputmargin->add_theme_constant_override("margin_right", 8);
	inputmargin->add_theme_constant_override("margin_top", 8);
	inputmargin->add_theme_constant_override("margin_bottom", 8);
	inputmargin->set_mouse_filter(MOUSE_FILTER_IGNORE);
	hb->add_child(inputmargin);


	VBoxContainer* input_vb = memnew(VBoxContainer);
	inputmargin->add_child(input_vb);
	input_vb->set_mouse_filter(MOUSE_FILTER_IGNORE);
	input_vb->add_child(memnew(Label("Source")));

	m_input_title = memnew(Label("no input"));
	input_vb->add_child(m_input_title);

	m_input_icon = memnew(TextureRect);
	input_vb->add_child(m_input_icon);

	TextureRect* input_symbol = memnew(TextureRect);
	input_symbol->set_texture(EditorNode::get_singleton()->get_class_icon("Node"));
	input_symbol->set_expand_mode(TextureRect::EXPAND_FIT_WIDTH_PROPORTIONAL);
	input_symbol->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
	input_vb->add_child(input_symbol);

	// Spacer 1
	Control* spacer = memnew(Control);
	spacer->set_h_size_flags(SIZE_EXPAND_FILL);
	spacer->set_mouse_filter(MOUSE_FILTER_IGNORE);
	hb->add_child(spacer);

	// Script + Spacer 2 & 3
	VBoxContainer* script_vb = memnew(VBoxContainer);
	script_vb->set_mouse_filter(MOUSE_FILTER_IGNORE);
	hb->add_child(script_vb);
	spacer = memnew(Control);
	spacer->set_v_size_flags(SIZE_EXPAND_FILL);
	spacer->set_mouse_filter(MOUSE_FILTER_IGNORE);
	script_vb->add_child(spacer);
	
	script_vb->add_child(memnew(Label("Script:")));

	m_script_title = memnew(Label("empty"));
	script_vb->add_child(m_script_title);

	spacer = memnew(Control);
	spacer->set_v_size_flags(SIZE_EXPAND_FILL);
	spacer->set_mouse_filter(MOUSE_FILTER_IGNORE);
	script_vb->add_child(spacer);

	// Spacer 4
	spacer = memnew(Control);
	spacer->set_h_size_flags(SIZE_EXPAND_FILL);
	spacer->set_mouse_filter(MOUSE_FILTER_IGNORE);
	hb->add_child(spacer);

	// Animation + Spacer 5 & 6
	VBoxContainer* animation_vb = memnew(VBoxContainer);
	hb->add_child(animation_vb);
	spacer = memnew(Control);
	spacer->set_v_size_flags(SIZE_EXPAND_FILL);
	spacer->set_mouse_filter(MOUSE_FILTER_IGNORE);
	animation_vb->add_child(spacer);
	animation_vb->add_child(memnew(Label("Animation:")));

	m_animation_title = memnew(Label("empty"));
	animation_vb ->add_child(m_animation_title);

	spacer = memnew(Control);
	spacer->set_v_size_flags(SIZE_EXPAND_FILL);
	spacer->set_mouse_filter(MOUSE_FILTER_IGNORE);
	animation_vb->add_child(spacer);

	// Spacer 7
	spacer = memnew(Control);
	spacer->set_h_size_flags(SIZE_EXPAND_FILL);
	spacer->set_mouse_filter(MOUSE_FILTER_IGNORE);
	hb->add_child(spacer);

	// Output
	MarginContainer* outputmargin = memnew(MarginContainer);
	outputmargin->add_theme_constant_override("margin_left", 8);
	outputmargin->add_theme_constant_override("margin_right", 8);
	outputmargin->add_theme_constant_override("margin_top", 8);
	outputmargin->add_theme_constant_override("margin_bottom", 8);
	hb->add_child(outputmargin);


	VBoxContainer* output_vb = memnew(VBoxContainer);
	outputmargin->add_child(output_vb);
	outputmargin->set_mouse_filter(MOUSE_FILTER_IGNORE);
	output_vb->add_child(memnew(Label("Destination")));

	m_output_title = memnew(Label("no output"));
	output_vb->add_child(m_output_title);
	m_output_icon = memnew(TextureRect);
	output_vb->add_child(m_output_icon);


	//set_custom_minimum_size(Size2(250, 100) * EDSCALE);
	set_h_size_flags(Control::SIZE_EXPAND_FILL);
}


ShowEditor* ShowEditor::get_singleton()
{
	return singleton;
}


ShowEditor::ShowEditor()
{
	ERR_FAIL_COND(singleton != nullptr);
	singleton = this;

	editor_selection = EditorNode::get_singleton()->get_editor_selection();
	editor_selection->add_editor_plugin(this);

	// BG Stylebox
	StyleBoxFlat* panelstylebox = memnew(StyleBoxFlat);
	panelstylebox->set_bg_color(Color(0.14, 0.14, 0.14));
	add_theme_style_override("panel", panelstylebox);

	set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);

	HBoxContainer* main_flow = memnew(HBoxContainer);
	main_flow->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	add_child(main_flow);
	VBoxContainer* library_main = memnew(VBoxContainer);
	add_child(library_main);

	m_asset_header_bar = memnew(HBoxContainer);
	library_main->add_child(m_asset_header_bar);
	m_add_connection = memnew(Button("Add"));
	m_add_connection->connect("pressed", callable_mp(this, &ShowEditor::_onAddSlotClicked));
	m_asset_header_bar->add_child(m_add_connection);
	m_asset_header_bar->add_child(memnew(VSeparator));
	m_remove_connection = memnew(Button("Remove"));
	m_remove_connection->connect("pressed", callable_mp(this, &ShowEditor::_onRemoveSlotClicked));
	m_asset_header_bar->add_child(m_remove_connection);
	m_asset_header_bar->add_child(memnew(VSeparator));
	auto separator = memnew(Control);
	separator->set_h_size_flags(SIZE_EXPAND_FILL);
	m_asset_header_bar->add_child(separator);
	m_asset_header_bar->add_child(memnew(VSeparator));
	m_interface_manager = memnew(Button("Interface\nManager"));
	m_interface_manager->connect("pressed", callable_mp(this, &ShowEditor::_onInterfaceManagerClicked));
	m_asset_header_bar->add_child(m_interface_manager);

	HBoxContainer* search_hb = memnew(HBoxContainer);

	library_main->add_child(search_hb);
	library_main->add_theme_constant_override("separation", 10 * EDSCALE);

	
	m_filter = memnew(LineEdit);
	m_filter->set_placeholder(TTR("Search Connections"));
	m_filter->set_clear_button_enabled(true);
	search_hb->add_child(m_filter);
	m_filter->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	m_filter->connect("text_changed", callable_mp(this, &ShowEditor::_search_text_changed));

	// Perform a search automatically if the user hasn't entered any text for a certain duration.
	// This way, the user doesn't need to press Enter to initiate their search.
	m_filter_debounce_timer = memnew(Timer);
	m_filter_debounce_timer->set_one_shot(true);
	m_filter_debounce_timer->set_wait_time(0.25);
	m_filter_debounce_timer->connect("timeout", callable_mp(this, &ShowEditor::_filter_debounce_timer_timeout));
	search_hb->add_child(m_filter_debounce_timer);
	
	search_hb->add_child(memnew(VSeparator));


	HBoxContainer* search_hb2 = memnew(HBoxContainer);
	library_main->add_child(search_hb2);

	search_hb2->add_child(memnew(Label(TTR("Sort:") + " ")));
	m_sort = memnew(OptionButton);
	for (int i = 0; i < SORT_MAX; i++) {
		m_sort->add_item(TTRGET(sort_text[i]));
	}

	search_hb2->add_child(m_sort);

	m_sort->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	m_sort->set_clip_text(true);
	m_sort->connect("item_selected", callable_mp(this, &ShowEditor::_rerun_search));

	search_hb2->add_child(memnew(VSeparator));

	search_hb2->add_child(memnew(Label(TTR("Interface Type:") + " ")));
	m_categories = memnew(OptionButton);
	m_categories->add_item(TTR("All"));

	m_categories->set_item_metadata(0, 0);

	for (int i = 0; i < m_interfaceTypes.size(); i++)
	{
		auto interface_type_name = m_interfaceTypes[m_interfaceTypesSortIdx[i]];
		m_categories->add_item(interface_type_name);
		m_categories->set_item_metadata(-1, m_interfaceTypesSortIdx[i]);
	}


	search_hb2->add_child(m_categories);
	m_categories->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	m_categories->set_clip_text(true);
	m_categories->connect("item_selected", callable_mp(this, &ShowEditor::_rerun_search));

	search_hb2->add_child(memnew(VSeparator));

	search_hb2->add_child(memnew(Label(TTR("Type:") + " ")));
	m_type = memnew(OptionButton);
	m_type->add_item(TTRGET(filter_text[ALL]), ALL);
	m_type->add_item(TTRGET(filter_text[FILTER_INPUTS]), FILTER_INPUTS);
	m_type->add_item(TTRGET(filter_text[FILTER_OUTPUTS]), FILTER_OUTPUTS);
	m_type->select(1);
	_update_connection_types();

	m_type->connect("item_selected", callable_mp(this, &ShowEditor::_filter_connection_type_changed));

	search_hb2->add_child(m_type);
	m_type->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	m_type->set_clip_text(true);

	search_hb2->add_child(memnew(VSeparator));

	//HSplitContainer* m_items_hs = memnew(HSplitContainer);
	//m_items_hs->set_split_offset(get_size().width / 2);
	//m_items_hs->set_h_size_flags(SIZE_EXPAND_FILL);
	//m_items_hs->set_v_size_flags(SIZE_EXPAND_FILL);
	//library_main->add_child(m_items_hs);

	m_library_scroll_bg = memnew(PanelContainer);
	//m_items_hs->add_child(m_library_scroll_bg);
	library_main->add_child(m_library_scroll_bg);
	m_library_scroll_bg->set_v_size_flags(Control::SIZE_EXPAND_FILL);

	//m_items_description = memnew(SlotItemDescription);
	//m_items_description->set_h_size_flags(SIZE_EXPAND_FILL);
	//m_items_hs->add_child(m_items_description);

	m_library_scroll = memnew(ScrollContainer);
	m_library_scroll->set_horizontal_scroll_mode(ScrollContainer::SCROLL_MODE_DISABLED);

	m_library_scroll_bg->add_child(m_library_scroll);

	Ref<StyleBoxEmpty> border2;
	border2.instantiate();
	border2->set_content_margin_individual(15 * EDSCALE, 15 * EDSCALE, 35 * EDSCALE, 15 * EDSCALE);

	PanelContainer* library_vb_border = memnew(PanelContainer);
	m_library_scroll->add_child(library_vb_border);
	library_vb_border->add_theme_style_override("panel", border2);
	library_vb_border->set_h_size_flags(Control::SIZE_EXPAND_FILL);

	m_library_vb = memnew(VBoxContainer);
	m_library_vb->set_h_size_flags(Control::SIZE_EXPAND_FILL);

	library_vb_border->add_child(m_library_vb);

	m_library_info = memnew(Label);
	m_library_info->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	m_library_vb->add_child(m_library_info);

	m_library_error = memnew(VBoxContainer);
	m_library_error->hide();
	m_library_vb->add_child(m_library_error);

	m_library_error_label = memnew(Label(TTR("No entries found.")));
	m_library_error_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	m_library_error->add_child(m_library_error_label);


	m_connection_top_page = memnew(HBoxContainer);
	m_library_vb->add_child(m_connection_top_page);

	auto m_items_vb = memnew(HBoxContainer);
	m_library_vb->add_child(m_items_vb);

	m_items = memnew(VBoxContainer);
	//_update_asset_items_columns();
	m_items->add_theme_constant_override("separation", 10 * EDSCALE);
	m_items->set_h_size_flags(SIZE_EXPAND_FILL);


	m_library_vb->add_child(m_items);


	m_connection_bottom_page = memnew(HBoxContainer);
	m_library_vb->add_child(m_connection_bottom_page);

	m_library_vb->add_theme_constant_override("separation", 20 * EDSCALE);

	m_error_hb = memnew(HBoxContainer);
	library_main->add_child(m_error_hb);
	m_error_label = memnew(Label);
	m_error_hb->add_child(m_error_label);
	m_error_tr = memnew(TextureRect);
	m_error_tr->set_v_size_flags(Control::SIZE_SHRINK_CENTER);
	m_error_hb->add_child(m_error_tr);

	// TO DO -> Add Detailed Connection Description
	//m_description = nullptr;

	set_process(true);
	set_process_shortcut_input(true); // Global shortcuts since there is no main element to be focused.


	// Signals
	ShowEngine::get_singleton()->connect("slot_list_changed", callable_mp(this, &ShowEditor::_onSlotListChanged));

}
ShowEditor::~ShowEditor()
{
	ERR_FAIL_COND(singleton != this);
	singleton = nullptr;
}

void ShowEditor::_search_text_changed(const String& p_text)
{
	m_filter_debounce_timer->start();
}

void ShowEditor::_filter_debounce_timer_timeout()
{
	_search();
}

void ShowEditor::_rerun_search(int p_ignore)
{
	_search();
}

void ShowEditor::_search(int p_page)
{
	

	const String filter = m_filter->get_text();

	const int sort_key_idx = m_sort->get_selected();

	const int type_idx = ([this](){
							for (int i = 0; i < FILTER_MAX; i++)
								{
									if (m_type->get_popup()->is_item_checked(i))
									{
										return i;
									}
								}
								return 0;
							})();

	// sort-idx-translated
	const int interface_type_idx = m_interfaceTypesSortIdx.find(m_categories->get_selected());

	if (p_page > -1)
		m_page_index_to_show = p_page;


	Array result;

	const auto slotlistarray = ShowEngine::get_singleton()->getSlotList();
	for (auto i = 0; i < slotlistarray.size(); ++i)
	{
		const Dictionary& slot = slotlistarray[i];

		if (type_idx != 2)
		{
			Dictionary sourceinterface = slot["sourceInterface"];
			const String labelname = sourceinterface["label"];

			if (!filter.is_empty() && !labelname.contains(filter))
				continue;

			const int typeindex = sourceinterface["type"];
			if (typeindex != interface_type_idx)
				continue;

			result.append(slot);
			
		}
		if (type_idx != 1)
		{
			Dictionary destinationinterface = slot["destinationInterface"];
			const String& labelname = destinationinterface["label"];

			if (!labelname.contains(filter))
				continue;


			const int typeindex = destinationinterface["type"];
			if (typeindex != interface_type_idx)
				continue;

			result.append(slot);
			
		}
	}


	switch (SortOrder(sort_key_idx))
	{
		// Newest Updated
	case ShowEditor::SORT_UPDATED:
		result.sort_custom(callable_mp(this, &ShowEditor::_sort_updated));
		break;
		// Oldest Updated
	case ShowEditor::SORT_UPDATED_REVERSE:
		result.sort_custom(callable_mp(this, &ShowEditor::_sort_updated_reversed));
		break;
		// Name (A-Z)
	case ShowEditor::SORT_NAME:
		break;
		result.sort_custom(callable_mp(this, &ShowEditor::_sort_names));
		// Name (Z-A)
	case ShowEditor::SORT_NAME_REVERSE:
		result.sort_custom(callable_mp(this, &ShowEditor::_sort_names_reversed));
		break;
		// ID (0-X)
	case ShowEditor::SORT_COST:
		break;
		// ID (X-0) -> no need to sort
	case ShowEditor::SORT_COST_REVERSE:
		result.reverse();
		break;
	default:
		break;
	}


	if (m_items)
	{
		memdelete(m_items);
	}

	if (m_connection_top_page)
	{
		memdelete(m_connection_top_page);
	}

	if (m_connection_bottom_page)
	{
		memdelete(m_connection_bottom_page);
	}

	int page = 0;
	int pages = 1;
	int page_len = 10;
	int total_items = 1;

	int from = 0, to = 0;
	switch (m_items_per_page)
	{
	case ShowEditor::ITEMS_PER_PAGE_25:
		from = MIN<unsigned, unsigned>((m_page_index_to_show) * 25, result.size());
		to = MIN<unsigned, unsigned>((m_page_index_to_show + 1) * 25, result.size());
		pages = 1 + (result.size() / 25);
		break;
	case ShowEditor::ITEMS_PER_PAGE_50:
		from = MIN<unsigned, unsigned>((m_page_index_to_show) * 50, result.size());
		to = MIN<unsigned, unsigned>((m_page_index_to_show + 1) * 50, result.size());
		pages = 1 + (result.size() / 50);
		break;
	case ShowEditor::ITEMS_PER_PAGE_100:
		from = MIN<unsigned, unsigned>((m_page_index_to_show) * 100, result.size());
		to = MIN<unsigned, unsigned>((m_page_index_to_show + 1) * 100, result.size());
		pages = 1 + (result.size() / 100);
		break;
	default:
		from = 0;
		to = result.size();
		break;
	}


	m_connection_top_page = _make_pages(page, pages, page_len, total_items, result.size());
	m_library_vb->add_child(m_connection_top_page);

	m_items = memnew(VBoxContainer);
	//_update_asset_items_columns();
	m_items->add_theme_constant_override("separation", 10 * EDSCALE);
	m_items->set_h_size_flags(SIZE_EXPAND_FILL);

	m_library_vb->add_child(m_items);

	//m_items_description->set_visible(false);


	m_connection_bottom_page = _make_pages(page, pages, page_len, total_items, result.size());
	m_library_vb->add_child(m_connection_bottom_page);

	if (result.is_empty())
	{
		if (!m_filter->get_text().is_empty())
		{
			m_library_info->set_text(
				vformat(TTR("No results for \"%s\" for type: %s."), m_filter->get_text(), m_type->get_popup()->get_item_text(type_idx)));
		}
		else {
			// No results, even though the user didn't search for anything specific.
			// This is typically because the version number changed recently
			// and no assets compatible with the new version have been published yet.
			m_library_info->set_text(
				vformat(TTR("No slots specified for type: %s interface category: %s"), m_type->get_popup()->get_item_text(type_idx), m_categories->get_item_text(interface_type_idx)));
		}
		m_library_info->show();
		return;
	}
	else {
		m_library_info->hide();
	}

	Vector<SlotItem*> newitems;
	for (int i = from; i < to; i++)
	{
		Dictionary r = result[i];

		Dictionary source = r["sourceInterface"];
		Dictionary destination = r["destinationInterface"];

		//ERR_CONTINUE(!r.has("title"));
		//ERR_CONTINUE(!r.has("asset_id"));
		//ERR_CONTINUE(!r.has("author"));
		//ERR_CONTINUE(!r.has("author_id"));
		//ERR_CONTINUE(!r.has("interface_type_id"));
		//ERR_FAIL_COND(!interface_type_map.has(r["interface_type_id"]));
		//ERR_CONTINUE(!r.has("cost"));
		

		SlotItem* item = memnew(SlotItem);
		newitems.append(item);
		item->configure(r["name"],
						r["rid"],
						source["type"],
						destination["type"],
						source["label"],
						destination["label"],
						r["script"],
						r["animation"],
						r["enabled"]);
		m_items->add_child(item);
		item->connect("slot_selected", callable_mp(this, &ShowEditor::_onSlotSelected));
		item->connect("slot_enabled", callable_mp(this, &ShowEditor::_onSlotEnabled));

	}
	for (auto slot : newitems)
	{
		if (slot->m_slot_rid == m_selected_slot_id)
		{
			if (slot->m_selected)
			{
				slot->m_selected->set_pressed_no_signal(true);
			}
		}
	}
}


void ShowEditor::_update_connection_types()
{
	Dictionary default_connection_types;
	default_connection_types["Analog"] = DATA_SIGNAL_ANALOG;
	default_connection_types["Binary"] = DATA_SIGNAL_BINARY;
	default_connection_types["Note"] = DATA_SIGNAL_NOTE;
}

void ShowEditor::_onAddSlotClicked()
{
	auto currentscene = EditorNode::get_singleton()->get_edited_scene();
	if (!currentscene)
		return;

	const auto newslotrid = ShowEngine::get_singleton()->addSlot();
	m_selected_slot_id = newslotrid;
}


void ShowEditor::_onRemoveSlotClicked()
{
	ShowEngine::get_singleton()->removeSlot(m_selected_slot_id);
}

void ShowEditor::_onInterfaceManagerClicked()
{

}

void ShowEditor::_onSlotSelected(const RID rid)
{
	m_selected_slot_id = rid;
	const auto slot = ShowEngine::get_singleton()->getSlotInfo(rid);
	
	if (slot.is_empty())
		return;

	// TO DO show selected node
	// in header section between
	// "Remove" and "Interface Manager"

	//m_items_description->configure(r);
	//m_items_description->set_visible(true);
}

void ShowEditor::_onSlotEnabled(const RID p_rid, const bool enabled)
{
	ShowEngine::get_singleton()->setSlotEnabled(p_rid, enabled);
	

	//m_items_description->configure(r);
	//m_items_description->set_visible(true);
}

HBoxContainer* ShowEditor::_make_pages(int p_page, int p_page_count, int p_page_len, int p_total_items, int p_current_items)
{
	HBoxContainer* hbc = memnew(HBoxContainer);

	if (p_page_count < 2) {
		return hbc;
	}

	//do the mario
	int from = p_page - 5;
	if (from < 0) {
		from = 0;
	}
	int to = from + 10;
	if (to > p_page_count) {
		to = p_page_count;
	}

	hbc->add_spacer();
	hbc->add_theme_constant_override("separation", 5 * EDSCALE);

	Button* first = memnew(Button);
	first->set_text(TTR("First", "Pagination"));
	if (p_page != 0) {
		first->connect("pressed", callable_mp(this, &ShowEditor::_search).bind(0));
	}
	else {
		first->set_disabled(true);
		first->set_focus_mode(Control::FOCUS_NONE);
	}
	hbc->add_child(first);

	Button* prev = memnew(Button);
	prev->set_text(TTR("Previous", "Pagination"));
	if (p_page > 0) {
		prev->connect("pressed", callable_mp(this, &ShowEditor::_search).bind(p_page - 1));
	}
	else {
		prev->set_disabled(true);
		prev->set_focus_mode(Control::FOCUS_NONE);
	}
	hbc->add_child(prev);
	hbc->add_child(memnew(VSeparator));

	for (int i = from; i < to; i++) {
		if (i == p_page) {
			Button* current = memnew(Button);
			// Keep the extended padding for the currently active page (see below).
			current->set_text(vformat(" %d ", i + 1));
			current->set_disabled(true);
			current->set_focus_mode(Control::FOCUS_NONE);

			hbc->add_child(current);
		}
		else {
			Button* current = memnew(Button);
			// Add padding to make page number buttons easier to click.
			current->set_text(vformat(" %d ", i + 1));
			current->connect("pressed", callable_mp(this, &ShowEditor::_search).bind(i));

			hbc->add_child(current);
		}
	}

	Button* next = memnew(Button);
	next->set_text(TTR("Next", "Pagination"));
	if (p_page < p_page_count - 1) {
		next->connect("pressed", callable_mp(this, &ShowEditor::_search).bind(p_page + 1));
	}
	else {
		next->set_disabled(true);
		next->set_focus_mode(Control::FOCUS_NONE);
	}
	hbc->add_child(memnew(VSeparator));
	hbc->add_child(next);

	Button* last = memnew(Button);
	last->set_text(TTR("Last", "Pagination"));
	if (p_page != p_page_count - 1) {
		last->connect("pressed", callable_mp(this, &ShowEditor::_search).bind(p_page_count - 1));
	}
	else {
		last->set_disabled(true);
		last->set_focus_mode(Control::FOCUS_NONE);
	}
	hbc->add_child(last);

	hbc->add_spacer();

	return hbc;
}


void ShowEditor::_filter_connection_type_changed(int p_repository_id)
{
}

void ShowEditor::clear()
{
	if (m_items)
	{
		memdelete(m_items);
	}

	//m_items_description->set_visible(false);

	if (m_connection_top_page)
	{
		memdelete(m_connection_top_page);
	}

	if (m_connection_bottom_page)
	{
		memdelete(m_connection_bottom_page);
	}

	int page = 0;
	int pages = 1;
	int page_len = 10;
	int total_items = 1;

	m_selected_slot_id;

	m_connection_top_page = _make_pages(page, pages, page_len, total_items, 0);
	m_library_vb->add_child(m_connection_top_page);

	m_items = memnew(VBoxContainer);
	//_update_asset_items_columns();
	m_items->add_theme_constant_override("separation", 10 * EDSCALE);
	m_items->set_h_size_flags(SIZE_EXPAND_FILL);

	m_library_vb->add_child(m_items);



	m_connection_bottom_page = _make_pages(page, pages, page_len, total_items, 0);
	m_library_vb->add_child(m_connection_bottom_page);
}

void ShowEditor::_bind_methods()
{
}

void ShowEditor::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_READY:
		{
			add_theme_style_override("panel", get_theme_stylebox(SNAME("bg"), SNAME("AssetLib")));
			m_error_label->move_to_front();
		}
		break;
		case NOTIFICATION_ENTER_TREE:
		case NOTIFICATION_THEME_CHANGED:
			{
				m_error_tr->set_texture(get_theme_icon(SNAME("Error"), SNAME("EditorIcons")));
				m_filter->set_right_icon(get_theme_icon(SNAME("Search"), SNAME("EditorIcons")));
				m_library_scroll_bg->add_theme_style_override("panel", get_theme_stylebox(SNAME("panel"), SNAME("Tree")));
				//m_downloads_scroll->add_theme_style_override("panel", get_theme_stylebox(SNAME("panel"), SNAME("Tree")));
				m_error_label->add_theme_color_override("color", get_theme_color(SNAME("error_color"), SNAME("Editor")));
			}
			break;

		case NOTIFICATION_VISIBILITY_CHANGED: {
			if (is_visible()) {
	#ifndef ANDROID_ENABLED
				// Focus the search box automatically when switching to the Templates tab (in the Project Manager)
				// or switching to the AssetLib tab (in the editor).
				// The Project Manager's project filter box is automatically focused in the project manager code.
				m_filter->grab_focus();
	#endif

				if (initial_loading)
				{
					_slots_changed(); // Update when shown for the first time.
				}
			}
		} break;

		case NOTIFICATION_PROCESS:
		{
			// To Do
			// Process Input/Output Signal visualization

		} break;

		case NOTIFICATION_RESIZED:
		{
			//_update_asset_items_columns();
		}
		break;

		case EditorSettings::NOTIFICATION_EDITOR_SETTINGS_CHANGED:
		{
			//_update_repository_options();
		}
		break;
	}

}

void ShowEditor::_slots_changed()
{
	m_library_error->hide();
	m_library_info->set_text(TTR("Loading..."));
	m_library_info->show();

	m_connection_top_page->hide();
	m_connection_bottom_page->hide();
	m_items->hide();

	m_filter->set_editable(false);
	//m_sort->set_disabled(true);
	//m_categories->set_disabled(true);
	//m_type->set_disabled(true);

	//m_host = repository->get_item_metadata(p_repository_id);

	_search();
	initial_loading = false;
}


bool ShowEditor::_sort_updated(const Variant& first, const Variant& second)
{
	const int64_t first_lasttimestamp = Time::get_singleton()->get_unix_time_from_datetime_string(Dictionary(first)["last_modified"]);
	const int64_t second_lasttimestamp = Time::get_singleton()->get_unix_time_from_datetime_string(Dictionary(first)["last_modified"]);
	return first_lasttimestamp > second_lasttimestamp;
}
bool ShowEditor::_sort_updated_reversed(const Variant& first, const Variant& second)
{
	const int64_t first_lasttimestamp = Time::get_singleton()->get_unix_time_from_datetime_string(Dictionary(first)["last_modified"]);
	const int64_t second_lasttimestamp = Time::get_singleton()->get_unix_time_from_datetime_string(Dictionary(first)["last_modified"]);
	return first_lasttimestamp < second_lasttimestamp;
}

bool ShowEditor::_sort_names(const Variant& first, const Variant& second)
{
	const String first_name = Dictionary(first)["name"];
	const String second_name = Dictionary(first)["name"];
	return first_name > second_name;
}
bool ShowEditor::_sort_names_reversed(const Variant& first, const Variant& second)
{
	const String first_lasttimestamp = Dictionary(first)["name"];
	const String second_lasttimestamp = Dictionary(first)["name"];
	return first_lasttimestamp < second_lasttimestamp;
}

void ShowEditor::_onSlotListChanged()
{
	_search();
}


// ==================================================================================================================


ShowEditorPlugin::ShowEditorPlugin()
{

	_show_editor = memnew(ShowEditor);
	_show_editor->set_v_size_flags(Control::SIZE_EXPAND_FILL);

	// Panel Setup
	const auto mainscreencontrols = EditorNode::get_singleton()->get_main_screen_control();
	mainscreencontrols->add_child(_show_editor);
	
	_show_editor->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	_show_editor->hide();

}

ShowEditorPlugin::~ShowEditorPlugin()
{
}


void ShowEditorPlugin::shortcut_input(const Ref<InputEvent>& p_event)
{
	ERR_FAIL_COND(p_event.is_null());
	Ref<InputEventKey> k = p_event;
	if ((k.is_valid() && k->is_pressed() && !k->is_echo()) || Object::cast_to<InputEventShortcut>(*p_event))
	{
		if (ED_IS_SHORTCUT("editor/show_editor", p_event))
			EditorNode::get_singleton()->editor_select(get_index());
	}

}

void ShowEditorPlugin::make_visible(bool p_visible)
{
	if (p_visible && _show_editor) {
		_show_editor->show();
	}
	else {
		_show_editor->hide();
	}
}

void ShowEditorPlugin::edit(Object* p_object)
{
}

bool ShowEditorPlugin::handles(Object* p_object) const
{
	return false;
}

inline Dictionary ShowEditorPlugin::get_state() const
{
	return Dictionary();
}

const Ref<Texture2D> ShowEditorPlugin::get_icon() const
{
	return Ref<Texture2D>();
}

inline void ShowEditorPlugin::set_state(const Dictionary& p_state)
{
}

inline void ShowEditorPlugin::edited_scene_changed()
{
}
