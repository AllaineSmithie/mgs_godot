/**************************************************************************/
/*  audio_stream_pro.h                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             Metro Gaya Syste,                          */
/*                        https://deadline-entertainment.com              */
/**************************************************************************/
/* Copyright (c) 2022-present Deadline Entertainment All rights reserved. */
/**************************************************************************/

#ifndef AUDIO_STREAM_PRO_H
#define AUDIO_STREAM_PRO_H

#include "core/io/resource_loader.h"
#include "modules/deadline_audio_engine/DeadlineAudioEngine.h"
#include "modules/deadline_audio_engine/sfizz/src/sfizz.h"
#include "scene/resources/texture.h"
#include "servers/audio/audio_stream.h"
//#include "../sfizz/external/st_audiofile/thirdparty/dr_libs/dr_flac.h"

class AudioStreamPRO;

class AudioStreamPlaybackPRO : public AudioStreamPlayback {
	GDCLASS(AudioStreamPlaybackPRO, AudioStreamPlayback);

	enum {
		FADE_SIZE = 256
	};
	AudioFrame loop_fade[FADE_SIZE];
	int loop_fade_remaining = FADE_SIZE;

	//*pFlac = nullptr;
	uint32_t buffer_position = 0;
	bool active = false;
	bool preview_stream = false;
	int current_voice_id = -1;
	int loops = 0;
	uint8_t note = 0;
	uint8_t velocity = 0;
	uint8_t sequence_idx = 0;
	std::weak_ptr<sfz::Sfizz> player_reference;
	friend class AudioStreamPRO;

	Ref<AudioStreamPRO> audio_pro_stream;
	bool initialized = false;
	int region_id = -1;
	String file_path;

	juce::AudioSampleBuffer voice_buffer;
	inline void _fillRBuffer(int k, int p_frames, AudioFrame *p_buffer);

protected:
	virtual int mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames) override;
	//virtual float get_stream_sampling_rate() override;

public:
	virtual void start(double p_from_pos = 0.0) override;
	virtual void stop() override;
	virtual bool is_playing() const override;

	virtual int get_loop_count() const override; //times it looped

	virtual double get_playback_position() const override;
	virtual void seek(double p_time) override;

	virtual void tag_used_streams() override;

	AudioStreamPlaybackPRO() {}
	~AudioStreamPlaybackPRO();
};

class AudioStreamPRO : public AudioStream {
	GDCLASS(AudioStreamPRO, AudioStream);
	OBJ_SAVE_TYPE(AudioStream) //children are all saved as AudioStream, so they can be exchanged
	RES_BASE_EXTENSION("proaudiostr");

	friend class AudioStreamPlaybackPRO;
	friend class ResourceImporterPRO;

	//PackedByteArray data;

	//float sample_rate = 1.0;
	int channels = 1;
	float length = 0.0;
	bool loop = false;
	float loop_offset = 0.0;
	int region_id = -1;
	int note = -1;
	int velocity = -1;
	int sequence_idx = -1;
	void clear_data();

	bool preview_stream = false;
	std::shared_ptr<sfz::Sfizz> preview_player;
	String sfz_content;

	double bpm = 0;
	int beat_count = 0;
	int bar_beats = 4;

	String file_path;
	void set_preview_mode(const bool p_stream_is_in_preview);

protected:
	static void _bind_methods();

public:
	void set_loop(bool p_enable);
	virtual bool has_loop() const override;

	void set_loop_offset(double p_seconds);
	double get_loop_offset() const;

	void set_bpm(double p_bpm);
	virtual double get_bpm() const override;

	void set_beat_count(int p_beat_count);
	virtual int get_beat_count() const override;

	void set_bar_beats(int p_bar_beats);
	virtual int get_bar_beats() const override;

	virtual Ref<AudioStreamPlayback> instantiate_playback() override;
	virtual String get_stream_name() const override;

	void set_file_path(const String p_file_path);
	String get_file_path();
	//void set_data(const Vector<uint8_t> &p_data);
	//Vector<uint8_t> get_data() const;

	virtual double get_length() const override; //if supported, otherwise return 0

	virtual bool is_monophonic() const override;

	AudioStreamPRO();
	virtual ~AudioStreamPRO();
};

#endif // AUDIO_STREAM_PRO_H
