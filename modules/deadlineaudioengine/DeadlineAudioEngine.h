/*
  ==============================================================================

    JuceLib.h
    Created: 15 Mar 2023 11:38:14pm
    Author:  RedneckJack

  ==============================================================================
*/

#pragma once
#ifndef DEADLINEAUDIOENGINE_H
#define DEADLINEAUDIOENGINE_H

#include "core/object/ref_counted.h"


class DeadlineAudioEngine : public RefCounted {
  GDCLASS(DeadlineAudioEngine, RefCounted);

protected:
  static void _bind_methods();

public:
	DeadlineAudioEngine();

	void initProcessor();

};

#endif
