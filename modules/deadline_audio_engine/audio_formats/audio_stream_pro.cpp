#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO
#define DR_FLAC_NO_OGG
#include <core/config/project_settings.h>
#include "audio_stream_pro.h"

#include "core/io/file_access.h"

inline void AudioStreamPlaybackPRO::_fillRBuffer(int k, int p_frames, AudioFrame* p_buffer)
{
	int j = k;
	ERR_FAIL_COND(buffer_position + p_frames > (uint32_t)voice_buffer.getNumSamples());
	for (; j < k + p_frames; j++)
	{
		p_buffer[j].l = voice_buffer.getSample(0, buffer_position + j - k);
	}
	for (j = k; j < k + p_frames; j++)
	{
		p_buffer[j].r = voice_buffer.getSample(1, buffer_position + j - k);
	}

}

int AudioStreamPlaybackPRO::mix(AudioFrame* p_buffer, float p_rate_scale, int p_frames)
{
	
	if(!active)
	{
		return 0;
	}

	// shortcut
	if (buffer_position == 0 && p_frames == voice_buffer.getNumSamples())
	{
		if (preview_stream)
		{
			if (player_reference.expired())
				return 0;

			player_reference.lock()->renderVoiceBlock(current_voice_id, const_cast<float**>(voice_buffer.getArrayOfWritePointers()), voice_buffer.getNumSamples(), voice_buffer.getNumChannels() / 2);
		}
		else
		{
			// always master bus for now
			DeadlineAudioEngine::get_singleton()->render_voice_block(0, current_voice_id, const_cast<float**>(voice_buffer.getArrayOfWritePointers()), voice_buffer.getNumSamples(), voice_buffer.getNumChannels() / 2);
		}
		voice_buffer.applyGain(juce::Decibels::decibelsToGain<float>(3.0));
		_fillRBuffer(0, p_frames, p_buffer);
	}
	else
	{
		//const bool request_too_large = p_frames > voice_buffer.getNumSamples();
		int todo = p_frames;

		if (preview_stream)
		{
			if (player_reference.expired())
				return 0;
			int k = 0;
			if (buffer_position > 0)
			{
				int _todo = MIN(todo, voice_buffer.getNumSamples() - buffer_position);
				_fillRBuffer(k, _todo, p_buffer);
				buffer_position += _todo;
				if (buffer_position >= voice_buffer.getNumSamples())
					buffer_position = 0;
				todo -= _todo;
				k += _todo;
			}
			while (todo > voice_buffer.getNumSamples())
			{
				// safety
				if (player_reference.expired())
					return 0;

				player_reference.lock()->renderVoiceBlock(0, const_cast<float**>(voice_buffer.getArrayOfWritePointers()), voice_buffer.getNumSamples(), voice_buffer.getNumChannels() / 2);
				voice_buffer.applyGain(juce::Decibels::decibelsToGain<float>(3.0));

				_fillRBuffer(k, voice_buffer.getNumSamples(), p_buffer);

				todo -= voice_buffer.getNumSamples();
				k += voice_buffer.getNumSamples();
			}
			if (todo > 0)
			{

				// safety
				if (player_reference.expired())
					return 0;
				player_reference.lock()->renderVoiceBlock(0, const_cast<float**>(voice_buffer.getArrayOfWritePointers()), voice_buffer.getNumSamples(), voice_buffer.getNumChannels() / 2);
				voice_buffer.applyGain(juce::Decibels::decibelsToGain<float>(3.0));

				_fillRBuffer(k, todo, p_buffer);
				buffer_position = todo;
			}
		}
		else
		{
			int k = 0;
			if (buffer_position > 0)
			{
				int _todo = MIN(todo, voice_buffer.getNumSamples() - buffer_position);
				_fillRBuffer(k, _todo, p_buffer);
				buffer_position += _todo;
				if (buffer_position >= voice_buffer.getNumSamples())
					buffer_position = 0;
				todo -= _todo;
				k += _todo;
			}
			while (todo > voice_buffer.getNumSamples())
			{
				// always master bus for now
				DeadlineAudioEngine::get_singleton()->render_voice_block(0, region_id, const_cast<float**>(voice_buffer.getArrayOfWritePointers()), voice_buffer.getNumSamples(), voice_buffer.getNumChannels() / 2);
				voice_buffer.applyGain(juce::Decibels::decibelsToGain<float>(3.0));

				_fillRBuffer(k, p_frames, p_buffer);

				todo -= voice_buffer.getNumSamples();
				k += voice_buffer.getNumSamples();
			}
			if (todo > 0)
			{
				// always master bus for now
				DeadlineAudioEngine::get_singleton()->render_voice_block(0, region_id, const_cast<float**>(voice_buffer.getArrayOfWritePointers()), voice_buffer.getNumSamples(), voice_buffer.getNumChannels() / 2);
				voice_buffer.applyGain(juce::Decibels::decibelsToGain<float>(3.0));

				_fillRBuffer(k, p_frames, p_buffer);
				buffer_position = todo;
			}
		}
	}

	return p_frames;
	
}

/*float AudioStreamPlaybackPRO::get_stream_sampling_rate()
{

	return audio_pro_stream->sample_rate;
}*/

void AudioStreamPlaybackPRO::start(double p_from_pos)
{
	if (!initialized)
		return;

	active = true;
	seek(p_from_pos);


	if (preview_stream)
	{
		if (player_reference.expired())
			return;
		current_voice_id = player_reference.lock()->playRegionByID(region_id);
	}
	else
	{
		// =========================
		// !! All Master Bus for now!
		current_voice_id = DeadlineAudioEngine::get_singleton()->play_audio_stream(0, region_id);
		ERR_FAIL_COND_MSG(current_voice_id  > -1, "Region" + String::num(region_id, 0) + "could not be played");
	}
	loops = 0;
	//begin_resample();
}

void AudioStreamPlaybackPRO::stop()
{
	active = false;
	if (preview_stream)
	{
		if (player_reference.expired())
			return;
		const bool result = player_reference.lock()->stopRegionByID(region_id);
		ERR_FAIL_COND_MSG(result == false, "Region could not be stopped");
	}
	else
	{
		// =========================
		// !! All Master Bus for now!
		DeadlineAudioEngine::get_singleton()->stop_audio_stream(0, region_id);
	}
}

bool AudioStreamPlaybackPRO::is_playing() const
{
	return active;
}

int AudioStreamPlaybackPRO::get_loop_count() const
{
	return loops;
}

double AudioStreamPlaybackPRO::get_playback_position() const
{
	if (preview_stream)
	{
		if (player_reference.expired())
			return 0.0;
		const int position = player_reference.lock()->getVoicePosition(current_voice_id);
		return double(position) / double(AudioServer::get_singleton()->get_mix_rate());
	}
	else
	{
		// =========================
		// !! All Master Bus for now!
		const int position = DeadlineAudioEngine::get_singleton()->get_voice_position(0, current_voice_id);
		return double(position) / double(AudioServer::get_singleton()->get_mix_rate());
	}
}

void AudioStreamPlaybackPRO::seek(double p_time)
{
	if (!active) {
		return;
	}

	if (p_time >= audio_pro_stream->get_length())
	{
		p_time = 0;
	}

	const int frames_mixed = AudioServer::get_singleton()->get_mix_rate() * p_time;
	if (preview_stream)
	{
		if (player_reference.expired())
			return;
		player_reference.lock()->setVoicePosition(current_voice_id, frames_mixed);
	}
	else
	{
		// =========================
		// !! All Master Bus for now!
		DeadlineAudioEngine::get_singleton()->set_voice_position(0, current_voice_id, frames_mixed);
		//drflac_seek_to_pcm_frame(pFlac, frames_mixed);
	}
}

void AudioStreamPlaybackPRO::tag_used_streams()
{
	audio_pro_stream->tag_used(get_playback_position());
}

AudioStreamPlaybackPRO::~AudioStreamPlaybackPRO()
{
	stop();
	if (!preview_stream)
	{
		// =========================
		// !! All Master Bus for now!
		DeadlineAudioEngine::get_singleton()->remove_sound_from_bus(file_path, 0);
	}
}

Ref<AudioStreamPlayback> AudioStreamPRO::instantiate_playback()
{
	Ref<AudioStreamPlaybackPRO> audio_file;
	audio_file.instantiate();

	audio_file->audio_pro_stream = Ref<AudioStreamPRO>(this);
	audio_file->active = false;

	ERR_FAIL_COND_V_MSG(file_path.is_empty(), audio_file, "Could not load sample, please provide a valid file");
	// all preview mode for now
	audio_file->initialized = false;

	audio_file->file_path = file_path;

	// set buffer size here
	audio_file->voice_buffer.setSize(AudioServer::get_singleton()->get_channel_count() * 2, AudioServer::get_singleton()->thread_get_mix_buffer_size());

	audio_file->preview_stream = preview_stream;
	if (preview_stream)
		audio_file->player_reference = preview_player;
	audio_file->region_id = region_id;
	audio_file->note = note;
	audio_file->velocity = velocity;
	audio_file->sequence_idx = sequence_idx;
	audio_file->initialized = true;
	
	return audio_file;
}

String AudioStreamPRO::get_stream_name() const
{

	return "Pro Audio Stream"; //return stream_name;
}

void AudioStreamPRO::clear_data()
{
	if (preview_player)
	{
		preview_player->allSoundOff();
		preview_player = nullptr;
	}
}

void AudioStreamPRO::set_preview_mode(const bool p_stream_is_in_preview)
{
	preview_stream = p_stream_is_in_preview;
	if (p_stream_is_in_preview)
	{	
		preview_player = std::make_shared<sfz::Sfizz>();
		preview_player->setSampleRate(AudioServer::get_singleton()->get_mix_rate());

		const int samples_per_block = AudioServer::get_singleton()->thread_get_mix_buffer_size();
		preview_player->setSamplesPerBlock(samples_per_block);
		
	}
	else
	{
		if (preview_player)
		{
			preview_player = nullptr;
		}
	}
}


void AudioStreamPRO::set_loop(bool p_enable) {
	loop = p_enable;
}

bool AudioStreamPRO::has_loop() const {

	return loop;
}

void AudioStreamPRO::set_loop_offset(double p_seconds) {
	loop_offset = p_seconds;
}

double AudioStreamPRO::get_loop_offset() const {
	return loop_offset;
}

double AudioStreamPRO::get_length() const {
	return length;
}

bool AudioStreamPRO::is_monophonic() const {
	return false;
}

void AudioStreamPRO::set_bpm(double p_bpm) {
	ERR_FAIL_COND(p_bpm < 0);
	bpm = p_bpm;
	emit_changed();
}

double AudioStreamPRO::get_bpm() const {
	return bpm;
}

void AudioStreamPRO::set_beat_count(int p_beat_count) {
	ERR_FAIL_COND(p_beat_count < 0);
	beat_count = p_beat_count;
	emit_changed();
}

int AudioStreamPRO::get_beat_count() const {
	return beat_count;
}

void AudioStreamPRO::set_bar_beats(int p_bar_beats) {
	ERR_FAIL_COND(p_bar_beats < 0);
	bar_beats = p_bar_beats;
	emit_changed();
}

void AudioStreamPRO::set_file_path(const String p_file_path)
{
	file_path = p_file_path;
	if (preview_stream)
	{
		const String project_path = ProjectSettings::get_singleton()->get_resource_path();
		String preview_sfz_content = "<control> hint_ram_based=1\n";
		if (!file_path.is_empty())
		{
			const String global_path = ProjectSettings::get_singleton()->globalize_path(file_path);
			preview_sfz_content += "<region> sample=" + global_path
				+ " amp_veltrack=0"
				+ " ampeg_attack=0"
				+ " ampeg_release=0"
				+ " lokey=0"
				+ " hikey=127"
				+ " pitch_keycenter=64"
				+ " lovel=0"
				+ " hivel=127";

			preview_player->loadSfzString(project_path.ascii().get_data(), preview_sfz_content.ascii().get_data());
			length = preview_player->getRegionLengthSeconds(0);
		}
		else
		{
			preview_player->loadSfzString(project_path.ascii().get_data(), preview_sfz_content.ascii().get_data());
			length = 0.0f;
		}
		region_id = 0;
		note = 64;
		velocity = 127;
		sequence_idx = 0;
	}
	else
	{
		// =========================
		// !! All Master Bus for now!
		region_id = DeadlineAudioEngine::get_singleton()->add_sound_to_bus(file_path, 0);
		length = DeadlineAudioEngine::get_singleton()->get_region_length_seconds(0, region_id);

		note = DeadlineAudioEngine::get_singleton()->get_sound_velocity_in_bus(file_path, 0);
		velocity = DeadlineAudioEngine::get_singleton()->get_sound_note_in_bus(file_path, 0);
		sequence_idx = DeadlineAudioEngine::get_singleton()->get_sound_sequence_idx_in_bus(file_path, 0);

		ERR_FAIL_COND_MSG(note < 0 || velocity < 0 || sequence_idx < 0, "Note's MIDI information invalid");
	}
}

String AudioStreamPRO::get_file_path()
{
	return file_path;
}

int AudioStreamPRO::get_bar_beats() const {
	return bar_beats;
}

void AudioStreamPRO::_bind_methods() {

	/*ClassDB::bind_method(D_METHOD("set_data", "data"), &AudioStreamPRO::set_data);
	ClassDB::bind_method(D_METHOD("get_data"), &AudioStreamPRO::get_data);*/

	ClassDB::bind_method(D_METHOD("set_file_path", "file_path"), &AudioStreamPRO::set_file_path);
	ClassDB::bind_method(D_METHOD("get_file_path"), &AudioStreamPRO::get_file_path);

	ClassDB::bind_method(D_METHOD("set_loop", "enable"), &AudioStreamPRO::set_loop);
	ClassDB::bind_method(D_METHOD("has_loop"), &AudioStreamPRO::has_loop);

	ClassDB::bind_method(D_METHOD("set_loop_offset", "seconds"), &AudioStreamPRO::set_loop_offset);
	ClassDB::bind_method(D_METHOD("get_loop_offset"), &AudioStreamPRO::get_loop_offset);

	ClassDB::bind_method(D_METHOD("set_bpm", "bpm"), &AudioStreamPRO::set_bpm);
	ClassDB::bind_method(D_METHOD("get_bpm"), &AudioStreamPRO::get_bpm);

	ClassDB::bind_method(D_METHOD("set_beat_count", "count"), &AudioStreamPRO::set_beat_count);
	ClassDB::bind_method(D_METHOD("get_beat_count"), &AudioStreamPRO::get_beat_count);

	ClassDB::bind_method(D_METHOD("set_bar_beats", "count"), &AudioStreamPRO::set_bar_beats);
	ClassDB::bind_method(D_METHOD("get_bar_beats"), &AudioStreamPRO::get_bar_beats);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "file_path"), "set_file_path", "get_file_path");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "bpm", PROPERTY_HINT_RANGE, "0,400,0.01,or_greater"), "set_bpm", "get_bpm");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "beat_count", PROPERTY_HINT_RANGE, "0,512,1,or_greater"), "set_beat_count", "get_beat_count");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bar_beats", PROPERTY_HINT_RANGE, "2,32,1,or_greater"), "set_bar_beats", "get_bar_beats");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "loop", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_loop", "has_loop");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "loop_offset", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_loop_offset", "get_loop_offset");
}

AudioStreamPRO::AudioStreamPRO() {
	set_preview_mode(true);
}

AudioStreamPRO::~AudioStreamPRO() {
	clear_data();
}
