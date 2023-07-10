/**************************************************************************/
/*  image_frames_loader.h                                                 */
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
#ifndef IMAGE_FRAMES_LOADER_H
#define IMAGE_FRAMES_LOADER_H

#include "image_frames.h"

#include <core/io/resource_loader.h>
#include <core/io/file_access.h>

class ImageFramesFormatLoader {
	friend class ImageFramesLoader;
	friend class ResourceFormatLoaderImageFrames;
public:
	virtual Error load_image_frames(Ref<ImageFrames> &r_image_frames, const Ref<FileAccess> &p_f, int p_max_frames = 0) const = 0;
	virtual void get_recognized_extensions(List<String> *p_extensions) const = 0;
	bool recognize(String p_extension);

	virtual ~ImageFramesFormatLoader() {}
};

class ImageFramesLoader {
	static Vector<ImageFramesFormatLoader *> loader;
	friend class ResourceFormatLoaderImageFrames;
public:
	static Error load_image_frames(String p_file, Ref<ImageFrames> p_image, const Ref<FileAccess>& p_custom = Ref<FileAccess>(), int p_max_frames = 0);
	static void get_recognized_extensions(List<String> *p_extensions);
	static ImageFramesFormatLoader *recognize(const String &p_extension);

	static void add_image_frames_format_loader(ImageFramesFormatLoader *p_loader);
	static void remove_image_frames_format_loader(ImageFramesFormatLoader *p_loader);

	static void cleanup();
};

class ResourceFormatLoaderImageFrames : public ResourceFormatLoader {
public:
	virtual Ref<Resource> load(const String& p_path, const String& p_original_path = "", Error* r_error = nullptr, bool p_use_sub_threads = false, float* r_progress = nullptr, CacheMode p_cache_mode = CACHE_MODE_REUSE);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

class ResourceFormatLoaderAnimatedTexture : public ResourceFormatLoader {
public:
	virtual Ref<Resource> load(const String& p_path, const String& p_original_path = "", Error* r_error = nullptr, bool p_use_sub_threads = false, float* r_progress = nullptr, CacheMode p_cache_mode = CACHE_MODE_REUSE);
	//virtual Ref<Resource> load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_no_subresource_cache = false);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

class ResourceFormatLoaderSpriteFrames : public ResourceFormatLoader {
public:
	virtual Ref<Resource> load(const String& p_path, const String& p_original_path = "", Error* r_error = nullptr, bool p_use_sub_threads = false, float* r_progress = nullptr, CacheMode p_cache_mode = CACHE_MODE_REUSE);
	//virtual Ref<Resource> load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_no_subresource_cache = false);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

#endif // IMAGE_FRAMES_LOADER_H
