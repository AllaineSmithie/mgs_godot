#include "resource_importer_pro.h"

#include "core/io/file_access.h"
#include "core/io/resource_saver.h"
#include "scene/resources/texture.h"

#ifdef TOOLS_ENABLED
#include "editor/import/audio_stream_import_settings.h"
#endif

String ResourceImporterPRO::get_importer_name() const {
	return "flac";
}

String ResourceImporterPRO::get_visible_name() const {
	return "PRO";
}

void ResourceImporterPRO::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("flac");
	p_extensions->push_back("wavpack");
	p_extensions->push_back("lame");
}

String ResourceImporterPRO::get_save_extension() const {
	return "proaudiostr";
}

String ResourceImporterPRO::get_resource_type() const {
	return "AudioStreamPRO";
}

bool ResourceImporterPRO::get_option_visibility(const String &p_path, const String &p_option, const HashMap<StringName, Variant> &p_options) const {
	return true;
}

int ResourceImporterPRO::get_preset_count() const {
	return 0;
}

String ResourceImporterPRO::get_preset_name(int p_idx) const {
	return String();
}

void ResourceImporterPRO::get_import_options(const String &p_path, List<ImportOption> *r_options, int p_preset) const {
	r_options->push_back(ImportOption(PropertyInfo(Variant::BOOL, "loop"), false));
	r_options->push_back(ImportOption(PropertyInfo(Variant::FLOAT, "loop_offset"), 0));
	r_options->push_back(ImportOption(PropertyInfo(Variant::FLOAT, "bpm", PROPERTY_HINT_RANGE, "0,400,0.01,or_greater"), 0));
	r_options->push_back(ImportOption(PropertyInfo(Variant::INT, "beat_count", PROPERTY_HINT_RANGE, "0,512,or_greater"), 0));
	r_options->push_back(ImportOption(PropertyInfo(Variant::INT, "bar_beats", PROPERTY_HINT_RANGE, "2,32,or_greater"), 4));
}

#ifdef TOOLS_ENABLED
bool ResourceImporterPRO::has_advanced_options() const {
	return true;
}
void ResourceImporterPRO::show_advanced_options(const String &p_path) {
	Ref<AudioStreamPRO> pro_audio_stream = import_pro_audio_file(p_path);
	if (pro_audio_stream.is_valid()) {
		AudioStreamImportSettings::get_singleton()->edit(p_path, "flac", pro_audio_stream);
		
	}
}
#endif

Ref<AudioStreamPRO> ResourceImporterPRO::import_pro_audio_file(const String &p_path) {
	/*Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ);
	ERR_FAIL_COND_V(f.is_null(), Ref<AudioStreamPRO>());

	uint64_t len = f->get_length();

	Vector<uint8_t> data;
	data.resize(len);
	uint8_t *w = data.ptrw();

	f->get_buffer(w, len);

	*/
	Ref<AudioStreamPRO> pro_audio_stream;
	pro_audio_stream.instantiate();

	pro_audio_stream->set_file_path(p_path);

	//ERR_FAIL_COND_V(!pro_audio_stream->get_data().size(), Ref<AudioStreamPRO>());
	return pro_audio_stream;
}

Error ResourceImporterPRO::import(const String &p_source_file, const String &p_save_path, const HashMap<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files, Variant *r_metadata) {
	bool loop = p_options["loop"];
	float loop_offset = p_options["loop_offset"];
	double bpm = p_options["bpm"];
	float beat_count = p_options["beat_count"];
	float bar_beats = p_options["bar_beats"];

	Ref<AudioStreamPRO> pro_audio_stream = import_pro_audio_file(p_source_file);
	if (pro_audio_stream.is_null()) {
		return ERR_CANT_OPEN;
	}
	pro_audio_stream->set_loop(loop);
	pro_audio_stream->set_loop_offset(loop_offset);
	pro_audio_stream->set_bpm(bpm);
	pro_audio_stream->set_beat_count(beat_count);
	pro_audio_stream->set_bar_beats(bar_beats);

	return ResourceSaver::save(pro_audio_stream, p_save_path + ".proaudiostr");
}

ResourceImporterPRO::ResourceImporterPRO() {
}
