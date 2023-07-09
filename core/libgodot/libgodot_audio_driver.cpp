/**************************************************************************/
/*  libgodot_audio_driver.cpp                                             */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             Metro Gaya System                          */
/*                        https://deadline-entertainment.com              */
/**************************************************************************/
/* Copyright (c) 2022-present Deadline Entertainment					  */
/**************************************************************************/

#ifdef LIBRARY_ENABLED

#include "libgodot_audio_driver.h"
#include <modules/deadline_audio_engine/juce/juce_AudioSampleBuffer.h>
#include <modules/deadline_audio_engine/juce/juce_AudioDataConverters.h>

#include "core/config/project_settings.h"
#include "core/os/os.h"

// SourceFormat 
using ExternalFormat = juce::AudioData::Format<juce::AudioData::Float32, juce::AudioData::BigEndian>;
// DestFormat
using InternalFormat = juce::AudioData::Format<int32_t, juce::AudioData::NativeEndian>;

Error AudioDriverLibGodot::init_from_external(bool p_reinit) {

	channels = ProjectSettings::get_singleton()->get_setting("audio/driver/num_channels", 2);
	if (ProjectSettings::get_singleton()->has_setting("audio/driver/mix_rate"))
		mix_rate = ProjectSettings::get_singleton()->get_setting("audio/driver/mix_rate", 44100);
	else
		mix_rate = ProjectSettings::get_singleton()->get_setting("audio/driver/sample_rate", 44100);
	buffer_frames = ProjectSettings::get_singleton()->get_setting("audio/driver/samples_per_block", 2);
	target_latency_ms = (int)((real_t)buffer_frames / mix_rate * 1000.0);
	real_latency = 0.0;

	lock();
	interleaved_buffer.resize(buffer_frames * channels);
	unlock();
	return OK;
}

Error AudioDriverLibGodot::init() {
	init_from_external();

	Error err = init_from_external();
	if (err != OK) {
		ERR_PRINT("External Audio Driver Connection: init_from_external error");
	}
	active.set();
	return OK;
}

int AudioDriverLibGodot::get_mix_rate() const {
	return mix_rate;
}

float AudioDriverLibGodot::get_latency() {
	return real_latency;
}

AudioDriver::SpeakerMode AudioDriverLibGodot::get_speaker_mode() const {
	return get_speaker_mode_by_total_channels(channels);
}

PackedStringArray AudioDriverLibGodot::audio_device_get_list(bool p_input) {
	PackedStringArray list;
	// To Do get all devices
	list.push_back(String("External"));
	//if (p_input)
	//else
	return list;
}

PackedStringArray AudioDriverLibGodot::get_output_device_list() {
	return audio_device_get_list(false);
}

String AudioDriverLibGodot::get_output_device() {
	lock();
	// To Do -> retrieve external device name
	String name = "External";
	//String name = audio_output.device_name;
	unlock();
	return name;
}

void AudioDriverLibGodot::set_output_device(const String& p_name) {
	lock();
	//audio_output.new_device = p_name;
	unlock();
}

void AudioDriverLibGodot::process_samples(int num_channels, int samples_per_block, float* const* p_buffer) {

	if (!active.is_set())
		return;

	juce::AudioSampleBuffer ad(p_buffer, num_channels, samples_per_block);
	
	lock();

	for (auto i = 0; i < samples_per_block; ++i) {
		for (auto c = 0; c < num_channels; ++c) {
			interleaved_buffer.set(i * c, ad.getSample(c, i));
		}
	}

	audio_server_process(ad.getNumSamples(), interleaved_buffer.ptrw());

	for (auto i = 0; i < samples_per_block; ++i) {
		for (auto c = 0; c < num_channels; ++c) {
			ad.setSample(c, i, interleaved_buffer[i * c]);
		}
	}
	/*
	for (auto i = 0; i < num_stereo_channels; ++i)
	{
		juce::AudioData::deinterleaveSamples(juce::AudioData::InterleavedSource<ExternalFormat>    { interleaved_buffer.getReadPointer(i), interleaved_buffer.getNumChannels()},
											 juce::AudioData::NonInterleavedDest<ExternalFormat>   { tmp_buffer.getArrayOfWritePointers(), tmp_buffer.getNumChannels()},
											 tmp_buffer.getNumSamples());

		juce::FloatVectorOperations::fill(ad.getWritePointer(i), 0.0, ad.getNumSamples());
		ad.copyFrom(i, 0, tmp_buffer.getReadPointer(0), tmp_buffer.getNumSamples());
		if (i + 1 < ad.getNumChannels())
		{
			juce::FloatVectorOperations::fill(ad.getWritePointer(i + 1), 0.0, ad.getNumSamples());
			ad.copyFrom(i + 1, 0, tmp_buffer.getReadPointer(1), tmp_buffer.getNumSamples());
		}
		
	}*/
	unlock();
}

void AudioDriverLibGodot::start() {
	/*if (audio_output.audio_client) {
		HRESULT hr = audio_output.audio_client->Start();
		if (hr != S_OK) {
			ERR_PRINT("WASAPI: Start failed");
		}
		else {
			audio_output.active.set();
		}
	}*/
}

void AudioDriverLibGodot::lock() {
	mutex.lock();
}

void AudioDriverLibGodot::unlock() {
	mutex.unlock();
}

void AudioDriverLibGodot::finish() {
	// To Do -> send "finish" to host -> whatever to do then :D
}

Error AudioDriverLibGodot::input_start() {
	/*Error err = init_input_device();
	if (err != OK) {
		ERR_PRINT("WASAPI: init_input_device error");
		return err;
	}

	if (audio_input.active.is_set()) {
		return FAILED;
	}

	audio_input.audio_client->Start();
	audio_input.active.set();*/
	return OK;
}

Error AudioDriverLibGodot::input_stop() {
	/*if (audio_input.active.is_set()) {
		audio_input.audio_client->Stop();
		audio_input.active.clear();
		*/
		return OK;
	//}

	//return FAILED;
}

PackedStringArray AudioDriverLibGodot::get_input_device_list() {
	return audio_device_get_list(true);
}

String AudioDriverLibGodot::get_input_device() {
	lock();
	//String name = audio_input.device_name;
	unlock();

	return "External";
}

void AudioDriverLibGodot::set_input_device(const String& p_name) {
	lock();
	
	unlock();
}

AudioDriverLibGodot::AudioDriverLibGodot() {
	//tmp_buffer.clear();
}

#endif // LIBRARY_ENABLED
