#include "register_types.h"

#include "core/object/class_db.h"
//#include "core/object/class_db.h"
#include "ShowEngine.h"

#ifndef _SHOWENGINE_DISABLED

#ifdef TOOLS_ENABLED

// if editor
#include "editor/ShowEditor.h"
#include "editor/export/editor_export.h"

	class EditorExportShow : public EditorExportPlugin
	{
		GDCLASS(EditorExportShow, EditorExportPlugin);

	public:
		virtual void _export_file(const String& p_path, const String& p_type, const HashSet<String>& p_features) override
		{
			String script_key;

			const Ref<EditorExportPreset>& preset = get_export_preset();

			if (preset.is_valid())
			{
				script_key = preset->get_script_encryption_key().to_lower();
			}

			if (!p_path.ends_with(".mdat"))
			{
				return;
			}

			return;
		}

		virtual String _get_name() const override { return "Metro Gaya Show File"; }
	};

	static ShowEditor* _show_editor = nullptr;

	static void _show_editor_init()
	{
		Ref<EditorExportShow> show_export;
		show_export.instantiate();
		EditorExport::get_singleton()->add_export_plugin(show_export);
		EditorNode::get_singleton()->add_editor_plugin(memnew(ShowEditorPlugin));
	}

#endif // TOOLS_ENABLED


#endif // _SHOWENGINE_DISABLED

ShowEngine* _show_engine= nullptr;
void initialize_showengine_module(ModuleInitializationLevel p_level)
{
#ifndef _SHOWENGINE_DISABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		_show_engine = memnew(ShowEngine);
		GDREGISTER_CLASS(ShowEngine);
		Engine::get_singleton()->add_singleton(Engine::Singleton("ShowEngine", ShowEngine::get_singleton()));
		_show_engine->init();

	}
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		EditorNode::add_init_callback(_show_editor_init);
	}
#endif // TOOLS_ENABLED

	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		GDREGISTER_CLASS(NodeShowSlot);
		GDREGISTER_CLASS(ShowNode);
		GDREGISTER_CLASS(ShowInterface);
		GDREGISTER_CLASS(AudioShowInterface);
		GDREGISTER_CLASS(MidiShowInterface);
		GDREGISTER_CLASS(InputEventShowInterface);
		GDREGISTER_CLASS(DmxUsbShowInterface);
		GDREGISTER_CLASS(DmxIPShowInterface);
		GDREGISTER_CLASS(OSCShowInterface);
		GDREGISTER_CLASS(RemoteShowInterface);
		GDREGISTER_CLASS(MetronomeShowInterface);
		GDREGISTER_CLASS(SmartlightShowInterface);
		GDREGISTER_CLASS(CameraShowInterface);
		GDREGISTER_CLASS(ScreenShowInterface);
		GDREGISTER_CLASS(ArduinoShowInterface);
		GDREGISTER_CLASS(RaspberryShowInterface);
		GDREGISTER_CLASS(ModbusShowInterface);

	}
	/*if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		// Register classes
		GDREGISTER_CLASS(ShowShowEngine);
	}*/
#endif // _SHOWENGINE_DISABLED
}

void uninitialize_showengine_module(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SERVERS) {
		return;
	}
#ifndef _SHOWENGINE_DISABLED
	if (_show_engine)
	{
		Engine::get_singleton()->remove_singleton("ShowEngine");
		memdelete(_show_engine);
	}
#endif // __SHOWENGINE_DISABLED
}
