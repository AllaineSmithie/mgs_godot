
#ifndef SFZ_PARSER_H
#define SFZ_PARSER_H

#include "core/io/resource_loader.h"
#include "core/string/ustring.h"
#include "core/templates/vector.h"
#include <core/io/file_access.h>

class SFZParser
{
	enum SFZResourceType
	{
		Region,
		Group,
		Master,
		Control,
		Global
	};

	Dictionary master_opcodes;
	Dictionary global_opcodes;
	Dictionary control_opcodes;
	Dictionary definitions;
	Array group_opcodes;
public:
	SFZParser();

	Dictionary init_with_file(const String p_sfz_file);
	void init_with_string(const String p_sfz_content_string);

	Dictionary get_master_opcodes()		{ return master_opcodes; }
	Dictionary get_global_opcodes()		{ return global_opcodes; }
	Dictionary get_control_opcodes()	{ return control_opcodes; }
	Dictionary get_definitions()		{ return definitions; }
};

#endif // SFZ_PARSER_H
