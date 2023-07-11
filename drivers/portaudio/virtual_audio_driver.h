
#ifndef VIRTUAL_AUDIO_DRIVER_H
#define VIRTUAL_AUDIO_DRIVER_H

#include "core/object/ref_counted.h"
#include "core/os/semaphore.h"
#include "core/templates/vector.h"
#include <servers/audio_server.h>

class VirtualAudioDriver : public AudioDriver {
public:
	//virtual int process_samples(const Ref<RefCounted>& data) = 0;
	virtual void trigger_process_samples() = 0;
	virtual void stop() = 0;
	virtual void set_stream_resolution(const int p_bytes_per_sample) = 0;
	virtual void frames_per_buffer_changed() = 0;
	VirtualAudioDriver() {}
	virtual ~VirtualAudioDriver() {}
	//Semaphore semaphore;
};

#endif // VIRTUAL_AUDIO_DRIVER_H
