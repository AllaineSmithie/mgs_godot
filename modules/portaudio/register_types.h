
#ifndef PORTAUDIO_REGISTER_TYPES_H
#define PORTAUDIO_REGISTER_TYPES_H

#ifdef PORT_AUDIO
#include "modules/register_module_types.h"

void initialize_portaudio_module(ModuleInitializationLevel p_level);
void uninitialize_portaudio_module(ModuleInitializationLevel p_level);

#endif // PORT_AUDIO

#endif
