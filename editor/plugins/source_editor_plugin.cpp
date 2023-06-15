
#include "source_editor_plugin.h"
//#include "../ShowEngine.h"
#include <scene/gui/menu_button.h>
#include <scene/gui/tab_container.h>
#include <core/os/time.h>
#include <scene/gui/separator.h>
#include <assert.h>
#include "editor/editor_scale.h"

//#include "../digishow/DigiShow/digishow_common.h"
//#include "../digishow/DigiShow/digishow_slot.h"
/*

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


const char* SourceEditor::sort_key[SORT_MAX] = {
	"updated",
	"updated",
	"name",
	"name",
	"cost",
	"cost",
};

const char* SourceEditor::sort_text[SORT_MAX] = {
	TTRC("Newest Updated"),
	TTRC("Oldest Updated"),
	TTRC("Name (A-Z)"),
	TTRC("Name (Z-A)"),
	TTRC("ID (0-X)"), // "cost" stores the SPDX license name in the Godot Asset Library.
	TTRC("ID (X-0)"), // "cost" stores the SPDX license name in the Godot Asset Library.
};

const char* SourceEditor::filter_key[FILTER_MAX] = {
	"all",
	"inputs",
	"outputs",
};

const char* SourceEditor::filter_text[FILTER_MAX] = {
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
}*/

SourceEditor* SourceEditor::singleton = nullptr;

SourceEditor* SourceEditor::get_singleton()
{
	return singleton;
}


SourceEditor::SourceEditor()
{
	ERR_FAIL_COND(singleton != nullptr);
	singleton = this;

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

	m_header_bar = memnew(TabContainer);
	library_main->add_child(m_header_bar);
	m_header_bar->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	m_header_bar->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	m_header_bar->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	

	// TO DO -> Add Detailed Connection Description
	//m_description = nullptr;

	set_process(true);
	set_process_shortcut_input(true); // Global shortcuts since there is no main element to be focused.


	// Signals
	//ShowEngine::get_singleton()->connect("slot_list_changed", callable_mp(this, &SourceEditor::_onSlotListChanged));

}
SourceEditor::~SourceEditor()
{
	ERR_FAIL_COND(singleton != this);
	singleton = nullptr;
}

void SourceEditor::_bind_methods()
{
}

void SourceEditor::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_READY:
		{
			add_theme_style_override("panel", get_theme_stylebox(SNAME("bg"), SNAME("AssetLib")));
		}
		break;
		case NOTIFICATION_ENTER_TREE:
		case NOTIFICATION_THEME_CHANGED:
			{
			}
			break;

		case NOTIFICATION_VISIBILITY_CHANGED: {
			if (is_visible()) {
	#ifndef ANDROID_ENABLED
				// Focus the search box automatically when switching to the Templates tab (in the Project Manager)
				// or switching to the AssetLib tab (in the editor).
				// The Project Manager's project filter box is automatically focused in the project manager code.
	#endif
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

void SourceEditor::register_extension(SourceEditorPluginExtension* p_extension)
{
	ERR_FAIL_COND_MSG(p_extension->editor_page == nullptr, "Editor extension has no editor page");
		
	for (auto i = 0; i < m_header_bar->get_child_count(); ++i)
	{
		Node* child = m_header_bar->get_child(i, false);
		if (child == p_extension->editor_page)
		{
			// do nothing on double assignment
			return;
		}
	}
	m_header_bar->add_child(p_extension->editor_page);
}

void SourceEditor::unregister_extension(SourceEditorPluginExtension* p_extension)
{
	for (auto i = 0; i < m_header_bar->get_child_count(); ++i)
	{
		Node* child = m_header_bar->get_child(i, false);
		if (child == p_extension->editor_page)
		{
			m_header_bar->remove_child(child);
		}
	}
}


//VBoxContainer* get_main_screen_control();


// ==================================================================================================================

Vector<Ref<SourceEditorPluginExtension>> SourceEditorPlugin::extensions;

SourceEditorPlugin* SourceEditorPlugin::singleton = nullptr;
SourceEditorPlugin::SourceEditorPlugin()
{
	singleton = this;

	_source_editor = memnew(SourceEditor);
	_source_editor->set_v_size_flags(Control::SIZE_EXPAND_FILL);

	// Panel Setup
	const auto mainscreencontrols = EditorNode::get_singleton()->get_main_screen_control();
	mainscreencontrols->add_child(_source_editor);

	_source_editor->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	_source_editor->hide();


	// =========================================
	// Editor Settings
	// Interfaces Maneger

	// EditorSettings::get_singleton()->set_setting(Variant::INT, PROPERTY_HINT_ENUM,, 0, display_scale_hint_string, PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_RESTART_IF_CHANGED)

}

SourceEditorPlugin::~SourceEditorPlugin()
{
	// Panel Setup
	const auto mainscreencontrols = EditorNode::get_singleton()->get_main_screen_control();
	mainscreencontrols->remove_child(_source_editor);
	memdelete(_source_editor);
}


void SourceEditorPlugin::shortcut_input(const Ref<InputEvent>& p_event)
{
	ERR_FAIL_COND(p_event.is_null());
	Ref<InputEventKey> k = p_event;
	if ((k.is_valid() && k->is_pressed() && !k->is_echo()) || Object::cast_to<InputEventShortcut>(*p_event))
	{
		if (ED_IS_SHORTCUT("editor/source_editor", p_event))
			EditorNode::get_singleton()->editor_select(get_index());
	}

}

void SourceEditorPlugin::_notification(int p_what)
{
	/*
	switch (p_what) {
	case NOTIFICATION_INTERNAL_PROCESS: {
	} break;
	}*/
}

void SourceEditorPlugin::make_visible(bool p_visible)
{
	if (p_visible && _source_editor) {
		_source_editor->show();
	}
	else {
		_source_editor->hide();
	}
}

void SourceEditorPlugin::edit(Object* p_object)
{
	for (auto ext : extensions)
	{
		if (ext->handles(p_object))
		{
			ext->edit(p_object);
			return;
		}
	}
}

bool SourceEditorPlugin::handles(Object* p_object) const
{
	for (auto ext : extensions)
	{
		if (ext->handles(p_object))
			return true;
		
	}
	return false;
}

inline Dictionary SourceEditorPlugin::get_state() const
{
	return Dictionary();
}

const Ref<Texture2D> SourceEditorPlugin::get_icon() const
{
	return Ref<Texture2D>();
}

inline void SourceEditorPlugin::set_state(const Dictionary& p_state)
{
}

void SourceEditorPlugin::clear()
{
	extensions.clear();
}

void SourceEditorPlugin::register_extension(Ref<SourceEditorPluginExtension> p_extension, bool p_at_front)
{
	ERR_FAIL_COND(p_extension.is_null());
	if (p_at_front) {
		extensions.insert(0, p_extension);
	}
	else {
		extensions.push_back(p_extension);
	}
}

void SourceEditorPlugin::unregister_extension(Ref<SourceEditorPluginExtension> p_extension)
{
	ERR_FAIL_COND(p_extension.is_null());
	extensions.erase(p_extension);

}

inline void SourceEditorPlugin::edited_scene_changed()
{
	for (auto e : extensions)
		e->edited_scene_changed();
}

SourceEditorPluginExtension::~SourceEditorPluginExtension()
{
	if (editor_page != nullptr)
		memdelete(editor_page);

}
