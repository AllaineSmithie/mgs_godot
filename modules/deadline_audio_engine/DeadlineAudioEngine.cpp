/*
  ==============================================================================

    JuceLib.cpp
    Created: 15 Mar 2023 11:48:07pm
    Author:  RedneckJack

  ==============================================================================
*/


#include "DeadlineAudioEngine.h"
#include <core/config/project_settings.h>
#include <core/io/resource_loader.h>
#include <core/io/dir_access.h>
#include "SynthMessagingAPI.h"

#define SFIZZ_NUM_CCS 512
///
class ConstBitSpan {
public:
	ConstBitSpan() noexcept = default;
	ConstBitSpan(const uint8_t* data, size_t bits) noexcept : data_(data), bits_(bits) {}
	const uint8_t* data() const noexcept { return data_; }
	size_t bit_size() const noexcept { return bits_; }
	size_t byte_size() const noexcept { return (bits_ + 7) / 8; }
	bool test(size_t i) const noexcept { return data_[i / 8] & (1u << (i % 8)); }
	bool all() const noexcept
	{
		size_t n = bits_;
		for (size_t i = 0; i < n / 8; ++i) {
			if (data_[i] != 0xff)
				return false;
		}
		return n % 8 == 0 || data_[n / 8] == (1u << (n % 8)) - 1u;
	}
	bool any() const noexcept
	{
		size_t n = bits_;
		for (size_t i = 0; i < n / 8; ++i) {
			if (data_[i] != 0x00)
				return true;
		}
		return n % 8 != 0 && (data_[n / 8] & ((1u << (n % 8)) - 1u)) != 0;
	}
	bool none() const noexcept { return !any(); }

private:
	const uint8_t* data_ = nullptr;
	size_t bits_ = 0;
};

static void bufferedStrCat(std::string* buffer, const char* arg1, const char* arg2, const char* arg3)
{
	buffer->clear();
	buffer->append(arg1);
	buffer->append(arg2);
	buffer->append(arg3);
}

// ==============================================================================
// SFZ Resource
Error Instrument::load_file(const String& p_path)
{
	sfz_parser.init_with_file(p_path);
	return OK;
}

Error Instrument::save_file(const String& p_path, const Ref<Resource>& p_resource)
{
	return OK;
}

// ==============================================================================
// Sound Region Node

void SoundRegionNode::_validate_property(PropertyInfo& p_property) const
{
	if (p_property.name == "bus") {
		String options;
		for (int i = 0; i < AudioServer::get_singleton()->get_bus_count(); i++) {
			if (i > 0) {
				options += ",";
			}
			String name = AudioServer::get_singleton()->get_bus_name(i);
			options += name;
		}

		p_property.hint_string = options;
	}
}

void SoundRegionNode::bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_mic_channel", "mic_channel"), &SoundRegionNode::set_mic_channel);
	ClassDB::bind_method(D_METHOD("get_mic_channel"), &SoundRegionNode::get_mic_channel);
	ClassDB::bind_method(D_METHOD("set_start_offset", "start_offset"), &SoundRegionNode::set_start_offset);
	ClassDB::bind_method(D_METHOD("get_start_offset"), &SoundRegionNode::get_start_offset);
	ClassDB::bind_method(D_METHOD("set_end_sample", "end_sample"), &SoundRegionNode::set_end_sample);
	ClassDB::bind_method(D_METHOD("get_end_sample"), &SoundRegionNode::get_end_sample);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "mic_channel", PROPERTY_HINT_ENUM, ""), "set_mic_channel", "get_mic_channel");
}

void SoundRegionNode::set_mic_channel(const StringName& p_mic_channel)
{
	mic_channel = p_mic_channel;
}

StringName SoundRegionNode::get_mic_channel()
{
	for (int i = 0; i < AudioServer::get_singleton()->get_bus_count(); i++) {
		if (AudioServer::get_singleton()->get_bus_name(i) == String(mic_channel)) {
			return mic_channel;
		}
	}
	return SNAME("Master");
}


// ==============================================================================
// Instrument Node

void InstrumentGroupNode::bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_mic_channel", "mic_channel"), &InstrumentGroupNode::set_mic_channel);
	ClassDB::bind_method(D_METHOD("get_mic_channel"), &InstrumentGroupNode::get_mic_channel);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "mic_channel", PROPERTY_HINT_ENUM, ""), "set_mic_channel", "get_mic_channel");
}

void InstrumentGroupNode::_validate_property(PropertyInfo& p_property) const {
	if (p_property.name == "bus") {
		String options;
		for (int i = 0; i < AudioServer::get_singleton()->get_bus_count(); i++) {
			if (i > 0) {
				options += ",";
			}
			String name = AudioServer::get_singleton()->get_bus_name(i);
			options += name;
		}

		p_property.hint_string = options;
	}
}
void InstrumentGroupNode::set_mic_channel(const StringName& p_mic_channel)
{
	mic_channel = p_mic_channel;
	for (auto i = 0; i < get_child_count(); ++i)
	{
		if (auto child = Object::cast_to<SoundRegionNode>(get_child(i)))
		{
			child->set_mic_channel(p_mic_channel);
		}
	}
}
StringName InstrumentGroupNode::get_mic_channel() const
{
	return mic_channel;
}

PackedStringArray InstrumentGroupNode::get_configuration_warnings() const {
	PackedStringArray warnings = Node::get_configuration_warnings();

	if (!Object::cast_to<InstrumentNode>(get_parent()))
	{
		warnings.push_back(RTR("An InstrumentGroupNode can not be used without an InstrumentNode parent node."));
	}

	if (get_child_count() < 1)
	{
		warnings.push_back(RTR("An InstrumentGroupNode requires at least 1 SoundRegionNode"));
	}
	else
	{
		
		for (auto i = 0; i < get_child_count(); ++i)
		{
			if (Object::cast_to<SoundRegionNode>(get_parent()))
			{
				return warnings;
			}
		}
		warnings.push_back(RTR("No SoundRegionNode children found! Requires at least 1 SoundRegionNode"));
	}

	return warnings;
}

// ==============================================================================
// Instrument Node

void InstrumentNode::bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_master_bus", "master_bus"), &InstrumentNode::set_master_bus);
	ClassDB::bind_method(D_METHOD("get_master_bus"), &InstrumentNode::get_master_bus);

	ClassDB::bind_method(D_METHOD("set_instrument", "instrument"), &InstrumentNode::set_instrument);
	ClassDB::bind_method(D_METHOD("get_instrument"), &InstrumentNode::get_instrument);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "master_bus", PROPERTY_HINT_ENUM, ""), "set_master_bus", "get_master_bus");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "instrument", PROPERTY_HINT_RESOURCE_TYPE, "Instrument"), "set_instrument", "get_instrument");
}

void InstrumentNode::set_master_bus(const StringName& p_bus)
{
	bus = p_bus;
}

StringName InstrumentNode::get_master_bus() const
{
	for (int i = 0; i < AudioServer::get_singleton()->get_bus_count(); i++) {
		if (AudioServer::get_singleton()->get_bus_name(i) == String(bus)) {
			return bus;
		}
	}
	return SNAME("Master");
}

void InstrumentNode::_validate_property(PropertyInfo& p_property) const {
	if (p_property.name == "bus") {
		String options;
		for (int i = 0; i < AudioServer::get_singleton()->get_bus_count(); i++) {
			if (i > 0) {
				options += ",";
			}
			String name = AudioServer::get_singleton()->get_bus_name(i);
			options += name;
		}

		p_property.hint_string = options;
	}
}

void InstrumentNode::set_instrument(Ref<Instrument> p_sfz_instrument)
{
	sfz_instrument_resource = p_sfz_instrument;
}

Ref<Instrument> InstrumentNode::get_instrument() const
{
	return sfz_instrument_resource;
}

// ==============================================================================
// SFZ Resource Saver
Error ResourceFormatSaverSFZ::save(const String& p_path, const Ref<Resource>& p_resource, uint32_t p_flags) {
	Ref<Instrument> sfz = memnew(Instrument);
	Error error = sfz->save_file(p_path, p_resource);
	return error;
}

bool ResourceFormatSaverSFZ::recognize(const Ref<Resource>& p_resource) const {
	return Object::cast_to<SFZResource>(*p_resource) != NULL;
}

void ResourceFormatSaverSFZ::get_recognized_extensions(const Ref<Resource>& p_resource, List<String>* r_extensions) const {
	if (Object::cast_to<SFZResource>(*p_resource)) {
		r_extensions->push_back("json");
	}
}

// ==============================================================================
// SFZ Resource Loader
Ref<Resource> ResourceFormatLoaderSFZ::load(const String& p_path, const String& p_original_path, Error* r_error) {
	Ref<Instrument> sfz = memnew(Instrument);
	if (r_error) {
		*r_error = OK;
	}
	Error err = sfz->load_file(p_path);
	ERR_FAIL_COND_V_MSG(err != OK, sfz, "SFZ file Loading failed");
	return sfz;
}

void ResourceFormatLoaderSFZ::get_recognized_extensions(List<String>* r_extensions) const {
	if (!r_extensions->find("sfz")) {
		r_extensions->push_back("sfz");
	}
}

String ResourceFormatLoaderSFZ::get_resource_type(const String& p_path) const {
	return "Resource";
}

bool ResourceFormatLoaderSFZ::handles_type(const String& p_type) const {
	return ClassDB::is_parent_class(p_type, "Resource");
}



// ==============================================================================
// Synth Instrument

const bool SynthInstrument::get_voice_buffer_copy(const int p_region_id, float** p_out, const int p_num_samples, const int p_num_stereo_channels)
{
	return sfzPlayer->getVoiceBufferCopy(p_region_id, p_out, p_num_stereo_channels, p_num_samples);;
}
void SynthInstrument::render_voice_block(const int p_region_id, float** p_out, const int p_num_samples, const int p_num_stereo_channels)
{
	return sfzPlayer->renderVoiceBlock(p_region_id, p_out, p_num_samples, p_num_stereo_channels);
}


int SynthInstrument::get_sound_note_in_bus(const String p_path) const
{
	return sample_note_idx[p_path];
}


int SynthInstrument::get_sound_velocity_in_bus(const String p_path) const
{
	return sample_velocity_idx[p_path];
}

int SynthInstrument::get_sound_sequence_idx_in_bus(const String p_path) const
{
	return sample_sequence_idx[p_path];
}
void SynthInstrument::bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_name", "name"), &SynthInstrument::set_name);
	ClassDB::bind_method(D_METHOD("get_name"), &SynthInstrument::get_name);
	ClassDB::bind_method(D_METHOD("set_uuid", "uuid"), &SynthInstrument::set_uuid);
	ClassDB::bind_method(D_METHOD("get_uuid"), &SynthInstrument::get_uuid);
}

inline bool __isdigit(unsigned char c) { return c >= '0' && c <= '9'; }

inline bool matchOSC(const char* pattern, const char* path, unsigned* indices)
{
	unsigned nthIndex = 0;
	PackedByteArray nums;
	while (const char* endp = std::strchr(pattern, '&'))
	{
		size_t length = endp - pattern;
		if (std::strncmp(pattern, path, length))
			return false;
		pattern += length;
		path += length;

		length = 0;
		nums.clear();
		while (__isdigit(path[length]))
		{
			nums.append(path[length]);
			++length;
		}

		const int digit = atoi((const char*)nums.ptr());
		indices[nthIndex++] = digit;

		pattern += 1;
		path += length;
	}

	return !std::strcmp(path, pattern);
}

void SynthInstrument::set_name(StringName p_name)
{
	name = p_name;
}

StringName SynthInstrument::get_name()
{
	return name;
}

void SynthInstrument::set_uuid(uint32_t p_uuid)
{
	uuid = p_uuid;
}

uint32_t SynthInstrument::get_uuid()
{
	return uuid;
}

void SynthInstrument::_update_sfz_player()
{
	std::string sfz_string;
	sfz_string.append("seq_length=" + std::to_string(seq_length));
	for (auto& line : sfz_regions_content)
	{
		sfz_string += line.value.ascii().get_data();
		sfz_string += "\r\n";
	}
	const String project_path = ProjectSettings::get_singleton()->get_resource_path();
	sfzPlayer->loadSfzString(project_path.ascii().get_data(), sfz_string);
}

int SynthInstrument::add_sound_to_bus(const String p_path)
{
	const String global_path = ProjectSettings::get_singleton()->globalize_path(p_path);
	//const String global_resource_path = ProjectSettings::get_singleton()->get_resource_path();
	//const String rel_path = global_resource_path.path_to(global_path);
	for (uint32_t i = 0; i < max_idx; ++i)
	{
		if (!reversed_idx.has(i))
		{
			sample_sequence_idx[p_path] = i % 16384;
			sample_note_idx[p_path] = (i % 16384) / 128;
			sample_velocity_idx[p_path] = i % 128;
			sfz_regions_content[i] = "<region> sample=" + global_path
				+ " amp_veltrack=0"
				+ " ampeg_attack=0"
				+ " ampeg_release=0"
				+ " lokey=" + sample_note_idx[p_path]
				+ " hikey=" + sample_note_idx[p_path]
				+ " pitch_keycenter=" + sample_note_idx[p_path]
				+ " lovel=" + sample_velocity_idx[p_path]
				+ " hivel=" + sample_velocity_idx[p_path]
				+ " seq_position=" + String::num(sample_sequence_idx[p_path] + 1, 0);
			seq_length = MAX(sample_sequence_idx[p_path], seq_length);
			sample_idx_in_reversed[p_path] = i;
			reversed_idx[i] = p_path;
			break;
		}
	}
	_update_sfz_player();
	return sfzPlayer->getRegionID(global_path.ascii().get_data());
}

void SynthInstrument::remove_sound_from_bus(const String p_path)
{
	const String global_path = ProjectSettings::get_singleton()->globalize_path(p_path);

	reversed_idx.erase(sample_idx_in_reversed[p_path]);
	sample_velocity_idx.erase(p_path);
	sample_note_idx.erase(p_path);
	sample_sequence_idx.erase(p_path);
	sfz_regions_content.erase(sample_idx_in_reversed[p_path]);
	sample_idx_in_reversed.erase(p_path);

	_update_sfz_player();
}

SynthInstrument::SynthInstrument()
{
	blob.reserve(128 * 1024);
	msgbuf.resize(1024);
	pathbuf.reserve(256);

	sfzPlayer.reset(new sfz::Sfizz());
	client = sfz::Sfizz::createClient(&cdata);
	cdata.synth = sfzPlayer.get();
	cdata.client = client.get();
	cdata.blob = &blob;
	cdata.msgbuf = &msgbuf;
	cdata.pathbuf = &pathbuf;

	sfz::Sfizz::setReceiveCallback(*client, [](void* data, int, const char* path, const char* sig, const sfizz_arg_t* args)
		{
			ClientData& cdata = *reinterpret_cast<ClientData*>(data);
	unsigned indices[8];

	///
	uint32_t msglen = sfizz_prepare_message(cdata.msgbuf->data(), cdata.msgbuf->size(), path, sig, args);
	if (msglen > cdata.msgbuf->size()) {
		cdata.msgbuf->resize(msglen);
		sfizz_prepare_message(cdata.msgbuf->data(), cdata.msgbuf->size(), path, sig, args);
	}
	cdata.blob->append(cdata.msgbuf->data(), msglen);

	///
	if (matchOSC(SynthMessagingAPI::key_slots, path, indices) && !strcmp(sig, "b")) {
		ConstBitSpan bits(args[0].b->data, 8 * args[0].b->size);
		for (unsigned key = 0; key < 128 && key < bits.bit_size(); ++key) {
			if (bits.test(key)) {
				bufferedStrCat(cdata.pathbuf, "/key", itos(key).ascii().get_data(), "/label");
				cdata.synth->sendMessage(*cdata.client, 0, cdata.pathbuf->c_str(), "", nullptr);
			}
		}
	}
	else if (matchOSC(SynthMessagingAPI::sw_last_slots, path, indices) && !strcmp(sig, "b")) {
		ConstBitSpan bits(args[0].b->data, 8 * args[0].b->size);
		for (unsigned key = 0; key < 128 && key < bits.bit_size(); ++key) {
			if (bits.test(key)) {
				bufferedStrCat(cdata.pathbuf, "/sw/last/", itos(key).ascii().get_data(), "/label");
				cdata.synth->sendMessage(*cdata.client, 0, cdata.pathbuf->c_str(), "", nullptr);
			}
		}
	}
	else if (matchOSC(SynthMessagingAPI::cc_slots, path, indices) && !strcmp(sig, "b")) {
		ConstBitSpan bits(args[0].b->data, 8 * args[0].b->size);
		for (unsigned cc = 0; cc < SFIZZ_NUM_CCS && cc < bits.bit_size(); ++cc) {
			if (bits.test(cc)) {
				bufferedStrCat(cdata.pathbuf, "/cc", itos(cc).ascii().get_data(), "/value");
				cdata.synth->sendMessage(*cdata.client, 0, cdata.pathbuf->c_str(), "", nullptr);
				bufferedStrCat(cdata.pathbuf, "/cc", itos(cc).ascii().get_data(), "/default");
				cdata.synth->sendMessage(*cdata.client, 0, cdata.pathbuf->c_str(), "", nullptr);
				bufferedStrCat(cdata.pathbuf, "/cc", itos(cc).ascii().get_data(), "/value");
				cdata.synth->sendMessage(*cdata.client, 0, cdata.pathbuf->c_str(), "", nullptr);
			}
		}
	}
		});

	sfzPlayer->sendMessage(*client, 0, SynthMessagingAPI::num_regions, "", nullptr);
	sfzPlayer->sendMessage(*client, 0, SynthMessagingAPI::num_groups, "", nullptr);
	sfzPlayer->sendMessage(*client, 0, SynthMessagingAPI::num_masters, "", nullptr);
	sfzPlayer->sendMessage(*client, 0, SynthMessagingAPI::num_curves, "", nullptr);
	sfzPlayer->sendMessage(*client, 0, SynthMessagingAPI::num_samples, "", nullptr);
	sfzPlayer->sendMessage(*client, 0, SynthMessagingAPI::root_path, "", nullptr);
	sfzPlayer->sendMessage(*client, 0, SynthMessagingAPI::image, "", nullptr);
	sfzPlayer->sendMessage(*client, 0, SynthMessagingAPI::image_controls, "", nullptr);
	sfzPlayer->sendMessage(*client, 0, SynthMessagingAPI::key_slots, "", nullptr);
	sfzPlayer->sendMessage(*client, 0, SynthMessagingAPI::sw_last_slots, "", nullptr);
	sfzPlayer->sendMessage(*client, 0, SynthMessagingAPI::cc_slots, "", nullptr);
	sfzPlayer->sendMessage(*client, 0, SynthMessagingAPI::sustain_or_sostenuto_slots, "", nullptr);

	blob.shrink_to_fit();
}

SynthInstrument::~SynthInstrument()
{
	if (!sfzPlayer)
		return;

	sfzPlayer->allSoundOff();
	sfzPlayer = nullptr;
}


// ==============================================================================
// Deadline Audio Engine
#define DEADLINE_AUDIO_DEFAULT_NUM_VOICES 96
void DeadlineAudioEngine::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("load_instrument", "instrument_file"), &DeadlineAudioEngine::load_instrument);
	ClassDB::bind_method(D_METHOD("clear_instrument"), &DeadlineAudioEngine::clear_instrument);
}

DeadlineAudioEngine* DeadlineAudioEngine::singleton = nullptr;

int DeadlineAudioEngine::add_sound_to_bus(const String p_path, const int p_bus_idx)
{
	return sfizzPlayer[p_bus_idx]->add_sound_to_bus(p_path);
}

void DeadlineAudioEngine::remove_sound_from_bus(const String p_path, const int p_bus_idx)
{
	return sfizzPlayer[p_bus_idx]->remove_sound_from_bus(p_path);
}

const bool DeadlineAudioEngine::render_voice_block(const int p_bus_idx, const int p_voice_id, float** p_out, const int p_num_samples, const int p_num_stereo_channels)
{
	if (p_bus_idx < 0 || p_bus_idx > sfizzPlayer.size())
		return false;
	sfizzPlayer[p_bus_idx]->render_voice_block(p_voice_id, p_out, p_num_samples, p_num_stereo_channels);
	return true;
}


const bool DeadlineAudioEngine::get_voice_buffer_copy(const int p_bus_idx, const int p_voice_id, float** p_out, const int p_num_samples, const int p_num_stereo_channels)
{
	return sfizzPlayer[p_bus_idx]->get_voice_buffer_copy(p_voice_id, p_out, p_num_samples, p_num_stereo_channels);
}

int DeadlineAudioEngine::add_sound_to_bus(const String p_path, const StringName p_bus_name)
{
	for (auto sfz : sfizzPlayer)
	{
		if (sfz->get_name() == p_bus_name)
		{
			return sfz->add_sound_to_bus(p_path);
		}
	}
	ERR_FAIL_V_MSG(-1, "Bus " + p_bus_name + " not found");
	return -1;
}

int DeadlineAudioEngine::get_sound_note_in_bus(const String p_path, const int p_idx) const
{
	ERR_FAIL_COND_V_MSG(p_idx >= sfizzPlayer.size(), -1, "Bus Idx out of bounds");
	return sfizzPlayer[p_idx]->get_sound_note_in_bus(p_path);
}

int DeadlineAudioEngine::get_sound_note_in_bus(const String p_path, const StringName p_bus_name) const
{
	for (auto sfz : sfizzPlayer)
	{
		if (sfz->get_name() == p_bus_name)
		{
			return sfz->get_sound_note_in_bus(p_path);
		}
	}
	return -1;
}

int DeadlineAudioEngine::get_sound_velocity_in_bus(const String p_path, const int p_idx) const
{
	ERR_FAIL_COND_V_MSG(p_idx >= sfizzPlayer.size(), -1, "Bus Idx out of bounds");
	return sfizzPlayer[p_idx]->get_sound_velocity_in_bus(p_path);
}
int DeadlineAudioEngine::get_sound_velocity_in_bus(const String p_path, const StringName p_bus_name) const
{
	for (auto sfz : sfizzPlayer)
	{
		if (sfz->get_name() == p_bus_name)
		{
			return sfz->get_sound_velocity_in_bus(p_path);
		}
	}
	return -1;
}

int DeadlineAudioEngine::get_sound_sequence_idx_in_bus(const String p_path, const int p_idx) const
{
	ERR_FAIL_COND_V_MSG(p_idx >= sfizzPlayer.size(), -1, "Bus Idx out of bounds");
	return sfizzPlayer[p_idx]->get_sound_sequence_idx_in_bus(p_path);
}
int DeadlineAudioEngine::get_sound_sequence_idx_in_bus(const String p_path, const StringName p_bus_name) const
{
	for (auto sfz : sfizzPlayer)
	{
		if (sfz->get_name() == p_bus_name)
		{
			return sfz->get_sound_sequence_idx_in_bus(p_path);
		}
	}
	return -1;
}

int DeadlineAudioEngine::get_voice_position(const int p_bus_idx, const int p_voice_id) const
{
	ERR_FAIL_COND_V_MSG(p_bus_idx >= sfizzPlayer.size(), 0, "Bus Idx out of bounds");
	return sfizzPlayer[p_bus_idx]->sfzPlayer->getVoicePosition(p_voice_id);
}

int DeadlineAudioEngine::get_region_length_samples(const int p_bus_idx, const int p_region_id) const
{
	ERR_FAIL_COND_V_MSG(p_bus_idx >= sfizzPlayer.size(), 0, "Bus Idx out of bounds");
	return sfizzPlayer[p_bus_idx]->sfzPlayer->getRegionLengthSamples(p_region_id);
}

float DeadlineAudioEngine::get_region_length_seconds(const int p_bus_idx, const int p_region_id) const
{
	ERR_FAIL_COND_V_MSG(p_bus_idx >= sfizzPlayer.size(), 0, "Bus Idx out of bounds");
	return sfizzPlayer[p_bus_idx]->sfzPlayer->getRegionLengthSeconds(p_region_id);
}

void DeadlineAudioEngine::set_voice_position(const int p_bus_idx, const int p_voice_id, const int p_position_frames)
{
	ERR_FAIL_COND(p_bus_idx >= sfizzPlayer.size());
	return sfizzPlayer[p_bus_idx]->sfzPlayer->setVoicePosition(p_voice_id, p_position_frames);
}

bool DeadlineAudioEngine::play_audio_stream(const int p_bus_idx, const uint8_t p_midi_note, const uint8_t p_velocity, const uint8_t p_sequence_idx)
{
	ERR_FAIL_COND_V_MSG(p_bus_idx >= sfizzPlayer.size(), false, "Bus Idx out of bounds");
	sfizzPlayer[p_bus_idx]->sfzPlayer->noteOn(0, p_midi_note, p_velocity);
	return true;
}

int DeadlineAudioEngine::play_audio_stream(const int p_bus_idx, const int p_region_id)
{
	ERR_FAIL_COND_V_MSG(p_bus_idx >= sfizzPlayer.size(), false, "Bus Idx out of bounds");
	return sfizzPlayer[p_bus_idx]->sfzPlayer->playRegionByID(p_region_id);
}

bool DeadlineAudioEngine::stop_audio_stream(const int p_bus_idx, const uint8_t p_midi_note, const uint8_t p_velocity, const uint8_t p_sequence_idx)
{
	ERR_FAIL_COND_V_MSG(p_bus_idx >= sfizzPlayer.size(), false, "Bus Idx out of bounds");
	sfizzPlayer[p_bus_idx]->sfzPlayer->noteOff(0, p_midi_note, p_velocity);
	return true;
}

bool DeadlineAudioEngine::stop_audio_stream(const int p_bus_idx, const int p_region_id)
{
	ERR_FAIL_COND_V_MSG(p_bus_idx >= sfizzPlayer.size(), false, "Bus Idx out of bounds");
	return sfizzPlayer[p_bus_idx]->sfzPlayer->stopRegionByID(p_region_id);
}

DeadlineAudioEngine* DeadlineAudioEngine::get_singleton()
{
	return singleton;
};

DeadlineAudioEngine::DeadlineAudioEngine()
{
	singleton = this;
	
}
DeadlineAudioEngine::~DeadlineAudioEngine()
{
	AudioServer::get_singleton()->remove_mix_callback(_mix_audios, this);
	AudioServer::get_singleton()->disconnect("bus_layout_changed", callable_mp(this, &DeadlineAudioEngine::on_bus_layout_changed));
	AudioServer::get_singleton()->disconnect("bus_layout_loaded", callable_mp(this, &DeadlineAudioEngine::on_bus_layout_changed));
	for (auto i = 0; i < sfizzPlayer.size(); ++i)
	{
		sfizzPlayer[i]->sfzPlayer->allSoundOff();
		memdelete(sfizzPlayer[i]);
		sfizzPlayer[i] = nullptr;
	}
	singleton = nullptr;
}

void DeadlineAudioEngine::init_audio_server()
{
	AudioServer::get_singleton()->connect("bus_layout_changed", callable_mp(this, &DeadlineAudioEngine::on_bus_layout_changed));
	AudioServer::get_singleton()->connect("bus_layout_loaded", callable_mp(this, &DeadlineAudioEngine::on_bus_layout_changed));
	AudioServer::get_singleton()->add_mix_callback(_mix_audios, this);
	on_bus_layout_changed();
}

void DeadlineAudioEngine::_mix_audios(void* p_self)
{
	ERR_FAIL_NULL(p_self);
	static_cast<DeadlineAudioEngine*>(p_self)->_mix_audio();
}

// Called from audio thread
void DeadlineAudioEngine::_mix_audio()
{

	if (lock_render || sfizzPlayer.size() < 1 || sfz_mix_buffer.getNumSamples() < 1)
	{
		return;
	}

	int device_stereo_channel_count = AudioServer::get_singleton()->get_channel_count();
	ERR_FAIL_COND(sfz_mix_buffer.getNumChannels() / 2 != device_stereo_channel_count);


	// as the rendering is happening in the individual audiostreamplayers
	// the sfizz engine was rewritten to support single voice rendering
	// therefore the main render method is only triggering the renderBlockEmpty()
	// method
	for (auto bus_idx = 0; bus_idx < device_stereo_channel_count; ++bus_idx)
	{
		SynthInstrument* sfz = sfizzPlayer[bus_idx];
		if (!sfz) // safety
			continue;

		//sfz_mix_buffer.clear();
		// !! Trigger only the render callback,
		// pickup the rendered buffers in the upcoming audio_stream_file mix_internal callback
		sfz->sfzPlayer->renderBlockEmpty(const_cast<float**>(sfz_mix_buffer.getArrayOfWritePointers()), sfz_mix_buffer.getNumSamples(), sfz_mix_buffer.getNumChannels() / 2);


		/*AudioFrame* target = AudioServer::get_singleton()->thread_get_channel_mix_buffer(bus_idx, 0);
		ERR_FAIL_COND(!target);
		for (int j = 0; j < sfz_mix_buffer.getNumSamples(); j++)
		{
			target[j] += AudioFrame(sfz_mix_buffer.getSample(0, j), sfz_mix_buffer.getSample(1, j));
		}*/

		
	}


	/*
	AudioFrame* targets[4];

	for (int k = 0; k < cc; k++) {
		targets[k] = AudioServer::get_singleton()->thread_get_channel_mix_buffer(bus_index, k);
		ERR_FAIL_COND(!targets[k]);
	}

	for (int j = 0; j < buffer_size; j++) {
		AudioFrame frame = buffer[j] * vol;
		for (int k = 0; k < cc; k++) {
			targets[k][j] += frame;
		}
	}*/
}

void DeadlineAudioEngine::on_bus_layout_changed()
{
	lock_render = true;

	// TEMPORARILY DISABLED
	// for PortAudio implementation
	/*

	
	// always update sfz_mix_buffer_sizer
	sfz_mix_buffer.setSize(AudioServer::get_singleton()->get_channel_count() * 2, AudioServer::get_singleton()->thread_get_mix_buffer_size());
	// moved or renamed
	List<SynthInstrument*> new_sort;
	for (auto i = 0; i < AudioServer::get_singleton()->get_bus_count(); ++i)
	{
		bool found = false;
		for (auto a = 0; a < sfizzPlayer.size(); ++a)
		{
			if (!sfizzPlayer[a])
				continue;

			if (sfizzPlayer[a]->get_uuid() == AudioServer::get_singleton()->get_bus_uuid(i))
			{
				new_sort.push_back(sfizzPlayer[a]);
				sfizzPlayer[a]->set_uuid(-1);

				// update name
				new_sort[i]->set_name(AudioServer::get_singleton()->get_bus_name(i));
				found = true;
				break;
			}
		}
		if (!found)
		{
			print_line("Bus " + AudioServer::get_singleton()->get_bus_name(i) + " Synth added. UUID: " + String::num(AudioServer::get_singleton()->get_bus_uuid(i)), 0);
			new_sort.push_back(add_synth_to_bus(i));
		}
	}

	for (auto i = 0; i < sfizzPlayer.size(); ++i)
	{
		if (!sfizzPlayer[i])
			continue;

		if (sfizzPlayer[i]->get_uuid() != -1)
		{
			memdelete(sfizzPlayer[i]);
			sfizzPlayer[i] = nullptr;
		}
	}

	sfizzPlayer = new_sort;
	*/

	lock_render = false;
}

SynthInstrument* DeadlineAudioEngine::add_synth_to_bus(const int p_bus_index)
{
	SynthInstrument* synth = memnew(SynthInstrument);
	std::string bus_name_std_string = (const char*)AudioServer::get_singleton()->get_bus_name(p_bus_index).to_ascii_buffer().ptr();
	const float samplerate = AudioServer::get_singleton()->get_mix_rate();
	const uint64_t buffer_size = AudioServer::get_singleton()->thread_get_mix_buffer_size();
	synth->set_name(AudioServer::get_singleton()->get_bus_name(p_bus_index));
	synth->set_uuid(AudioServer::get_singleton()->get_bus_uuid(p_bus_index));

	//synth->sfzPlayer->setLoggingPrefix(bus_name_std_string);
	//synth->sfzPlayer->setSampleQuality(sfz::Sfizz::ProcessMode::ProcessLive, 0);
	
	synth->sfzPlayer->setSampleRate(samplerate);
	synth->sfzPlayer->setSamplesPerBlock(buffer_size);
	synth->sfzPlayer->setNumVoices(DEADLINE_AUDIO_DEFAULT_NUM_VOICES);
	return synth;
}


void DeadlineAudioEngine::load_instrument(String p_file)
{
	Ref<Instrument> sfz_instrument = ResourceLoader::load(p_file);
	if (!sfz_instrument.is_valid())
		ERR_FAIL_MSG("Instrument Load Error");
}


bool DeadlineAudioEngine::clear_instrument()
{
	return true;
}
