/*
  ==============================================================================

    JuceLib.h
    Created: 15 Mar 2023 11:38:14pm
    Author:  RedneckJack

  ==============================================================================
*/

#ifndef DEADLINEAUDIOENGINE_H
#define DEADLINEAUDIOENGINE_H

#include "juce/juce_AudioSampleBuffer.h"
#include "core/io/resource_saver.h"
#include "core/object/ref_counted.h"
#include "scene/audio/audio_stream_player.h"
#include "sfizz/src/sfizz.hpp"
#include "sfizz/src/sfizz_message.h"
#include "SFZParser.h"
#include "AudioSampleStreamPlayer.h"


// ==============================================================================
// Mic Position Resource
class MicPosition: public Resource
{
	GDCLASS(MicPosition, Resource);

protected:
	static void _bind_methods()
	{
		ClassDB::bind_method(D_METHOD("set_full_name", "full_name"), &MicPosition::set_full_name);
		ClassDB::bind_method(D_METHOD("get_full_name"),&MicPosition::get_full_name);
		ClassDB::bind_method(D_METHOD("set_short_name", "short_name"), &MicPosition::set_short_name);
		ClassDB::bind_method(D_METHOD("get_short_name"),&MicPosition::get_short_name);
		ClassDB::bind_method(D_METHOD("set_sort_index", "sort_index"), &MicPosition::set_sort_index);
		ClassDB::bind_method(D_METHOD("get_sort_index"), &MicPosition::get_sort_index);
		ClassDB::bind_method(D_METHOD("set_volume", "volume"), &MicPosition::set_volume);
		ClassDB::bind_method(D_METHOD("get_volume"), &MicPosition::get_volume);
		ClassDB::bind_method(D_METHOD("set_pan", "pan"), &MicPosition::set_pan);
		ClassDB::bind_method(D_METHOD("get_pan"), &MicPosition::get_pan);
		ClassDB::bind_method(D_METHOD("set_phase_invert", "phase_invert"), &MicPosition::set_phase_invert);
		ClassDB::bind_method(D_METHOD("get_phase_invert"), &MicPosition::get_phase_invert);

		ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "full_name"), "set_full_name", "get_full_name");
		ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "short_name"), "set_short_name", "get_short_name");
		ADD_PROPERTY(PropertyInfo(Variant::INT, "sort_index", PROPERTY_HINT_RANGE, "0,50,1,or_greater"), "set_sort_index", "get_sort_index");
		ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "volume", PROPERTY_HINT_RANGE, "-180.0,12.0,0.001,or_greater,suffix:dB"), "set_volume", "get_volume");
		ADD_PROPERTY(PropertyInfo(Variant::BOOL, "phase_invert"), "set_phase_invert", "get_phase_invert");

	}
private:
	Dictionary opcodes;
	StringName full_name;
	StringName short_name;
	int sort_index = 0;
	real_t volume = 0.0;
	real_t pan = 0.0;
	bool phase_invert = false;
public:
	void set_opcodes(Dictionary p_opcodes)			{ opcodes = p_opcodes; }
	Dictionary get_opcodes()						{ return opcodes; }

	void set_full_name(StringName p_full_name)		{ full_name = p_full_name; }
	StringName get_full_name()						{ return full_name; }
	void set_short_name(StringName p_short_name)	{ short_name = p_short_name; }
	StringName get_short_name()						{ return short_name; }
	void set_sort_index(int p_sort_index)			{ sort_index = p_sort_index; }
	int get_sort_index()							{ return sort_index; }
	void set_volume(real_t p_volume)				{ volume = p_volume;}
	real_t get_volume()								{ return volume; }
	void set_pan(real_t p_pan)						{ pan = p_pan;}
	real_t get_pan()								{ return pan; }
	void set_phase_invert(bool p_phase_invert)		{ phase_invert = p_phase_invert;}
	bool get_phase_invert()							{ return phase_invert; }
	
};


// ==============================================================================
// SFZ Resource
class SFZResource : public Resource
{
	GDCLASS(SFZResource, Resource);

protected:
	static void _bind_methods()
	{
		ClassDB::bind_method(D_METHOD("set_opcodes", "opcodes"), &SFZResource::set_opcodes);
		ClassDB::bind_method(D_METHOD("get_opcodes"), &SFZResource::get_opcodes);

		ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "opcodes"), "set_opcodes", "get_opcodes");
	}
private:
	Dictionary opcodes;
public:
	void set_opcodes(Dictionary p_opcodes)	{ opcodes = p_opcodes; }
	Dictionary get_opcodes()				{ return opcodes; }
};


// ==============================================================================
// SFZ Region Resource
class SFZRegion: public SFZResource
{
	GDCLASS(SFZRegion, SFZResource);

	NodePath sound_file;
protected:
	static void _bind_methods()
	{
		ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "sound_file"), "set_sound_file", "get_sound_file");
	}
public:
	void set_sound_file(NodePath p_sound_file) { sound_file = p_sound_file; }
	NodePath get_sound_file() { return sound_file; }
};


// ==============================================================================
// Instrument Resource
class Instrument : public SFZResource
{
	GDCLASS(Instrument, SFZResource);

	SFZParser sfz_parser;
	Array mic_channels;
	//String sfz_file_content;
protected:
	static void _bind_methods()
	{
		//ClassDB::bind_method(D_METHOD("get_sfz_file_content"), &SFZInstrument::get_sfz_file_content);
		ClassDB::bind_method(D_METHOD("set_mic_channels"), &Instrument::set_mic_channels);
		ClassDB::bind_method(D_METHOD("get_mic_channels"), &Instrument::get_mic_channels);
		ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "mic_channels", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("MicChannel")), "set_mic_channels", "get_mic_channels");
		ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "master_opcodes"), "set_master_opcodes", "get_master_opcodes");
		ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "global_opcodes"), "set_global_opcodes", "get_global_opcodes");
		ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "control_opcodes"), "set_control_opcodes", "get_control_opcodes");
		ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "definitions"), "set_definitions", "get_definitions");
	}
public:
	void set_mic_channels(Array p_mic_channels) { mic_channels = p_mic_channels; }
	Array get_mic_channels()					{ return mic_channels; }

	Error load_file(const String& p_path);
	Error save_file(const String& p_path, const Ref<Resource>& p_resource);
	//Variant get_sfz_file_content() { return sfz_file_content; }
};


// ==============================================================================
// SFZ Resource Saver
class ResourceFormatSaverSFZ : public ResourceFormatSaver {
	GDCLASS(ResourceFormatSaverSFZ, ResourceFormatSaver);
public:
	virtual Error save(const String& p_path, const Ref<Resource>& p_resource, uint32_t p_flags = 0);
	virtual bool recognize(const Ref<Resource>& p_resource) const;
	virtual void get_recognized_extensions(const Ref<Resource>& p_resource, List<String>* r_extensions) const;
};


// ==============================================================================
// SFZ Resource Loader
class ResourceFormatLoaderSFZ : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderSFZ, ResourceFormatLoader);
public:
	virtual Ref<Resource> load(const String& p_path, const String& p_original_path, Error* r_error = NULL);
	virtual void get_recognized_extensions(List<String>* r_extensions) const;
	virtual bool handles_type(const String& p_type) const;
	virtual String get_resource_type(const String& p_path) const;
};


// ==============================================================================
// Instrument Group Node
class SoundRegionNode : public AudioSampleStreamPlayer
{
	GDCLASS(SoundRegionNode, AudioSampleStreamPlayer);
private:

protected:
	void _validate_property(PropertyInfo& p_property) const;
	static void bind_methods();
	StringName mic_channel;
public:
	void set_mic_channel(const StringName& p_bus);
	StringName get_mic_channel();
};

// ==============================================================================
// Instrument Group Node
class InstrumentGroupNode : public Synth
{
	GDCLASS(InstrumentGroupNode, Synth);
private:
	StringName mic_channel;

protected:
	void _validate_property(PropertyInfo& p_property) const;
	static void bind_methods();
public:
	void set_mic_channel(const StringName& p_bus);
	StringName get_mic_channel() const;
	PackedStringArray get_configuration_warnings() const;
};

// ==============================================================================
// Instrument Node
class InstrumentNode : public Synth
{
	GDCLASS(InstrumentNode, Synth);
private:
	Ref<Instrument> sfz_instrument_resource;
	StringName bus;
protected:
	void _validate_property(PropertyInfo& p_property) const;
	static void bind_methods();
public:
	void set_master_bus(const StringName& p_bus);
	StringName get_master_bus() const;
	void set_instrument(Ref<Instrument> p_sfz_instrument);
	Ref<Instrument> get_instrument() const;
};


// ==============================================================================
// Synth Instrument
class SynthInstrument : public Object
{
	GDCLASS(SynthInstrument, Object);

	struct ClientData
	{
		sfz::Sfizz* synth = nullptr;
		sfz::Client* client = nullptr;
		std::string* blob = nullptr;
		std::vector<char>* msgbuf = nullptr;
		std::string* pathbuf = nullptr;
	};

	StringName name;
	uint32_t uuid;
	sfz::ClientPtr client_ptr;
	HashMap<uint32_t, String> sfz_regions_content;

	// Map storing the filepath 
	// and the id of the note inside the sampler
	// the id correspondes to sequence_pos
	// max 256*128*128 note idxs
	const uint32_t max_idx = 4194304;
	uint8_t seq_length = 0;
	HashMap<String, uint8_t> sample_sequence_idx;
	HashMap<String, uint8_t> sample_note_idx;
	HashMap<String, uint8_t> sample_velocity_idx;
	HashMap<String, uint32_t> sample_idx_in_reversed;
	HashMap<uint32_t, String> reversed_idx;

	ClientData cdata;

	sfz::ClientPtr client;
	std::string blob;
	std::vector<char> msgbuf;
	std::string pathbuf;

	void _update_sfz_player();
protected:

	static void bind_methods();

public:

	std::unique_ptr<sfz::Sfizz> sfzPlayer = nullptr;

	void set_name(StringName p_name);
	StringName get_name();
	void set_uuid(uint32_t p_uuid);
	uint32_t get_uuid();

	const bool get_voice_buffer_copy(const int p_region_id, float** p_out, const int p_num_samples, const int p_num_stereo_channels);
	void render_voice_block(const int p_region_id, float** p_out, const int p_num_samples, const int p_num_stereo_channels);

	// returns the region id of the added sound
	int add_sound_to_bus(const String p_path);
	void remove_sound_from_bus(const String p_path);
	int get_sound_note_in_bus(const String p_path) const;
	int get_sound_velocity_in_bus(const String p_path) const;
	int get_sound_sequence_idx_in_bus(const String p_path) const;

	SynthInstrument();
	~SynthInstrument();

};



// ==============================================================================
// Audio Engine
class DeadlineAudioEngine : public Object {
  GDCLASS(DeadlineAudioEngine, Object);

	List<SynthInstrument*> sfizzPlayer;
	static void _mix_audios(void* p_self);
	void _mix_audio();
	juce::AudioSampleBuffer sfz_mix_buffer;
	std::atomic_bool lock_render;
protected:
	static void _bind_methods();

	static DeadlineAudioEngine* singleton;
public:
	DeadlineAudioEngine();
	~DeadlineAudioEngine();

	void init_audio_server();

	void on_bus_layout_changed();
	SynthInstrument* add_synth_to_bus(const int p_bus_index);
	void load_instrument(String p_file);
	bool remove_instrument();

	int add_sound_to_bus(const String p_path, const StringName p_bus_name);
	int add_sound_to_bus(const String p_path, const int p_bus_idx);
	void remove_sound_from_bus(const String p_path, const int p_bus_idx);

	const bool render_voice_block(const int p_bus_idx, const int p_region_id, float** p_out, const int p_num_samples, const int p_num_stereo_channels);
	

	void set_voice_position(const int p_bus_idx, const int p_voice_id, const int p_position_frames);
	int get_voice_position(const int p_bus_idx, const int p_voice_id) const;
	int get_region_length_samples(const int p_bus_idx, const int p_region_id) const;
	float get_region_length_seconds(const int p_bus_idx, const int p_region_id) const;

	bool play_audio_stream(const int bus_idx, const uint8_t p_midi_note, const uint8_t p_velocity, const uint8_t p_sequence_idx = -1);
	// returns the voice id, if region could be played
	int play_audio_stream(const int bus_idx, const int p_region_id);
	bool stop_audio_stream(const int p_bus_idx, const uint8_t p_midi_note, const uint8_t p_velocity, const uint8_t p_sequence_idx = -1);
	bool stop_audio_stream(const int p_bus_idx, const int p_region_id);

	const bool get_voice_buffer_copy(const int p_bus_idx, const int p_voice_id, float** p_out, const int p_num_samples, const int p_num_stereo_channels);

	int get_sound_note_in_bus(const String p_path, const StringName p_bus_name) const;
	int get_sound_note_in_bus(const String p_path, const int p_idx) const;
	int get_sound_velocity_in_bus(const String p_path, const StringName p_bus_name) const;
	int get_sound_velocity_in_bus(const String p_path, const int p_idx) const;
	int get_sound_sequence_idx_in_bus(const String p_path, const StringName p_bus_name) const;
	int get_sound_sequence_idx_in_bus(const String p_path, const int p_idx) const;

	static DeadlineAudioEngine* get_singleton();
};

#endif
