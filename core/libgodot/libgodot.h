/**************************************************************************/
/*  libgodot.h                                                            */
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

#ifndef LIBGODOT_H
#define LIBGODOT_H

#if defined(LIBRARY_ENABLED)

#if defined(WINDOWS_ENABLED) | defined(UWP_ENABLED)
#define LIBGODOT_API __declspec(dllexport)
#elif defined(ANDROID_ENABLED)
#include <jni.h>
#define LIBGODOT_API JNIEXPORT
#else
#define LIBGODOT_API
#endif
#include "libgodot_audio_driver.h"
#include "core/extension/gdextension_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

// const String &p_path, const List<String> &p_arguments, ProcessID *r_child_id, bool p_open_console
void libgodot_init_resource();

void libgodot_project_settings_load(void *project_settings);

void libgodot_scene_load(void *scene);

bool libgodot_is_scene_loadable();

void call_create_process_callback(const char* path, int path_size, const char* args, int args_size);

LIBGODOT_API void *libgodot_create_callable(void *customobject);

LIBGODOT_API void libgodot_bind_custom_callable(uint32_t (*callable_hash_bind)(void *), void *(*get_as_text_bind)(void *), uint64_t (*get_object_bind)(void *), void (*disposes_bind)(void *), void (*call_bind)(void *, const void *, int, void *, void *));

LIBGODOT_API void libgodot_bind(GDExtensionBool (*initialization_bind)(const GDExtensionInterfaceGetProcAddress, GDExtensionClassLibraryPtr, GDExtensionInitialization *), void (*scene_function_bind)(void *), void (*project_settings_function_bind)(void *));

LIBGODOT_API int godot_main(int argc, char *argv[]);


LIBGODOT_API int godot_main_setup(int argc, char* argv_utf8[], void* handle, bool secondphase);

LIBGODOT_API void godot_initialize_audio(int num_channels, int samplesperblock, float samplerate);

LIBGODOT_API void godot_process_audio(int numchannels, int samplesperblock, float* const* buffer);

LIBGODOT_API void godot_process_midi(const unsigned char* buffer, int buffer_size);

LIBGODOT_API int godot_main_setup2();

LIBGODOT_API void godot_init_windows();

LIBGODOT_API void godot_deinit_windows();

LIBGODOT_API int godot_my_very_first_hello_world(int argc);

LIBGODOT_API void* godot_get_native_window();

LIBGODOT_API void godot_main_run_pre();

LIBGODOT_API void godot_main_run_iteration();

LIBGODOT_API int godot_main_finalize();

#ifdef __cplusplus
}
#endif

#endif

#endif // LIBGODOT_H
