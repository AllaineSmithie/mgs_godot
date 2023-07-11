/**************************************************************************/
/*  port_audio_virtual_driver.cpp                                         */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "port_audio_virtual_driver.h"

#include "modules/portaudio/port_audio.h"
#include "core/config/project_settings.h"
#include "core/os/os.h"

PortAudioVirtualDriver *PortAudioVirtualDriver::singleton = nullptr;

void PortAudioVirtualDriver::trigger_process_samples() {
	mix_audio();
}

Error PortAudioVirtualDriver::init() {
	active.clear();
	exit_thread.clear();
	samples_in = nullptr;

	PortAudio::get_singleton()->init_main_stream();

	Ref<PortAudioStream> pa_stream = PortAudio::get_singleton()->get_main_stream();

	if (pa_stream.is_valid()) {
		StringName audio_driver_setting = "audio/driver/mix_rate";
		if (!ProjectSettings::get_singleton()->has_setting(audio_driver_setting))
			audio_driver_setting = "audio/driver/sample_rate";
		ProjectSettings::get_singleton()->set_setting(audio_driver_setting, pa_stream->get_sample_rate());

		buffer_frames = (int)((float)(pa_stream->get_frames_per_buffer()));
	}

	channels = get_channels();
	samples_in = memnew_arr(int32_t, (size_t)buffer_frames * channels);
	/*if (use_threads) {
		thread.start(PortAudioVirtualDriver::thread_func, this);
	}*/

	return OK;
}

void PortAudioVirtualDriver::thread_func(void *p_udata) {
	PortAudioVirtualDriver *ad = static_cast<PortAudioVirtualDriver *>(p_udata);

	Ref<PortAudioStream> pa_stream = PortAudio::get_singleton()->get_main_stream();

	//uint64_t usdelay = (pa_stream->get_frames_per_buffer() / float(pa_stream->get_sample_rate())) * 1000000;

	while (!ad->exit_thread.is_set()) {
		if (ad->active.is_set()) {
			if (PortAudio::get_singleton()->get_main_stream_input_buffer(ad->samples_in, ad->buffer_frames) == 0) {
				//ad->semaphore.wait();
				continue;
			}
			ad->start_counting_ticks();

			ad->lock();
			ad->audio_server_process(ad->buffer_frames, ad->samples_in);
			ad->unlock();
			PortAudio::get_singleton()->push_main_stream_buffer(ad->samples_in, ad->buffer_frames * ad->channels);
			ad->stop_counting_ticks();
			//ad->semaphore.wait();
		}
	}
}

void PortAudioVirtualDriver::start() {
	PortAudio::get_singleton()->start_main_stream();
	active.set();
}

int PortAudioVirtualDriver::get_mix_rate() const {
	Ref<PortAudioStream> pa_stream = PortAudio::get_singleton()->get_main_stream();
	if (pa_stream.is_null())
		return 44100;
	else
		return pa_stream->get_sample_rate();
}

AudioDriver::SpeakerMode PortAudioVirtualDriver::get_speaker_mode() const {
	return speaker_mode;
}

void PortAudioVirtualDriver::lock() {
	mutex.lock();
}

void PortAudioVirtualDriver::unlock() {
	mutex.unlock();
}

void PortAudioVirtualDriver::set_use_threads(bool p_use_threads) {
	use_threads = p_use_threads;
}

void PortAudioVirtualDriver::frames_per_buffer_changed() {
	Ref<PortAudioStream> pa_stream = PortAudio::get_singleton()->get_main_stream();
	if (pa_stream.is_null())
		return;

	channels = get_channels();
	lock();
	buffer_frames = pa_stream->get_frames_per_buffer();
	int32_t *_samples_in = samples_in;
	samples_in = memnew_arr(int32_t, buffer_frames * channels);
	memdelete_arr(_samples_in);
	unlock();
}

void PortAudioVirtualDriver::set_stream_resolution(int p_stream_size_bytes) {
}

void PortAudioVirtualDriver::set_speaker_mode(SpeakerMode p_mode) {
	speaker_mode = p_mode;
}

void PortAudioVirtualDriver::set_mix_rate(int p_rate) {
	Ref<PortAudioStream> pa_stream = PortAudio::get_singleton()->get_main_stream();
	if (pa_stream.is_null())
		return;

	pa_stream->set_sample_rate(p_rate);
}

uint32_t PortAudioVirtualDriver::get_channels() const {
	ERR_FAIL_COND_V(PortAudio::get_singleton()->get_main_stream().is_null(), 2);
	return MAX(PortAudio::get_singleton()->get_main_stream()->get_output_channel_count(), PortAudio::get_singleton()->get_main_stream()->get_input_channel_count());
}

void PortAudioVirtualDriver::mix_audio() {
	ERR_FAIL_COND(!active.is_set()); // If not active, should not mix.
	if (PortAudio::get_singleton()->get_main_stream_input_buffer(samples_in, buffer_frames) == 0) {
		return;
	}
	start_counting_ticks();

	lock();
	audio_server_process(buffer_frames, samples_in);
	unlock();
	PortAudio::get_singleton()->push_main_stream_buffer(samples_in, buffer_frames * channels);
	stop_counting_ticks();
	//semaphore.wait();
}

void PortAudioVirtualDriver::finish() {
	if (use_threads) {
		exit_thread.set();
		if (thread.is_started()) {
			thread.wait_to_finish();
		}
	}
	PortAudio::get_singleton()->finish_main_stream();

	if (samples_in) {
		memdelete_arr(samples_in);
	}
}

void PortAudioVirtualDriver::stop() {
	active.clear();
}

float PortAudioVirtualDriver::get_latency() {
	Ref<PortAudioStream> pa_stream = PortAudio::get_singleton()->get_main_stream();
	if (pa_stream.is_null())
		return (float)buffer_frames / 44100.0;

	return (float)buffer_frames / pa_stream->get_sample_rate();
}

PortAudioVirtualDriver::PortAudioVirtualDriver() {
	singleton = this;
}
