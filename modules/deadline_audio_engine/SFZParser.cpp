#include "SFZParser.h"
#include "core/io/resource.h"

SFZParser::SFZParser()
{

}

inline Dictionary _parse_definition(const String& p_line)
{
	String definition = p_line.replace("#define", "").strip_edges(true, true);
	if (definition.contains("="))
	{
		PackedStringArray result = definition.split("=", false, 2);
		if (result.size() < 1)
		{
			Dictionary r;
			r[result[0]] = "";
			return r;
		}
	}

	if (definition.contains(" "))
	{
		PackedStringArray result = definition.split(" ", false, 2);

		Dictionary r;
		r[result[0]] = result.size() > 1 ? result[1].replace("\"", "") : "";
		return r;
	}

	Dictionary r;
	r[definition.strip_edges(true, true).strip_escapes()] = "";
	return r;
}

Dictionary SFZParser::init_with_file(const String p_sfz_file)
{
	group_opcodes.clear();
	master_opcodes.clear();
	control_opcodes.clear();
	global_opcodes.clear();
	definitions.clear();
	
	Variant group_1;
	bool rvalid = true;
	group_1.set_keyed("Regions", Array(), rvalid);
	group_opcodes.append(group_1);
	bool first_group = true;

	Error error;
	Ref<FileAccess> file = FileAccess::open(p_sfz_file, FileAccess::READ, &error);
	if (error != OK) {
		if (file.is_valid()) {
			file->close();
		}
		return Dictionary();
	}

	String sfz_file_content = String("");
	SFZResourceType current_header = SFZResourceType::Group;
	//SFZResourceType previous_header = SFZResourceType::Group;;
	int i = 0;
	while (!file->eof_reached())
	{
		String line = file->get_line().strip_edges(true, false);
		if (line.length() < 1)
			continue;

		char32_t first_char = line.get(0);
		if (first_char == '#')
		{
			definitions.merge(_parse_definition(line));
		}
		else
		{			
			if (first_char == '<')
			{
				const PackedStringArray opcode_array = line.split(">", false, 2);
				const String header = opcode_array[0].replace("<", "");

				if (header == "control")
				{
					current_header = SFZResourceType::Control;
				}
				else if (header == "global")
				{
					current_header = SFZResourceType::Global;
				}
				else if (header == "master")
				{
					current_header = SFZResourceType::Master;
				}
				else if (header == "group")
				{
					if (first_group)
						first_group = false;
					else
					{
						Variant group;
						group.set_keyed("Regions", Array(), rvalid);
						group_opcodes.append(group);
					}
					current_header = SFZResourceType::Group;
				}
				else if (header == "region")
				{
					Array regions = group_opcodes[group_opcodes.size() - 1].get_keyed("Regions", rvalid);
					if (!rvalid)
					{
						group_opcodes[group_opcodes.size() - 1].set_keyed("Regions", Array(), rvalid);
						regions = group_opcodes[group_opcodes.size() - 1].get_keyed("Regions", rvalid);
					}
					regions.append(Variant());
					group_opcodes[group_opcodes.size() - 1].set_keyed("Regions", regions, rvalid);
					current_header = SFZResourceType::Region;
				}

				line = opcode_array[1].strip_edges(true, true);
			}

			PackedStringArray equal_split = line.split("=", "");
			PackedStringArray residual = { equal_split[0] };
			for (i = 1; i < equal_split.size(); ++i)
			{
				residual.append_array(equal_split[i].rsplit(" ", false, 2));
			}
			Dictionary opcodes;
			for (i = 0; i < residual.size(); i += 2)
			{
				opcodes[residual[i]] = residual[i + 1];
			}

			switch (current_header)
			{
			case Region:
				{
					Array regions = group_opcodes[group_opcodes.size() - 1].get_keyed("Regions", rvalid);
					if (!rvalid)
					{
						group_opcodes[group_opcodes.size() - 1].set_keyed("Regions", Array(), rvalid);
						regions = group_opcodes[group_opcodes.size() - 1].get_keyed("Regions", rvalid);
					}
					regions.append(Variant());
					group_opcodes[group_opcodes.size() - 1].set_keyed("Regions", regions, rvalid);
				}
				break;
			case Group:
				{
					Dictionary last_group = group_opcodes[group_opcodes.size() - 1];
					last_group.merge(opcodes);
					group_opcodes[group_opcodes.size() - 1] = last_group;
				}
				break;
			case Master:
				master_opcodes.merge(opcodes);
				break;
			case Control:
				control_opcodes.merge(opcodes);
				break;
			case Global:
				global_opcodes.merge(opcodes);
				break;
			default:
				break;
			}
		}
	}

	file->close();


	Dictionary out;
	out["Global"] = global_opcodes;
	out["Master"] = master_opcodes;
	out["Control"] = control_opcodes;
	out["Groups"] = group_opcodes;
	return out;

}

void SFZParser::init_with_string(const String p_sfz_content_string)
{
	group_opcodes.clear();
	master_opcodes.clear();
	control_opcodes.clear();
	global_opcodes.clear();
	definitions.clear();

	Variant group_1;
	bool rvalid = true;
	group_1.set_keyed("Regions", Array(), rvalid);
	group_opcodes.append(group_1);
	bool first_group = true;

	SFZResourceType current_header = SFZResourceType::Group;
	//SFZResourceType previous_header = SFZResourceType::Group;
	int i = 0;
	PackedStringArray lines = p_sfz_content_string.split("\n");
	for (auto l = 0; l < lines.size(); ++l)
	{
		String line = lines[i].strip_edges(true, false);
		if (line.length() < 1)
			continue;

		char32_t first_char = line.get(0);
		if (first_char == '#')
		{
			definitions.merge(_parse_definition(line));
		}
		else
		{
			if (first_char == '<')
			{
				const PackedStringArray opcode_array = line.split(">", false, 2);
				const String header = opcode_array[0].replace("<", "");

				if (header == "control")
				{
					current_header = SFZResourceType::Control;
				}
				else if (header == "global")
				{
					current_header = SFZResourceType::Global;
				}
				else if (header == "master")
				{
					current_header = SFZResourceType::Master;
				}
				else if (header == "group")
				{
					if (first_group)
						first_group = false;
					else
					{
						Variant group;
						group.set_keyed("Regions", Array(), rvalid);
						group_opcodes.append(group);
					}
					current_header = SFZResourceType::Group;
				}
				else if (header == "region")
				{
					Array regions = group_opcodes[group_opcodes.size() - 1].get_keyed("Regions", rvalid);
					if (!rvalid)
					{
						group_opcodes[group_opcodes.size() - 1].set_keyed("Regions", Array(), rvalid);
						regions = group_opcodes[group_opcodes.size() - 1].get_keyed("Regions", rvalid);
					}
					regions.append(Variant());
					group_opcodes[group_opcodes.size() - 1].set_keyed("Regions", regions, rvalid);
					current_header = SFZResourceType::Region;
				}

				line = opcode_array[1].strip_edges(true, true);
			}

			PackedStringArray equal_split = line.split("=", "");
			PackedStringArray residual = { equal_split[0] };
			for (i = 1; i < equal_split.size(); ++i)
			{
				residual.append_array(equal_split[i].rsplit(" ", false, 2));
			}
			Dictionary opcodes;
			for (i = 0; i < residual.size(); i += 2)
			{
				opcodes[residual[i]] = residual[i + 1];
			}

			switch (current_header)
			{
			case Region:
			{
				Array regions = group_opcodes[group_opcodes.size() - 1].get_keyed("Regions", rvalid);
				if (!rvalid)
				{
					group_opcodes[group_opcodes.size() - 1].set_keyed("Regions", Array(), rvalid);
					regions = group_opcodes[group_opcodes.size() - 1].get_keyed("Regions", rvalid);
				}
				regions.append(Variant());
				group_opcodes[group_opcodes.size() - 1].set_keyed("Regions", regions, rvalid);
			}
			break;
			case Group:
			{
				Dictionary last_group = group_opcodes[group_opcodes.size() - 1];
				last_group.merge(opcodes);
				group_opcodes[group_opcodes.size() - 1] = last_group;
			}
			break;
			case Master:
				master_opcodes.merge(opcodes);
				break;
			case Control:
				control_opcodes.merge(opcodes);
				break;
			case Global:
				global_opcodes.merge(opcodes);
				break;
			default:
				break;
			}
		}
	}
}
