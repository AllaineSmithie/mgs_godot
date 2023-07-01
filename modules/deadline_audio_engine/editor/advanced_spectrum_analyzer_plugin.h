#ifndef ADVANCED_SPECTRUM_ANALYZER_PLUGIN_H
#define ADVANCED_SPECTRUM_ANALYZER_PLUGIN_H
#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "editor/editor_scale.h"
#include "editor/window_wrapper.h"
#include "editor/filesystem_dock.h"
#include "core/config/project_settings.h"
#include "editor/editor_command_palette.h"
#include "../DeadlineAudioEngine.h"
#include "../audio_analysis/advanced_spectrum_analyzer.h"

// =======================================================
class AdvancedSpectrumAnalyzerPlugin : public EditorPlugin
{
	GDCLASS(AdvancedSpectrumAnalyzerPlugin, EditorPlugin);
	Ref<AudioStream> current_edited_audio;

	enum {
		ASA_OPEN_AUDIO,
		ASA_SAVE_MIDI,
		ASA_SHOW_CONVERSION_OPTIONS,
		ASA_SHOW_GENERAL_OPTIONS,
		ASA_MAX
	};

	AudioToMidiEditor* audio_to_midi_editor = nullptr;

	Button* button = nullptr;
	MenuButton* file_menu = nullptr;

	Button* make_floating = nullptr;
	WindowWrapper* window_wrapper = nullptr;
	LocalVector<ObjectID> edited_audio;

	TabContainer* analyzer_tabs = nullptr;
	HSplitContainer* main_split = nullptr;

	VBoxContainer* options_panel = nullptr;
	VBoxContainer* main_container = nullptr;

	Button* analyze_audio = nullptr;
	Button* audio_to_midi_button = nullptr;
	CheckBox* mono_mode_checkbox = nullptr;
	FileDialog* save_midi_file_dialog = nullptr;
	void _save_to_midi_clicked();
	void _guess_midi_notes_clicked();
	void _mono_mode_checked();
	void _analyze_audio_clicked();
	
protected:

	static void _bind_methods();
	void _window_changed(bool p_visible);
	void _menu_item_pressed(int p_index);
public:
	virtual String get_name() const override { return "Audio Analysis"; }
	virtual void edit(Object* p_object) override;
	virtual bool handles(Object* p_object) const override;
	virtual void make_visible(bool p_visible) override;
	virtual void selected_notify() override;
	void set_edited_audio(const Ref<AudioStream>& p_audio_stream);
	Ref<AudioStream> get_edited_audio() { return current_edited_audio; }
	AdvancedSpectrumAnalyzerPlugin();
	~AdvancedSpectrumAnalyzerPlugin();
};

#endif // ADVANCED_SPECTRUM_ANALYZER_PLUGIN_H
