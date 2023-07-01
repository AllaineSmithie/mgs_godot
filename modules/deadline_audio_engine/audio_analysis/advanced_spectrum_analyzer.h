#ifndef ADVANCED_SPECTRUM_ANALYZER_H
#define ADVANCED_SPECTRUM_ANALYZER_H

#include "asa_resource.h"
#include "piano_roll.h"
#include "midi_note_container.h"
#include <core/object/ref_counted.h>
#include <core/variant/variant.h>
#include <scene/gui/dialogs.h>
#include <scene/gui/file_dialog.h>
#include <scene/gui/grid_container.h>
#include <scene/gui/panel_container.h>
#include <scene/gui/split_container.h>
#include <scene/gui/scroll_container.h>
#include <scene/gui/box_container.h>
#include <scene/gui/option_button.h>
#include <scene/gui/spin_box.h>
#include <scene/gui/texture_rect.h>
#include <scene/main/node.h>
#include <vector>

#if TOOLS_ENABLED
#include <editor/editor_scale.h>
#else
#endif

class ViewPanner;

// =================================================================
class ASA : public RefCounted {
	GDCLASS(ASA, RefCounted);
	//static ASA* singleton;
public:
	ASA();
private:

	static void _analyze_thread(void* p_analysis);
	static void _generate_images(void* p_analysis);


	static float remap(const float input_min, const float input_max, const float value, const float output_min, const float output_max);

	static std::vector<float> convert_magnitudes_to_exponential_frequencies(const PackedFloat32Array& linear_magnitudes, const float low_high_exponent, const int resolution, const float tuning, const float frequency_to_count_ratio, float& max_magnitude);

	static std::vector<std::vector<float>> analyze_subspectrum(PackedFloat32Array samples, const int sample_rate, const int fft_size, const float low_high_exponent, const float overamplification_multiplier, const int hop_size, const int resolution, const float tuning);


	//void _update_emit(ObjectID p_id);

protected:
	static void _bind_methods();
public:
	//static ASA* get_singleton() { return singleton; }
	void analyze_spectrum(ASAOptions* p_analysis, const bool p_force_recalc);
	void generate_images(ASAOptions* p_analysis);

	PackedInt32Array guess_notes(ASAOptions* p_options);
};


// =======================================================
class ASANode : public Audio
{
	GDCLASS(ASANode, Audio);
private:
	int sample_rate = 44100;
	Vector2 texture_size;
	Vector2 min_texture_size;
	Ref<ASA> analyzer;
	Ref<Gradient> color_scheme; // = preload("res://themes/spectrum_color_scheme.tres")
	Ref<AudioStream> base_stream;
	Ref<AudioStreamPlayback> playback;
	HashMap<ObjectID, ASAOptions> last_options;
	HashMap<ObjectID, ASAOptions> analyzes;

	List<Sprite2D*> _generate_images(ASAOptions* p_analysis);
	ASAOptions* _analyze_spectrum(const Ref<AudioStream>& p_audio_stream);

protected:
	static void _bind_methods();
	void _notification(int p_what);
public:

	//ASAOptions options;

	Ref<ASA> get_analyzer() { return analyzer; }
	void set_analyzer(const Ref<ASA>& p_analyzer) { analyzer = p_analyzer; }
	void set_sample_rate(int p_sample_rate) { sample_rate = p_sample_rate; }
	int get_sample_rate() { return sample_rate; }
	Vector2 get_texture_size() { return texture_size; }
	void set_texture_size(Vector2 p_texture_size) { texture_size = p_texture_size; }

	// bad written -> ref is alway undefined
	PackedInt32Array guess_notes(const ObjectID& id);
	Array analyze_spectrum_images(const Ref<AudioStream>& p_audio_stream);
	List<Sprite2D*> analyze_spectrum_sprites(const Ref<AudioStream>& p_audio_stream);
	List<Sprite2D*>* get_sprites(const ObjectID& id);
	Array get_images(const ObjectID& id) const;
	ASANode();
};

// =======================================================
class AudioGraphEditor : public HBoxContainer
{
	GDCLASS(AudioGraphEditor, HBoxContainer);
	Color color = { 0.0, 0.0, 0.0, 0.3 };
	int resolution = 9;
	HashMap<int, MIDINoteTrackContainer*> midi_note_tracks;
protected:
	static void _bind_methods();
	void _notification(int p_what);
	void _draw();
public:
	void set_color(const Color& p_color)			{ color = p_color; }
	Color get_color()								{ return color; }
	//void set_texture_size(Vector2 p_texture_size)	{ texture_size = p_texture_size; }
	//Vector2 get_texture_size()					{ return texture_size; queue_redraw(); }
	void set_resolution(int p_resolution)			{ resolution = p_resolution; queue_redraw(); }
	int get_resolution()							{ return resolution; }
	void remove_all_notes();
	void add_notes(const PackedInt32Array &p_notes);
	AudioGraphEditor();
};


class Timeline : public Range {
	GDCLASS(Timeline, Range);

	Ref<AudioStream> current_audio_stream;
	int name_limit = 0;
	Range* zoom = nullptr;
	//Range* h_scroll = nullptr;
	float play_position_pos = 0.0f;

	Control* play_position = nullptr; //separate control used to draw so updates for only position changed are much faster
	HScrollBar* hscroll = nullptr;

	void _zoom_changed(double);
	void _length_changed(double p_new_len);

	void _play_position_draw();
	Rect2 hsize_rect;

	bool editing = false;
	bool use_fps = false;

	Ref<ViewPanner> panner;
	void _pan_callback(Vector2 p_scroll_vec, Ref<InputEvent> p_event);
	void _zoom_callback(float p_zoom_factor, Vector2 p_origin, Ref<InputEvent> p_event);

	bool dragging_timeline = false;
	bool dragging_hsize = false;
	float dragging_hsize_from = 0.0f;
	float dragging_hsize_at = 0.0f;

	virtual void gui_input(const Ref<InputEvent>& p_event) override;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	int get_name_limit() const;

	float get_zoom_scale() const;
	float get_active_width() const;

	void set_zoom(Range* p_zoom);
	Range* get_zoom() const { return zoom; }

	void set_play_position(float p_pos);
	float get_play_position() const;
	void update_play_position();

	void update_values();

	void set_use_fps(bool p_use_fps);
	bool is_using_fps() const;

	void set_current_stream(const Ref<Resource>& p_resource);
	Ref<Resource> get_current_stream();

	void set_hscroll(HScrollBar* p_hscroll);

	virtual CursorShape get_cursor_shape(const Point2& p_pos) const override;

	Timeline();
};


// =======================================================
class AudioToMidiEditor : public VBoxContainer
{
	GDCLASS(AudioToMidiEditor, VBoxContainer);

	float play_position_pos = 0.0f;
	float vertical_zoom = 1.0f;
	//Range* play_position = nullptr; //separate control used to draw so updates for only position changed are much faster
	HBoxContainer* main = nullptr;
	HSlider* zoom_slider = nullptr;
	TextureRect* zoom_icon = nullptr;
	VBoxContainer* bottom_vb = nullptr;

	ScrollContainer* graph_scroll_container = nullptr;
	Ref<AudioStream> current_audio_stream;

	PianoRollBar* piano_bar = nullptr;
	ScrollContainer* piano_scroll_container = nullptr;
	Timeline* timeline = nullptr;

	MIDINoteNode* note_being_added = nullptr;

	void _on_graph_spacer_gui_input(const Ref<InputEvent>& p_event);
	void remove_notes();
	void add_notes(const PackedInt32Array& notes);
	PackedVector3Array get_notes();
	Rect2 hsize_rect;

	//HBoxContainer* graph_hb= nullptr;
	//PanelContainer* graph_spacer = nullptr;
	//Node2D* graph_area = nullptr;
	//Node2D* spectrum_sprites = nullptr;
	AudioGraphEditor* stripes = nullptr;
	ASANode* analyzer_node = nullptr;
	Button* play_button = nullptr;

	HScrollBar* hscroll = nullptr;

	List<Sprite2D*> get_spectrum_sprites(const ObjectID& p_id);
	Array get_spectrum_images(const ObjectID& p_id) const;
	PackedInt32Array get_guessed_notes();
	//void update_graph_spacer(Size2 p_scale = Size2(1.0, 1.0), bool p_only_x = false, bool p_only_y = false);
	void _play_pressed(bool p_pressed);

	void _timeline_changed(double p_pos, bool p_drag, bool p_timeline_only);
	void _update_scroll(double p_value);
	//void _zoom_horizontal(float p_zoom_x);
	void _zoom_vertical_step(bool p_zoom_in);
	//void _zoom_vertical(float p_zoom_y);
protected:
	static void _bind_methods();
	void _notification(int p_what);
public:
	int sprites_original_width = 0;
	void reset();
	void set_audio_stream(const Ref<AudioStream>& p_audio_stream);
	Ref<AudioStream> get_audio_stream();

	bool calculate_new_spectrum_sprites();
	bool calculate_new_notes();
	//void zoom(Size2 p_scale);

	void update_zoom();

	bool analyze_spectrum();
	bool guess_midi_notes(bool p_mono_mode);

	bool is_analyzed(const ObjectID& p_id);
	AudioToMidiEditor();
};

#endif // ADVANCED_SPECTRUM_ANALYZER_H
