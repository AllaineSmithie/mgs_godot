
#include "register_types.h"
#include "core/object/class_db.h"
#include <core/core_bind.h>
#include "core/config/project_settings.h"
#include "core/config/engine.h"
#include "DeadlineAudioEngine.h"

#include "audio_formats/audio_stream_pro.h"

#ifdef TOOLS_ENABLED
#include "core/config/engine.h"
#include "audio_formats/resource_importer_pro.h"
#endif

static DeadlineAudioEngine* _deadline_audio_engine = nullptr;

void initialize_deadline_audio_engine_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		// Register classes
		_deadline_audio_engine = memnew(DeadlineAudioEngine);
		GDREGISTER_CLASS(DeadlineAudioEngine);		
		GDREGISTER_CLASS(Synth);		
		GDREGISTER_CLASS(MicPosition);
		GDREGISTER_CLASS(SFZResource);
		GDREGISTER_CLASS(SFZRegion);
		GDREGISTER_CLASS(Instrument);
		GDREGISTER_CLASS(ResourceFormatSaverSFZ);
		GDREGISTER_CLASS(ResourceFormatLoaderSFZ);
		GDREGISTER_CLASS(InstrumentNode);

		Engine::get_singleton()->add_singleton(Engine::Singleton("DeadlineAudioEngine", DeadlineAudioEngine::get_singleton()));
	}

	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		return;
	}

	DeadlineAudioEngine::get_singleton()->init_audio_server();

#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint()) {
		Ref<ResourceImporterPRO> pro_audio_import;
		pro_audio_import.instantiate();
		ResourceFormatImporter::get_singleton()->add_importer(pro_audio_import);
	}
#endif
	GDREGISTER_CLASS(AudioStreamPRO);
}

void uninitialize_deadline_audio_engine_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		Engine::get_singleton()->remove_singleton("DeadlineAudioEngine");
		memdelete(_deadline_audio_engine);
	}
}
