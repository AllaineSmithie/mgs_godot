
/**************************************************************************/
/*  libgodot_midi_driver.cpp                                              */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             Metro Gaya System                          */
/*                        https://deadline-entertainment.com              */
/**************************************************************************/
/* Copyright (c) 2022-present Deadline Entertainment Gbr Germany.         */
/**************************************************************************/

#ifdef LIBRARY_ENABLED

#include "libgodot_midi_driver.h"
#include "core/string/print_string.h"

Error LibGodotMidiDriver::open() {
	return OK;
}

PackedStringArray LibGodotMidiDriver::get_connected_inputs() {
	PackedStringArray list;
	list.push_back("External");	
	return list;
}

void LibGodotMidiDriver::close() {
}

LibGodotMidiDriver::LibGodotMidiDriver() {
}

LibGodotMidiDriver::~LibGodotMidiDriver() {
	close();
}

#endif // LIBRARY_ENABLED
