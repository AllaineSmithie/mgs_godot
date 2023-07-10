/**************************************************************************/
/*  libgodot_audio_driver.h                                               */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             Metro Gaya System                          */
/*                        https://deadline-entertainment.com              */
/**************************************************************************/
/* Copyright (c) 2022-present Deadline Entertainment All rights reserved. */
/**************************************************************************/

#ifndef LIBGODOT_AUDIO_DRIVER_H
#define LIBGODOT_AUDIO_DRIVER_H

#ifdef LIBRARY_ENABLED

#include "core/os/mutex.h"
#include "core/os/thread.h"
#include "core/templates/safe_refcount.h"
#include "servers/audio_server.h"

class AudioDriverLibGodot : public AudioDriver {
	SafeFlag active;

	unsigned int channels = 0;
	int mix_rate = 0;
	int buffer_frames = 0;
	int target_latency_ms = 0;
	float real_latency = 0.0;
	bool using_audio_client_3 = false;

	Error init_from_external(bool p_reinit = false);

	Mutex mutex;

	PackedStringArray audio_device_get_list(bool p_input);
	PackedInt32Array interleaved_buffer;

public:
	virtual const char *get_name() const override {
		return "EXTERNAL";
	}

	virtual Error init() override;
	virtual void start() override;
	virtual int get_mix_rate() const override;
	virtual SpeakerMode get_speaker_mode() const override;
	virtual float get_latency() override;

	virtual void lock() override;
	virtual void unlock() override;
	virtual void finish() override;

	virtual PackedStringArray get_output_device_list() override;
	virtual String get_output_device() override;
	virtual void set_output_device(const String &p_name) override;

	void process_samples(int num_channels, int samples_per_block, float *const *p_buffer);

	virtual Error input_start() override;
	virtual Error input_stop() override;

	virtual PackedStringArray get_input_device_list() override;
	virtual String get_input_device() override;
	virtual void set_input_device(const String &p_name) override;

	AudioDriverLibGodot();
};

#endif // LIBRARY_ENABLED

#endif // LIBGODOT_AUDIO_DRIVER_H
