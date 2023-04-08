#include "register_types.h"

#include "core/object/class_db.h"
//#include "core/object/class_db.h"
#include "DeadlineAudioEngine.h"

void initialize_deadlineaudioengine_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		// Register classes
		GDREGISTER_CLASS(DeadlineAudioEngine);
	}
}

void uninitialize_deadlineaudioengine_module(ModuleInitializationLevel p_level)
{
}
