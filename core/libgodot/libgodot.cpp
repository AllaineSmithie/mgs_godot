/**************************************************************************/
/*  libgodot.cpp                                                          */
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

#if defined(LIBRARY_ENABLED)
#include "libgodot.h"
#include "libgodot_callable.h"
#include "core/config/project_settings.h"
#include "core/extension/gdextension_manager.h"
#include "core/os/midi_driver.h"
#include "core/variant/callable.h"
//#include "modules/deadline_audio_engine/juce/juce_MidiBuffer.h"

uint32_t (*lib_godot_callable_hash)(void *);
void *(*lib_godot_get_as_text)(void *);
uint64_t (*lib_godot_get_object)(void *);
void (*lib_godot_disposes)(void *);
void (*lib_godot_call)(void *, const void *, int, void *, void *);
void (*create_process_function_ptr)(const char*, int, const char*, int);


#ifdef __cplusplus
extern "C" {
#endif

GDExtensionBool (*initialization_function)(const GDExtensionInterfaceGetProcAddress, GDExtensionClassLibraryPtr, GDExtensionInitialization *);
void (*scene_load_function)(void *);
void (*project_settings_load_function)(void *);

/*LIBGODOT_API void register_create_process_cb(void (*p_function_ptr)(const char*, int, const char*, int))
{
	create_process_function_ptr = p_function_ptr;
}*/
LIBGODOT_API void libgodot_bind_custom_callable(uint32_t (*callable_hash_bind)(void *), void *(*get_as_text_bind)(void *), uint64_t (*get_object_bind)(void *), void (*disposes_bind)(void *), void (*call_bind)(void *, const void *, int, void *, void *)) {
	lib_godot_callable_hash = callable_hash_bind;
	lib_godot_get_as_text = get_as_text_bind;
	lib_godot_get_object = get_object_bind;
	lib_godot_disposes = disposes_bind;
	lib_godot_call = call_bind;
}

LIBGODOT_API void libgodot_bind(GDExtensionBool (*initialization_bind)(const GDExtensionInterfaceGetProcAddress, GDExtensionClassLibraryPtr, GDExtensionInitialization *), void (*scene_function_bind)(void *), void (*project_settings_function_bind)(void *)) {
	initialization_function = initialization_bind;
	scene_load_function = scene_function_bind;
	project_settings_load_function = project_settings_function_bind;
}

LIBGODOT_API void *libgodot_create_callable(void *targetObject) {
	return new Callable(new LibGodotCallable(targetObject));
}

void libgodot_project_settings_load(void *project_settings) {
	if (project_settings_load_function != nullptr) {
		project_settings_load_function(project_settings);
	}
}

void libgodot_scene_load(void *scene) {
	if (scene_load_function != nullptr) {
		scene_load_function(scene);
	}
}

LIBGODOT_API void godot_initialize_audio(int num_channels, int samplesperblock, float samplerate) {
	// get plugin host data
	GLOBAL_DEF_INTERNAL("audio/driver/enable_input", false);
	GLOBAL_DEF_INTERNAL("audio/driver/mix_rate", samplerate);
	GLOBAL_DEF_INTERNAL("audio/driver/num_channels", num_channels);
	GLOBAL_DEF_INTERNAL("audio/driver/samples_per_block", samplesperblock);
	GLOBAL_DEF_INTERNAL("audio/driver/mix_rate.web", 0); // Safer default output_latency for web (use browser default).
	GLOBAL_DEF_INTERNAL("audio/driver/output_latency", (float)samplesperblock / samplerate * 1000.0);
	GLOBAL_DEF_INTERNAL("audio/driver/output_latency.web", (float)samplesperblock / samplerate * 1000.0); // Safer default output_latency for web.
}

LIBGODOT_API void godot_process_audio(int numchannels, int samplesperblock, float* const* buffer) {
	// assumed it's always the first one
	AudioDriver* driver = AudioDriverManager::get_driver(0);
	if (auto lib_godot_driver = static_cast<AudioDriverLibGodot*>(driver)) {
		lib_godot_driver->process_samples(numchannels, samplesperblock, buffer);
	}
}

void godot_process_midi(const unsigned char* p_buffer, int buffer_size)
{
	/*juce::MidiBuffer buffer(p_buffer, buffer_size);
	for (auto& e : buffer) {
		MIDIDriver::get_singleton()->receive_input_packet((uint64_t)e.getMessage().getTimeStamp(), (uint8_t*)&e.data, 3);
	}*/
}

bool libgodot_is_scene_loadable() {
	return scene_load_function != nullptr;
}
/*
void call_create_process_callback(const char* path, int path_size, const char* args, int args_size) {
	if (create_process_function_ptr)
		create_process_function_ptr(path, path_size, args, args_size);
}*/

void libgodot_init_resource() {
	if (initialization_function != nullptr) {
		Ref<GDExtension> libgodot;
		libgodot.instantiate();
		Error err = libgodot->initialize_extension_function(initialization_function, "LibGodot");
		if (err != OK) {
			ERR_PRINT("LibGodot Had an error initialize_extension_function'");
		} else {
			print_verbose("LibGodot initialization");
			libgodot->set_path("res://LibGodotGDExtension");
			GDExtensionManager::get_singleton()->load_extension("res://LibGodotGDExtension");
		}
	}
}

#ifdef __cplusplus
}
#endif

bool LibGodotCallable::_equal_func(const CallableCustom *p_a, const CallableCustom *p_b) {
	const LibGodotCallable *a = static_cast<const LibGodotCallable *>(p_a);
	const LibGodotCallable *b = static_cast<const LibGodotCallable *>(p_b);

	return (a->customObject == b->customObject);
}

bool LibGodotCallable::_less_func(const CallableCustom *p_a, const CallableCustom *p_b) {
	const LibGodotCallable *a = static_cast<const LibGodotCallable *>(p_a);
	const LibGodotCallable *b = static_cast<const LibGodotCallable *>(p_b);

	return (a->customObject == b->customObject);
}

uint32_t LibGodotCallable::hash() const {
	return lib_godot_callable_hash(customObject);
}

String LibGodotCallable::get_as_text() const {
	return *(String *)lib_godot_get_as_text(customObject);
}

ObjectID LibGodotCallable::get_object() const {
	return (ObjectID)lib_godot_get_object(customObject);
}

LibGodotCallable::LibGodotCallable(void *target) {
	customObject = target;
}

LibGodotCallable::~LibGodotCallable() {
	lib_godot_disposes(customObject);
}

CallableCustom::CompareEqualFunc LibGodotCallable::get_compare_equal_func() const {
	return _equal_func;
}

CallableCustom::CompareLessFunc LibGodotCallable::get_compare_less_func() const {
	return _less_func;
}

void LibGodotCallable::call(const Variant **p_arguments, int p_argcount, Variant &r_return_value, Callable::CallError &r_call_error) const {
	Callable::CallError callar = {};
	Variant return_value = {};
	lib_godot_call(customObject, static_cast<const void *>(p_arguments), p_argcount, (void *)&return_value, (void *)&callar);
	r_return_value = return_value;
	r_call_error = callar;
}

#endif
