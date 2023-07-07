/**************************************************************************/
/*  libgodot_midi_driver.h                                                */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             Metro Gaya System                          */
/*                        https://deadline-entertainment.com              */
/**************************************************************************/
/* Copyright (c) 2022-present Deadline Entertainment Gbr Germany.         */
/**************************************************************************/

#ifndef LIBGODOT_MIDI_DRIVER_H
#define LIBGODOT_MIDI_DRIVER_H

#ifdef LIBRARY_ENABLED

#include "core/os/midi_driver.h"
#include "core/templates/vector.h"


class LibGodotMidiDriver : public MIDIDriver {
public:
	virtual Error open();
	virtual void close();

	virtual PackedStringArray get_connected_inputs();

	LibGodotMidiDriver();
	virtual ~LibGodotMidiDriver();
};

#endif // LIBRARY_ENABLED

#endif // LIBGODOT_MIDI_DRIVER_H
