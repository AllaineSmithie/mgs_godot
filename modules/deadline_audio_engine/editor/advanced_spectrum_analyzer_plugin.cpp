#include "advanced_spectrum_analyzer_plugin.h"

void AdvancedSpectrumAnalyzerPlugin::edit(Object* p_object)
{
	AudioStreamPlayer* stream_player = Object::cast_to<AudioStreamPlayer>(p_object);
	if (!stream_player)
		return;

	if (edited_audio.find(stream_player->get_instance_id()) > -1)
	{
		analyzer_tabs->set_current_tab(edited_audio.find(stream_player->get_instance_id()));
		if (AudioToMidiEditor* object_test = Object::cast_to<AudioToMidiEditor>(analyzer_tabs->get_current_tab_control()))
		{
			audio_to_midi_editor = object_test;
			set_edited_audio(stream_player->get_stream());
		}
		else
		{
			audio_to_midi_editor = nullptr;
		}
		return;
	}

	edited_audio.push_back(stream_player->get_instance_id());
	audio_to_midi_editor = memnew(AudioToMidiEditor);
	audio_to_midi_editor->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	audio_to_midi_editor->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	analyzer_tabs->add_child(audio_to_midi_editor);

	set_edited_audio(stream_player->get_stream());
}

bool AdvancedSpectrumAnalyzerPlugin::handles(Object* p_object) const
{
	AudioStreamPlayer* stream_player = Object::cast_to<AudioStreamPlayer>(p_object);
	if (stream_player)
		return true;
	return false;
}

void AdvancedSpectrumAnalyzerPlugin::make_visible(bool p_visible)
{
}

void AdvancedSpectrumAnalyzerPlugin::selected_notify()
{
}

// ==========================================================================
void AdvancedSpectrumAnalyzerPlugin::_bind_methods()
{
}

void AdvancedSpectrumAnalyzerPlugin::_window_changed(bool p_visible)
{
	make_floating->set_visible(!p_visible);
}

void AdvancedSpectrumAnalyzerPlugin::_menu_item_pressed(int p_index)
{
	switch (p_index) {
	case ASA_OPEN_AUDIO: {
		//String base_path = FileSystemDock::get_singleton()->get_current_path().get_base_dir();
	} break;
	case ASA_SAVE_MIDI: {
		String base_path = FileSystemDock::get_singleton()->get_current_path().get_base_dir();
		save_midi_file_dialog->popup_centered();
		//EditorNode::get_singleton()->save_resource(edited_shaders[index].shader);
		//shader_create_dialog->config(base_path.path_join("new_shader"), false, false, 2);
		//shader_create_dialog->popup_centered();
	} break;
	case ASA_SHOW_CONVERSION_OPTIONS: {
		//audio_to_midi_editor->_on_conversion_options_button_pressed();
	} break;
	case ASA_SHOW_GENERAL_OPTIONS: {
		//audio_to_midi_editor->_on_general_options_button_pressed();
	} break;

	default:
		break;
	}
}

void AdvancedSpectrumAnalyzerPlugin::set_edited_audio(const Ref<AudioStream>& p_audio_stream)
{
	current_edited_audio = p_audio_stream;
	audio_to_midi_editor->set_audio_stream(p_audio_stream);	
}

AdvancedSpectrumAnalyzerPlugin::AdvancedSpectrumAnalyzerPlugin()
{
	window_wrapper = memnew(WindowWrapper);
	window_wrapper->set_window_title(TTR("Audio Analysis"));
	window_wrapper->set_margins_enabled(true);

	save_midi_file_dialog = memnew(FileDialog);
	add_child(save_midi_file_dialog);
	save_midi_file_dialog->set_min_size(Size2(400, 200));
	save_midi_file_dialog->set_title("Save to MIDI file");
	save_midi_file_dialog->set_filters({ "*.mid ; MIDI files" });
	save_midi_file_dialog->set_current_dir(ProjectSettings::get_singleton()->globalize_path("res://"));
	save_midi_file_dialog->set_current_path("/");

	main_container = memnew(VBoxContainer);
	main_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_container->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	VBoxContainer* vb = memnew(VBoxContainer);

	HBoxContainer* menu_hb = memnew(HBoxContainer);
	main_container->add_child(menu_hb);
	file_menu = memnew(MenuButton);
	file_menu->set_text(TTR("File"));
	file_menu->get_popup()->add_item(TTR("Open audio..."), ASA_OPEN_AUDIO);
	file_menu->get_popup()->add_item(TTR("Save midifile..."), ASA_SAVE_MIDI);
	file_menu->get_popup()->add_separator();
	file_menu->get_popup()->add_item(TTR("Show conversion options"), ASA_SHOW_CONVERSION_OPTIONS);
	file_menu->get_popup()->add_item(TTR("Show general options"), ASA_SHOW_GENERAL_OPTIONS);
	file_menu->get_popup()->add_separator();
	menu_hb->add_child(file_menu);

	if (window_wrapper->is_window_available()) {
		Control* padding = memnew(Control);
		padding->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		menu_hb->add_child(padding);

		make_floating = memnew(ScreenSelect);
		make_floating->set_flat(true);
		make_floating->set_tooltip_text(TTR("Make the audio analysis editor floating."));
		make_floating->connect("request_open_in_screen", callable_mp(window_wrapper, &WindowWrapper::enable_window_on_screen).bind(true));

		menu_hb->add_child(make_floating);
		window_wrapper->connect("window_visibility_changed", callable_mp(this, &AdvancedSpectrumAnalyzerPlugin::_window_changed));
	}
	main_split = memnew(HSplitContainer);
	main_split->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_split->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	main_container->add_child(main_split);
	Ref<Shortcut> make_floating_shortcut = ED_SHORTCUT_AND_COMMAND("audio_analysis/make_floating", TTR("Make Floating"));
	window_wrapper->set_wrapped_control(main_container, make_floating_shortcut);

	main_split->add_child(vb);
	vb->set_custom_minimum_size(Size2(80, 300) * EDSCALE);

	Ref<StyleBoxEmpty> empty;
	empty.instantiate();

	options_panel = memnew(VBoxContainer);
	options_panel->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	vb->add_child(options_panel);

	analyze_audio = memnew(Button);
	analyze_audio->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	analyze_audio->set_custom_minimum_size(Size2(80, 16));
	analyze_audio->connect("pressed", callable_mp(this, &AdvancedSpectrumAnalyzerPlugin::_analyze_audio_clicked));
	analyze_audio->set_text(TTR("Analyze Audio..."));
	options_panel->add_child(analyze_audio);

	audio_to_midi_button = memnew(Button);
	audio_to_midi_button->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	audio_to_midi_button->set_custom_minimum_size(Size2(80, 16));
	audio_to_midi_button->connect("pressed", callable_mp(this, &AdvancedSpectrumAnalyzerPlugin::_guess_midi_notes_clicked));
	audio_to_midi_button->set_text(TTR("Audio to MIDI"));
	audio_to_midi_button->hide();
	options_panel->add_child(audio_to_midi_button);

	mono_mode_checkbox = memnew(CheckBox);
	mono_mode_checkbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	mono_mode_checkbox->set_custom_minimum_size(Size2(80, 16));
	mono_mode_checkbox->connect("pressed", callable_mp(this, &AdvancedSpectrumAnalyzerPlugin::_mono_mode_checked));
	mono_mode_checkbox->set_text(TTR("Mono Mode"));
	mono_mode_checkbox->hide();
	options_panel->add_child(mono_mode_checkbox);

	analyzer_tabs = memnew(TabContainer);
	analyzer_tabs->set_tabs_visible(false);
	analyzer_tabs->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	analyzer_tabs->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	main_split->add_child(analyzer_tabs);
	analyzer_tabs->add_theme_style_override("panel", empty);

	button = EditorNode::get_singleton()->add_bottom_panel_item(TTR("Audio Analysis"), window_wrapper);

	// Defer connect because Editor class is not in the binding system yet.
	//EditorNode::get_singleton()->call_deferred("connect", "resource_saved", callable_mp(this, &AdvancedSpectrumAnalyzerPlugin::_resource_saved), CONNECT_DEFERRED);
}

void AdvancedSpectrumAnalyzerPlugin::_mono_mode_checked()
{
}

void AdvancedSpectrumAnalyzerPlugin::_guess_midi_notes_clicked()
{
	ERR_FAIL_COND_MSG(audio_to_midi_editor == nullptr, "Error - no editor");
	ERR_FAIL_COND_MSG(current_edited_audio.is_null(), "No audio selected");
	if (!audio_to_midi_editor->guess_midi_notes(mono_mode_checkbox->is_pressed()))
		ERR_FAIL();
}
void AdvancedSpectrumAnalyzerPlugin::_analyze_audio_clicked()
{
	ERR_FAIL_COND_MSG(audio_to_midi_editor == nullptr, "Error - no editor");
	ERR_FAIL_COND_MSG(current_edited_audio.is_null(), "No audio selected");

	if (!audio_to_midi_editor->analyze_spectrum())
	{
		audio_to_midi_button->hide();
		mono_mode_checkbox->hide();
	}
	else
	{
		audio_to_midi_button->show();
		mono_mode_checkbox->show();
	}

	// retrigger
	audio_to_midi_editor->set_audio_stream(current_edited_audio);
}

void AdvancedSpectrumAnalyzerPlugin::_save_to_midi_clicked()
{
	ERR_FAIL_COND_MSG(audio_to_midi_editor == nullptr, "Error - no editor");
	ERR_FAIL_COND_MSG(current_edited_audio.is_null(), "No audio selected");
	ERR_FAIL_COND_MSG(!audio_to_midi_editor->is_analyzed(current_edited_audio->get_instance_id()), "Audio not analyzed yet. Hit analyze to extract MIDI information");

	save_midi_file_dialog->popup_centered();
}

AdvancedSpectrumAnalyzerPlugin::~AdvancedSpectrumAnalyzerPlugin()
{
}
