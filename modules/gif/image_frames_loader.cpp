/**************************************************************************/
/*  image_frames_loader.cpp                                                 */
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

#include "image_frames_loader.h"
#include "scene/2d/animated_sprite_2d.h"
#include "scene/resources/texture.h"

Vector<ImageFramesFormatLoader *> ImageFramesLoader::loader;

bool ImageFramesFormatLoader::recognize(String p_extension) {
	List<String> extensions;
	get_recognized_extensions(&extensions);
	for (List<String>::Element *E = extensions.front(); E; E = E->next()) {
		if (E->get().nocasecmp_to(p_extension) == 0) {
			return true;
		}
	}
	return false;
}

Error ImageFramesLoader::load_image_frames(String p_file, Ref<ImageFrames> p_image_frames, const Ref<FileAccess> &p_custom, int p_max_frames) {
	ERR_FAIL_COND_V_MSG(p_image_frames.is_null(), ERR_INVALID_PARAMETER, "It's not a reference to a valid ImageFrames object.");

	Ref<FileAccess> f = p_custom;
	if (!f.is_valid()) {
		Error err;
		f = FileAccess::open(p_file, FileAccess::READ, &err);
		if (!f.is_valid()) {
			ERR_PRINT("Error opening file '" + p_file + "'.");
			return err;
		}
	}
	String extension = p_file.get_extension();

	for (int i = 0; i < loader.size(); i++) {
		if (!loader[i]->recognize(extension)) {
			continue;
		}
		Error err = loader[i]->load_image_frames(p_image_frames, f);
		if (err != OK) {
			ERR_PRINT("Error loading image: " + p_file);
		}
		if (p_image_frames->get_frame_count() == 0) {
			ERR_PRINT("Images frames should contain at least one frame to be loaded.");
			if (!p_custom.is_valid()) {
				//memdelete(f);
				f.unref();
			}
			return ERR_INVALID_DATA;
		}
		if (err != ERR_FILE_UNRECOGNIZED) {
			if (!p_custom.is_valid()) {
				//memdelete(f);
				f.unref();
			}
			return err;
		}
	}
	if (!p_custom.is_valid()) {
		//memdelete(f);
		f.unref();
	}
	return ERR_FILE_UNRECOGNIZED;
}

void ImageFramesLoader::get_recognized_extensions(List<String> *p_extensions) {
	for (int i = 0; i < loader.size(); i++) {
		loader[i]->get_recognized_extensions(p_extensions);
	}
}

ImageFramesFormatLoader *ImageFramesLoader::recognize(const String &p_extension) {
	for (int i = 0; i < loader.size(); i++) {
		if (loader[i]->recognize(p_extension)) {
			return loader[i];
		}
	}
	return nullptr;
}

void ImageFramesLoader::add_image_frames_format_loader(ImageFramesFormatLoader *p_loader) {
	loader.push_back(p_loader);
}

void ImageFramesLoader::remove_image_frames_format_loader(ImageFramesFormatLoader *p_loader) {
	loader.erase(p_loader);
}

void ImageFramesLoader::cleanup() {
	while (loader.size()) {
		remove_image_frames_format_loader(loader[0]);
	}
}

// ImageFrames

Ref<Resource> ResourceFormatLoaderImageFrames::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ);
	if (!f.is_valid()) {
		if (r_error) {
			*r_error = ERR_CANT_OPEN;
		}
		return Ref<Resource>();
	}
	uint8_t header[5] = { 0, 0, 0, 0, 0 };
	f->get_buffer(header, 5);

	bool unrecognized = header[0] != 'G' || header[1] != 'D' || header[2] != 'I' || header[3] != 'M' || header[4] != 'F';
	if (unrecognized) {
		//memdelete(f);
		if (r_error) {
			*r_error = ERR_FILE_UNRECOGNIZED;
		}
		ERR_FAIL_V(Ref<Resource>());
	}
	String extension = f->get_pascal_string();
	int idx = -1;

	for (int i = 0; i < ImageFramesLoader::loader.size(); i++) {
		if (ImageFramesLoader::loader[i]->recognize(extension)) {
			idx = i;
			break;
		}
	}
	if (idx == -1) {
		//memdelete(f);
		if (r_error) {
			*r_error = ERR_FILE_UNRECOGNIZED;
		}
		ERR_FAIL_V(Ref<Resource>());
	}
	Ref<ImageFrames> image_frames;
	image_frames.instantiate();

	Error err = ImageFramesLoader::loader[idx]->load_image_frames(image_frames, f);
	//memdelete(f);

	if (err != OK) {
		if (r_error) {
			*r_error = err;
		}
		return Ref<Resource>();
	}
	if (r_error) {
		*r_error = OK;
	}
	return image_frames;
}

void ResourceFormatLoaderImageFrames::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("imageframes");
}

bool ResourceFormatLoaderImageFrames::handles_type(const String &p_type) const {
	return p_type == "ImageFrames";
}

String ResourceFormatLoaderImageFrames::get_resource_type(const String &p_path) const {
	return p_path.get_extension().to_lower() == "imageframes" ? "ImageFrames" : String();
}

// AnimatedTexture

Ref<Resource> ResourceFormatLoaderImageFrames::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ);
	if (!f.is_valid()) {
		if (r_error) {
			*r_error = ERR_CANT_OPEN;
		}
		return Ref<Resource>();
	}
	uint8_t header[4] = { 0, 0, 0, 0 };
	f->get_buffer(header, 4);

	bool unrecognized = header[0] != 'G' || header[1] != 'D' || header[2] != 'A' || header[3] != 'T';
	if (unrecognized) {
		//memdelete(f);
		if (r_error) {
			*r_error = ERR_FILE_UNRECOGNIZED;
		}
		ERR_FAIL_V(Ref<Resource>());
	}
	Ref<AnimatedTexture> atex;
	atex.instantiate();

	//uint32_t tex_flags = f->get_32();
	// flush unused bytes
	f->get_32();
	uint32_t frame_count = f->get_32();

	uint32_t width = f->get_32();
	uint32_t height = f->get_32();

	for (size_t i = 0; i < frame_count; ++i) {
		// Frame image data.
		PackedByteArray data;
		uint32_t len = f->get_32();
		data.resize(len);
		//PackedByteArray::Write w = data.write();
		f->get_buffer(data.ptrw(), len);

		Ref<Image> image = Image::create_from_data(width, height, false, Image::FORMAT_RGBA8, data);
		Ref<ImageTexture> frame = ImageTexture::create_from_image(image);
		atex->set_frame_texture(i, frame);

		// Frame delay data.
		atex->set_frame_duration(i, f->get_real());
	}
	atex->set_frames(frame_count);

	f->close();
	//memdelete(f);

	return atex;
}

void ResourceFormatLoaderAnimatedTexture::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("atex");
}

bool ResourceFormatLoaderAnimatedTexture::handles_type(const String &p_type) const {
	return p_type == "AnimatedTexture";
}

String ResourceFormatLoaderAnimatedTexture::get_resource_type(const String &p_path) const {
	return p_path.get_extension().to_lower() == "atex" ? "AnimatedTexture" : String();
}

// SpriteFrames

Ref<Resource> ResourceFormatLoaderImageFrames::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ);
	if (!f.is_valid()) {
		if (r_error) {
			*r_error = ERR_CANT_OPEN;
		}
		return Ref<Resource>();
	}
	uint8_t header[4] = { 0, 0, 0, 0 };
	f->get_buffer(header, 4);

	bool unrecognized = header[0] != 'G' || header[1] != 'D' || header[2] != 'S' || header[3] != 'F';
	if (unrecognized) {
		//memdelete(f);
		if (r_error) {
			*r_error = ERR_FILE_UNRECOGNIZED;
		}
		ERR_FAIL_V(Ref<Resource>());
	}
	Ref<SpriteFrames> sframes;
	sframes.instantiate();

	//uint32_t tex_flags = f->get_32();
	// flush unused bytes
	f->get_32();
	uint32_t frame_count = f->get_32();

	uint32_t width = f->get_32();
	uint32_t height = f->get_32();

	for (size_t i = 0; i < frame_count; ++i) {
		// Frame image data.
		PackedByteArray data;
		uint32_t len = f->get_32();
		data.resize(len);
		//PackedByteArray::Write w = data.write();
		f->get_buffer(data.ptrw(), len);

		Ref<Image> image = Image::create_from_data(width, height, false, Image::FORMAT_RGBA8, data);
		Ref<ImageTexture> frame = ImageTexture::create_from_image(image);
		sframes->add_frame("default", frame);
	}
	sframes->set_animation_speed("default", f->get_real());

	f->close();
	//memdelete(f);

	return sframes;
}

void ResourceFormatLoaderSpriteFrames::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("sframes");
}

bool ResourceFormatLoaderSpriteFrames::handles_type(const String &p_type) const {
	return p_type == "SpriteFrames";
}

String ResourceFormatLoaderSpriteFrames::get_resource_type(const String &p_path) const {
	return p_path.get_extension().to_lower() == "sframes" ? "SpriteFrames" : String();
}
