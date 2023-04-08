
#include "ShowEngine.h"
#include "digishow/DigiShow/digishow_environment.h"
#include "digishow/DigiShow/digishow_metronome.h"
#include "digishow/DigiShow/dgs_midi_interface.h"
#include "digishow/DigiShow/dgs_rioc_interface.h"
#include "digishow/DigiShow/dgs_modbus_interface.h"
#include "digishow/DigiShow/dgs_hue_interface.h"
#include "digishow/DigiShow/dgs_dmx_interface.h"
#include "digishow/DigiShow/dgs_artnet_interface.h"
#include "digishow/DigiShow/dgs_osc_interface.h"
#include "digishow/DigiShow/dgs_audioin_interface.h"
#include "digishow/DigiShow/dgs_screen_interface.h"
#include "digishow/DigiShow/dgs_pipe_interface.h"
#include "digishow/DigiShow/dgs_launch_interface.h"
#include "digishow/DigiShow/dgs_hotkey_interface.h"

#include <assert.h>

#undef interface

// ===========================================================================================================================

ModbusShowInterface::ModbusShowInterface()
	: ShowInterface(ShowInterface::OUTPUT)
{
}

ModbusShowInterface::~ModbusShowInterface()
{
	m_interface = ShowEngine::get_singleton()->newInterface("modbus");
}

void ModbusShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

RaspberryShowInterface::RaspberryShowInterface()
	: ShowInterface(ShowInterface::OUTPUT)
{
}

RaspberryShowInterface::~RaspberryShowInterface()
{
	m_interface = ShowEngine::get_singleton()->newInterface("raspberry");
}

void RaspberryShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

ArduinoShowInterface::ArduinoShowInterface()
	: ShowInterface(ShowInterface::OUTPUT)
{
}

ArduinoShowInterface::~ArduinoShowInterface()
{
	m_interface = ShowEngine::get_singleton()->newInterface("rioc");
}

void ArduinoShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

SmartlightShowInterface::SmartlightShowInterface()
	: ShowInterface(ShowInterface::OUTPUT)
{
}

SmartlightShowInterface::~SmartlightShowInterface()
{
	m_interface = ShowEngine::get_singleton()->newInterface("hue");
}

void SmartlightShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

OSCShowInterface::OSCShowInterface()
	: ShowInterface(ShowInterface::IN_OUT)
{
}

OSCShowInterface::~OSCShowInterface()
{
	m_interface = ShowEngine::get_singleton()->newInterface("osc");
}

void OSCShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

DmxIPShowInterface::DmxIPShowInterface()
	: ShowInterface(ShowInterface::OUTPUT)
{
}

DmxIPShowInterface::~DmxIPShowInterface()
{
	m_interface = ShowEngine::get_singleton()->newInterface("artnet");
}

void DmxIPShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

DmxUsbShowInterface::DmxUsbShowInterface()
	: ShowInterface(ShowInterface::OUTPUT)
{
}

DmxUsbShowInterface::~DmxUsbShowInterface()
{
	m_interface = ShowEngine::get_singleton()->newInterface("dmx");
}

void DmxUsbShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

InputEventShowInterface::InputEventShowInterface()
	: ShowInterface(ShowInterface::INPUT)
{
	m_interface = ShowEngine::get_singleton()->newInterface("hotkey");
}

InputEventShowInterface::~InputEventShowInterface()
{
}

void InputEventShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

MidiShowInterface::MidiShowInterface()
	: ShowInterface(ShowInterface::IN_OUT)
{
	m_interface = ShowEngine::get_singleton()->newInterface("midi");
}

MidiShowInterface::~MidiShowInterface()
{
}

void MidiShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

AudioShowInterface::AudioShowInterface()
	: ShowInterface(ShowInterface::IN_OUT)
{
	m_interface = ShowEngine::get_singleton()->newInterface("audio");
}

AudioShowInterface::~AudioShowInterface()
{
}

void AudioShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

ScreenShowInterface::ScreenShowInterface()
	: ShowInterface(ShowInterface::OUTPUT)
{
	m_interface = ShowEngine::get_singleton()->newInterface("screen");
}

ScreenShowInterface::~ScreenShowInterface()
{
}

void ScreenShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

CameraShowInterface::CameraShowInterface()
	: ShowInterface(ShowInterface::IN_OUT)
{
	m_interface = ShowEngine::get_singleton()->newInterface("camera");
}

CameraShowInterface::~CameraShowInterface()
{
}

void CameraShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

MetronomeShowInterface::MetronomeShowInterface()
	: ShowInterface(ShowInterface::INPUT)
{
	m_interface = ShowEngine::get_singleton()->newInterface("metronome");
}

MetronomeShowInterface::~MetronomeShowInterface()
{
}

void MetronomeShowInterface::_bind_methods()
{
}

// ===========================================================================================================================

RemoteShowInterface::RemoteShowInterface()
	: ShowInterface(ShowInterface::IN_OUT)
{
	m_interface = ShowEngine::get_singleton()->newInterface("pipe");
}

RemoteShowInterface::~RemoteShowInterface()
{
}

void RemoteShowInterface::_bind_methods()
{
}



// ===========================================================================================================================

ShowInterface::ShowInterface()
	: m_input_option(SocketType::IN_OUT)
{
}

ShowInterface::ShowInterface(const SocketType p_input_option)
		:	m_input_option(p_input_option)
{
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_MIDI);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_ARDUINO);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_MODBUS_ROBOTICS);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_SMARTLIGHT);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_DMX_USB);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_DMX_IP);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_OSC);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_AUDIOIN);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_APLAY);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_MPLAY);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_REMOTE);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_LAUNCH);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_INPUT_EVENT);
	BIND_ENUM_CONSTANT(INTERFACE_TYPE_METRONOME);

}

ShowInterface::~ShowInterface()
{
}

void ShowInterface::_notification(int notification)
{
}

void ShowInterface::_setEnabled(bool p_enabled)
{
	if (m_interface.is_null() || !m_interface.is_valid())
		return;

	if (p_enabled)
		m_interface->openInterface();
	else
		m_interface->closeInterface();
}

bool ShowInterface::_getEnabled()
{
	if (m_interface.is_null() || !m_interface.is_valid())
		return false;

	return m_interface->isInterfaceOpened();
}

void ShowInterface::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &ShowInterface::_setEnabled);
	ClassDB::bind_method(D_METHOD("get_enabled"), &ShowInterface::_getEnabled);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled", PROPERTY_HINT_NONE, "Enabled"), "set_enabled", "get_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "notes", PROPERTY_HINT_MULTILINE_TEXT, "Notes"), "set_notes", "get_notes");
}

// ===========================================================================================================================



// ===========================================================================================================================
NodeShowSlot::NodeShowSlot()
{
	m_slot = ShowEngine::get_singleton()->addSlot(get_name());
}

NodeShowSlot::~NodeShowSlot()
{
}

void NodeShowSlot::_notification(int notification)
{
}

void NodeShowSlot::_setEnabled(bool p_enabled)
{
	ShowEngine::get_singleton()->setSlotEnabled(m_slot, p_enabled);
}
const bool NodeShowSlot::_getEnabled()
{
	return false;
}
void NodeShowSlot::_setInputInterfaceType(ShowInterface::InterfaceType p_interface_type)
{
	//ShowEngine::get_singleton()->setLaunchOption(m_slot, p_enabled);
	//m_inputInterfaceType = p_interface_type;
}

void NodeShowSlot::_setOutputInterfaceType(ShowInterface::InterfaceType p_interface_type)
{
	//ShowEngine::get_singleton()->setLaunchOption(m_slot, p_enabled);
	//m_outputInterfaceType = p_interface_type;
}

void NodeShowSlot::_setScript(Ref<GDScript> p_script)
{
	m_script = p_script;
}

void NodeShowSlot::_setAnimationPlayer(NodePath p_animation_player)
{
	m_animationPlayer = p_animation_player;
}

void NodeShowSlot::_setAnimation(String p_animation)
{
	m_animation = p_animation;
}

ShowInterface::InterfaceType NodeShowSlot::_getInputInterfaceType()
{
	return ShowInterface::InterfaceType::INTERFACE_TYPE_NONE; //ShowEngine::get_singleton()->getSource()->type;
}

ShowInterface::InterfaceType NodeShowSlot::_getOutputInterfaceType()
{
	return ShowInterface::InterfaceType::INTERFACE_TYPE_NONE; //ShowEngine::get_singleton()->getDestination()->type;
}

Ref<GDScript> NodeShowSlot::_getScript()
{
	return m_script;
}

NodePath NodeShowSlot::_getAnimationPlayer()
{
	return m_animationPlayer;
}

String NodeShowSlot::_getAnimationName()
{
	return m_animation;
}

void NodeShowSlot::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_input", "input"), &NodeShowSlot::_setInputInterfaceType);
	ClassDB::bind_method(D_METHOD("get_input"), &NodeShowSlot::_getInputInterfaceType);

	ClassDB::bind_method(D_METHOD("set_output", "output"), &NodeShowSlot::_setOutputInterfaceType);
	ClassDB::bind_method(D_METHOD("get_output"), &NodeShowSlot::_getOutputInterfaceType);

	ClassDB::bind_method(D_METHOD("set_script", "script"), &NodeShowSlot::_setScript);
	ClassDB::bind_method(D_METHOD("get_script"), &NodeShowSlot::_getScript);

	ClassDB::bind_method(D_METHOD("get_animation_name"), &NodeShowSlot::_getAnimationName);

	ClassDB::bind_method(D_METHOD("set_animation_player", "animation_player"), &NodeShowSlot::_setAnimationPlayer);
	ClassDB::bind_method(D_METHOD("get_animation_player"), &NodeShowSlot::_getAnimationPlayer);


	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled", PROPERTY_HINT_NONE, "Enabled"), "set_enabled", "get_enabled");

	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "animation_player", PROPERTY_HINT_RESOURCE_TYPE, "Animation Player"), "set_animation_player", "get_animation_player");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "animation", PROPERTY_HINT_RESOURCE_TYPE, "Animation"), "set_animation", "get_animation");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "input_interface", PROPERTY_HINT_ENUM, "InterfaceType"), "set_input_interface", "get_input_interface");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "output_interface", PROPERTY_HINT_ENUM, "InterfaceType"), "set_output_interface", "get_output_interface");

}


void NodeShowSlot::_validate_property(PropertyInfo& p_property) const
{
	if (p_property.name == "animation")
	{
		List<StringName> animnames;

		p_property.hint_string = "";
		if (m_animationPlayer.is_empty())
			return;

		auto* animnode = get_node_or_null(m_animationPlayer);
		if (!animnode)
			return;

		auto animationplayerobj = cast_to<AnimationPlayer>(animnode);
		if (!animationplayerobj)
			return;

		animationplayerobj->get_animation_list(&animnames);

		animnames.push_front("---");
		String hint;
		for (auto n : animnames)
		{
			if (n != animnames.front())
			{
				hint += ",";
			}

			hint += n;
		}

		p_property.hint_string = hint;
	}
}
void NodeShowSlot::_get_property_list(List<PropertyInfo>* p_list) const
{
}
// ===========================================================================================================================

ShowEngine* ShowEngine::singleton = nullptr;
ShowEngine* ShowEngine::get_singleton()
{
	return singleton;
}

ShowEngine::ShowEngine()
{
	// start main app
	ERR_FAIL_COND(singleton != nullptr);
	singleton = this;

	m_timer.reset(new Timer());
	enableAutostart(true);
}

ShowEngine::~ShowEngine()
{
	ERR_FAIL_COND(singleton != this);
	m_timer = nullptr;
	m_metronome = nullptr;

	m_filepath.clear();

	// TO DO -> check if leaking!
	//m_slotsOwner.clear();

	singleton = nullptr;
}

void ShowEngine::_bind_methods()
{
	//ClassDB::bind_method(D_METHOD("new_show"), &ShowEngine::createNewShow);
	
	ClassDB::bind_method(D_METHOD("start_show"), &ShowEngine::start);
	ClassDB::bind_method(D_METHOD("pause_show"), &ShowEngine::pause);
	ClassDB::bind_method(D_METHOD("pause_stop"), &ShowEngine::stop);
	
	ClassDB::bind_method(D_METHOD("is_starting"), &ShowEngine::isStarting);
	ClassDB::bind_method(D_METHOD("is_running"), &ShowEngine::isRunning);
	ClassDB::bind_method(D_METHOD("is_paused"), &ShowEngine::isPaused);

	ClassDB::bind_method(D_METHOD("enableAutostart", "autostart"), &ShowEngine::enableAutostart);

	ClassDB::bind_method(D_METHOD("add_slot"), &ShowEngine::addSlot);
	ClassDB::bind_method(D_METHOD("duplicate_slot", "slot_index"), &ShowEngine::duplicateSlot);
	ClassDB::bind_method(D_METHOD("remove_slot", "slot_index"), &ShowEngine::removeSlot);
	ClassDB::bind_method(D_METHOD("clear"), &ShowEngine::clear);

	//ClassDB::bind_method(D_METHOD("import_data", "data"), &ShowEngine::importData);
	//ClassDB::bind_method(D_METHOD("export_data", "slot_list_order", "only_selection"), &ShowEngine::exportData);

	ClassDB::bind_method(D_METHOD("new_interface", "interface_type"), &ShowEngine::newInterface);
	ClassDB::bind_method(D_METHOD("delete_interface", "interface_index"), &ShowEngine::deleteInterface);
	

	ClassDB::bind_method(D_METHOD("interface_count"), &ShowEngine::interfaceCount);
	ClassDB::bind_method(D_METHOD("interface_info", "index"), &ShowEngine::interfaceInfo);
	ClassDB::bind_method(D_METHOD("interface_list_info"), &ShowEngine::interfaceInfoList);

	ClassDB::bind_method(D_METHOD("slot_count"), &ShowEngine::slotCount);
	//ClassDB::bind_method(D_METHOD("slotAt", "index"), &ShowEngine::slotAt);
	ClassDB::bind_method(D_METHOD("slotList"), &ShowEngine::getSlotList);
	
	ClassDB::bind_method(D_METHOD("update_launch", "launch_name", "slot_launch_options"), &ShowEngine::updateLaunch);
	ClassDB::bind_method(D_METHOD("delete_launch", "launch_name"), &ShowEngine::deleteLaunch);
	ClassDB::bind_method(D_METHOD("start_launch", "launch_name"), &ShowEngine::startLaunch);
	ClassDB::bind_method(D_METHOD("get_launch_options", "launch_name"), &ShowEngine::getLaunchOptions);
	ClassDB::bind_method(D_METHOD("set_launch_option", "launch_name", "option_name", "option_value"), &ShowEngine::setLaunchOption);
	ClassDB::bind_method(D_METHOD("get_slot_launch_details", "launch_name"), &ShowEngine::getSlotLaunchDetails);
	ClassDB::bind_method(D_METHOD("get_slot_launch_options", "launch_name"), &ShowEngine::getSlotLaunchOptions);

	ClassDB::bind_method(D_METHOD("find_interface_and_endpoint", "name", "interface_index"), &ShowEngine::findInterfaceAndEndpointIndex);
	ClassDB::bind_method(D_METHOD("confirm_endpoint_is_employed_index", "interface_index", "endpoint_index"), &ShowEngine::confirmEndpointIsEmployedIndex);
	ClassDB::bind_method(D_METHOD("confirm_endpoint_is_employed_name", "interface_name", "endpoint_name"), &ShowEngine::confirmEndpointIsEmployedName);

	/**/

	//BIND_ENUM_CONSTANT(STATE_STOPPED);
	//BIND_ENUM_CONSTANT(STATE_RUNNING);
	//BIND_ENUM_CONSTANT(STATE_PAUSED);
}


void ShowEngine::create_icon(const String& icon_path, IconType icon_type)
{
	Ref<Texture2D> texture = ResourceLoader::load(icon_path);

	if (texture.is_valid())
	{
		icons[icon_type] = texture;
	}
}

Error ShowEngine::init()
{

	//ADD_SIGNAL(MethodInfo("file_path_changed", PropertyInfo(Variant::STRING, "path")));
	ADD_SIGNAL(MethodInfo("interface_list_changed"));
	ADD_SIGNAL(MethodInfo("slot_list_changed"));
	ADD_SIGNAL(MethodInfo("launch_list_changed"));
	ADD_SIGNAL(MethodInfo("starting"));
	ADD_SIGNAL(MethodInfo("running"));
	ADD_SIGNAL(MethodInfo("paused"));

	//g_app.connect("interface_list_changed", callable_mp(this, &ShowEngine::_onInterfaceListChanged));
	//g_app.connect("slot_list_changed", callable_mp(this, &ShowEngine::_onSlotListChanged));
	//g_app.connect("launch_list_changed", callable_mp(this, &ShowEngine::_onLaunchListChanged));
	//g_app.connect("starting", callable_mp(this, &ShowEngine::_onStarting));
	//g_app.connect("running", callable_mp(this, &ShowEngine::_onRunning));
	//g_app.connect("paused", callable_mp(this, &ShowEngine::_onPaused));

	createNewShow();
	return OK;
}

void ShowEngine::createNewShow()
{
	stop();
	clear();

	m_filepath.clear();
	//emit_signal("file_path_changed");

	//newInterface("metronome");
	//newInterface("hotkey");
	//newInterface("launch");
	//newInterface("pipe");
	emit_signal("interface_list_changed");
}

RID ShowEngine::addSlot(String name)
{
	String finalname = name;
	if (finalname != "")
	{
		m_last_name = name;
		m_last_name_index = 0;
	}
	else
		finalname = m_last_name + "_" + String::num(++m_last_name_index, 0);

	DigishowSlot newslot(finalname);
	RID ret = m_slotsOwner.make_rid(newslot);
	m_slots.insert(ret);

	emit_signal("slot_list_changed");

	return ret;
}

void ShowEngine::removeSlot(RID p_rid)
{
	if (!m_slotsOwner.owns(p_rid))
		return;
	
	auto* slot = m_slotsOwner.get_or_null(p_rid);
	if (!slot)
		return;

	m_slotsOwner.free(p_rid);
	m_slots.erase(p_rid);
	//memdelete(slot);


	emit_signal("slot_list_changed");

	/*if (idx >= 0 && idx < m_slotsOwner.size()) {
		DigishowSlot* slot = m_slotsOwner[idx];
		m_slotsOwner.erase(slot);
		delete slot;
		return true;
	}
	return false;*/
}

void ShowEngine::removeSlotIdx(const int idx)
{
	if (idx < 0)
		return;

	const RID p_rid = m_slots[idx];
	removeSlot(p_rid);
}


bool ShowEngine::start()
{
	if (m_running && m_paused)
	{
		pause(); // toggles
		return true;
	}
	if (m_running)
		return -1;
	
	// request privacy access permission
	for (int n = 0; n < m_interfaces.size(); n++)
	{
		auto interface = m_interfaceOwner.get_or_null(m_interfaces[n]);
		if (interface->interfaceOptions()->operator[]("type") == "audioin")
		{
			// TMP
			//if (AppUtilities::requestAccessMicrophone()) {
			//    messageNotify(tr("Requesting access to your microphone ..."), MsgToast);
			return -2;
			//}
		}
	}

	// starting
	m_starting = true;
	emit_signal("starting");

	int hasError = 0; // interface opening error count

	// open all interface    
	for (int n = 0; n < m_interfaces.size(); n++)
	{
		auto interface = m_interfaceOwner.get_or_null(m_interfaces[n]);
		// close the interface if it is already opened
		if (interface->isInterfaceOpened())
			interface->closeInterface();

		// repair endpoints of the interface (disable endpoints that are not employed by any slot)
		for (int i = 0; i < interface->endpointCount(); i++) {
			if (!confirmEndpointIsEmployedIndex(n, i))
				interface->setEndpointOption(i, "enabled", false);
		}

		// open the interface
		int err = interface->openInterface();
		if (err == ERR_NONE) {

			const auto options = (*interface->interfaceOptions());
			const auto info = (*interface->interfaceInfo());
		
		}
		else {

			print_line(tr("Error occurred when open interface %1 .") + interface->interfaceInfo()->label);
			hasError++;
		}
	}

	// enable all slots
	for (int n = 0; n < m_slots.size(); n++) {

		auto slot = m_slotsOwner.get_or_null(m_slots[n]);
		if (!slot)
			continue;

		int err = slot->setEnabled(true);
		if (err == ERR_NONE) {

			dgsEndpointInfo srcEpInfo = (*slot->sourceInterface()->endpointInfoList())[slot->sourceEndpointIndex()];
			dgsEndpointInfo dstEpInfo = (*slot->destinationInterface()->endpointInfoList())[slot->destinationEndpointIndex()];
		}
	}

	// started
	m_starting = false;
	emit_signal("starting");

	m_running = true;
	emit_signal("running");

	// initialize all interfaces
	for (int n = 0; n < m_interfaces.size(); n++)
	{
		const auto rid = m_interfaces[n];
		auto* interface = m_interfaceOwner.get_or_null(rid);
		interface->init();
	}

	return hasError;
}

void ShowEngine::pause()
{
	// link or unlink all slots
	for (int n = 0; n < m_slots.size(); n++)
	{
		auto slot = m_slotsOwner.get_or_null(m_slots[n]);
		if (!slot)
			continue;

		slot->setLinked(m_paused);
	}

	m_paused = !m_paused;
	emit_signal("paused");
}

void ShowEngine::stop()
{
	if (!m_running)
		return;

	// disable all slots
	for (int n = 0; n < m_slots.size(); n++)
	{
		auto slot = m_slotsOwner.get_or_null(m_slots[n]);
		if (!slot)
			continue;

		slot->setEnabled(false);
	}

	// close all interface
	for (int n = 0; n < m_interfaces.size(); n++)
	{
		const auto rid = m_interfaces[n];
		auto* interface = m_interfaceOwner.get_or_null(rid);
		interface->closeInterface();
	}

	m_running = false;
	
	emit_signal("running");
}

void ShowEngine::clear()
{
	m_launches.clear();
	emit_signal("launch_list_changed");

	for (int n = 0; n < m_slots.size(); n++)
	{
		const auto rid = m_slots[n];
		auto *slot = m_slotsOwner.get_or_null(rid);
		m_slotsOwner.free(rid);
		m_slots.erase(rid);
		memdelete(slot);
	}
	emit_signal("slot_list_changed");

	for (int n = 0; n < m_interfaces.size(); n++)
	{
		const auto rid = m_interfaces[n];
		auto* interface = m_interfaceOwner.get_or_null(rid);
		interface->closeInterface();
		m_interfaceOwner.free(rid);
		m_interfaces.erase(rid);
		memdelete(interface);
	}	
	emit_signal("interface_list_changed");

	// reset metronome parameters
	m_metronome = nullptr;
}

void ShowEngine::importData(const Dictionary data)
{
	// clear all in the app environment
	clear();

	// set up metronome
	m_metronome->setParameters(data["metronome"]);

	// set up interfaces
	Array dataInterfaces = data["interfaces"];
	for (int n = 0; n < dataInterfaces.size(); n++)
	{
		// extract options of the interface and all included endpoints
		Dictionary dataInterface = dataInterfaces[n];
		Array dataEndpoints = dataInterface["endpoints"];
		dataInterface.erase("endpoints");

		// repair media list
		if (dataInterface.find_key("media"))
		{
			Array dataMediaList = dataInterface["media"];
			for (int i = 0; i < dataMediaList.size(); i++)
			{
				Dictionary dataMedia = dataMediaList[i];
				String url = dataMedia["url"];
				String file = dataMedia["file"];
				// TMP
				/*if (validateFilePath(file))
				{
					//if (url.is_empty() || (QUrl(url).isLocalFile() && !validateFileUrl(url))) {
					dataMedia["url"] = convertFilePathToUrl(file);
					//}
				}*/
				dataMediaList[i] = dataMedia;
			}
			dataInterface["media"] = dataMediaList;
		}

		// set up interface object
		String interfaceType = dataInterface["type"];
		DigishowInterface* interface;

		// note:
		// all interface objects created here must be set the parent for keeping the ownership
		// to avoid it released in qml accidently !!!

		if (interfaceType == "midi")			interface = new DgsMidiInterface();
		else if (interfaceType == "rioc")		interface = new DgsRiocInterface();
		else if (interfaceType == "modbus")		interface = new DgsModbusInterface();
		else if (interfaceType == "hue")		interface = new DgsHueInterface();
		else if (interfaceType == "dmx")		interface = new DgsDmxInterface();
		else if (interfaceType == "artnet")		interface = new DgsArtnetInterface();
		else if (interfaceType == "osc")		interface = new DgsOscInterface();
		else if (interfaceType == "audioin")	interface = new DgsAudioinInterface();
		else if (interfaceType == "screen")		interface = new DgsScreenInterface();
		else if (interfaceType == "pipe")		interface = new DgsPipeInterface();
		else if (interfaceType == "launch")		interface = new DgsLaunchInterface();
		else if (interfaceType == "hotkey")		interface = new DgsHotkeyInterface();
		else if (interfaceType == "metronome")	interface = new DgsMetronomeInterface(m_metronome.get());
		#ifdef DIGISHOW_EXPERIMENTAL
		else if (interfaceType == "aplay") interface = new DgsAPlayInterface();
		else if (interfaceType == "mplay") interface = new DgsMPlayInterface();
		#endif
		else									interface = new DigishowInterface();

		interface->setInterfaceOptions(dataInterface);
		for (int m = 0; m < dataEndpoints.size(); m++) {
			Dictionary dataEndpoint = dataEndpoints[m];
			interface->addEndpoint(dataEndpoint);
		}

		// build metadata after all options have been set
		interface->updateMetadata();

		// apply to the app environment
		RID ret = m_interfaceOwner.make_rid(*interface);
		m_interfaces.insert(ret);
	}

	// set up slots
	Array dataSlots = data["slots"];
	for (int n = 0; n < dataSlots.size(); n++)
	{

		// extract slot options
		Dictionary dataSlot = dataSlots[n];

		String src = dataSlot["source"];
		String name = dataSlot["name"];
		String dst = dataSlot["destination"];

		int  srcInterfaceIndex = -1;
		int  srcEndpointIndex = findInterfaceAndEndpointIndex(src, srcInterfaceIndex);

		int  dstInterfaceIndex = -1;
		int  dstEndpointIndex = findInterfaceAndEndpointIndex(dst, dstInterfaceIndex);

		// set up slot object

		// note:
		// all slot objects created here must be set the parent for keeping the ownership
		// to avoid it released in qml accidently !!!

		DigishowSlot newslot(name);

		auto interface_obj = m_interfaceOwner.get_or_null(m_interfaces[srcInterfaceIndex]);
		newslot.setSource(interface_obj, srcEndpointIndex);

		interface_obj = m_interfaceOwner.get_or_null(m_interfaces[dstInterfaceIndex]);
		newslot.setDestination(interface_obj, dstEndpointIndex);
		newslot.setSlotOptions(dataSlot);

		// set linked flag
		if (dataSlot.find_key("linked"))
			newslot.setLinked(dataSlot["linked"]);
		else
			newslot.setLinked(true);

		// apply to the app environment
		RID ret = m_slotsOwner.make_rid(newslot);
		m_slots.insert(ret);
	}

	// set up launches
	if (data.find_key("launches"))
		m_launches = data["launches"];

	// notify ui data change
	emit_signal("interface_list_changed");
	emit_signal("slot_list_changed");
	emit_signal("launch_list_changed");
}

Dictionary ShowEngine::exportData(Array slotListOrder, bool onlySelection)
{
	// prepare data of interfaces
	Array dataInterfaces;
	for (int n = 0; n < m_interfaces.size(); n++)
	{
		auto interface_obj = m_interfaceOwner.get_or_null(m_interfaces[n]);
		Dictionary dataInterface = (*interface_obj->interfaceOptions());

		// repair endpoint list
		List<Dictionary> dataEndpointsList = (*interface_obj->endpointOptionsList());
		Array dataEndpoints;
		for (int i = 0; i < dataEndpointsList.size(); i++)
		{
			if (confirmEndpointIsEmployedIndex(n, i))
				dataEndpoints.append(dataEndpointsList[i]); // remove the endpoint that is not employed
		}
		dataInterface["endpoints"] = dataEndpoints;

		// repair media list
		if (dataInterface.find_key("media"))
		{
			Array dataMediaList = dataInterface["media"];
			for (int i = 0; i < dataMediaList.size(); i++)
			{
				Dictionary dataMedia = dataMediaList[i];
				// TMP                                    ... this here.
				dataMedia["file"] = dataMedia["url"]; //convertFileUrlToPath(dataMedia["url"]);
				dataMediaList[i] = dataMedia;
			}
			dataInterface["media"] = dataMediaList;
		}

		dataInterfaces.append(dataInterface);
	}

	// prepare data of slots
	Array dataSlots;
	bool isCustomizedOrder = (!slotListOrder.is_empty() && slotListOrder.size() == m_slots.size());
	for (int n = 0; n < m_slots.size(); n++)
	{
		const int idx = isCustomizedOrder ? slotListOrder[n] : n;
		DigishowSlot* slot = m_slotsOwner.get_or_null(m_slots[idx]);
		if (!slot)
		{
			assert(false);
			continue;
		}

		Dictionary dataSlot = Dictionary(*slot->slotOptions());

		dataSlot["linked"] = slot->isLinked(); // save linked flag

		if (!onlySelection || slot->isSelected())
			dataSlots.append(dataSlot);
	}

	// write data
	Dictionary data;
	data["interfaces"] = dataInterfaces;
	data["slots"] = dataSlots;
	data["launches"] = m_launches;

	// Metronome
	data["metronome"] = m_metronome->getParameters();

	return data;
}
/*
bool ShowEngine::loadFile(String filepath)
{
	// confirm the system is not running
	if (m_running)
		return false;

	// confirm the file exists
	if (!FileAccess::exists(filepath))
		return false;

	// load data from the file
	Error err;
	auto filestring = FileAccess::get_file_as_string(filepath, &err);

	if (err) {
		return false;
	}

	Dictionary data = JSON::parse_string(filestring);
	if (!data.find_key("interfaces") || !data.find_key("slots"))
		return false;

	m_filepath = filepath;

	// process the data
	importData(data);

	emit_signal("file_path_changed");

	// auto start
	if (m_autostart)
		start();

	return true;
}

bool ShowEngine::saveFile(String filepath, Array slotListOrder, bool onlySelection)
{
	// confirm filepath is set
	String filepath1 = (filepath.is_empty() ? m_filepath : filepath);
	if (filepath1.is_empty())
		return false;

	// prepare the data
	Dictionary data = exportData(slotListOrder, onlySelection);

	// app version info
	Dictionary info;
	info["appName"] = DigishowEnvironment::appName() + (DigishowEnvironment::appExperimental() ? " +" : "");
	info["appVersion"] = DigishowEnvironment::appVersion();
	data["appInfo"] = info;

	// save data to file

	Error err;
	Ref<FileAccess> file = FileAccess::open(filepath1, FileAccess::WRITE, &err);

	if (err)
	{
		return err;
	}

	const auto jsonstring = JSON::stringify(info, "", true, true);

	file->store_string(jsonstring);

	m_filepath = filepath1;
	emit_signal("file_path_changed");
	return true;

}
*/

void ShowEngine::enableAutostart(bool autostart)
{
	m_autostart = autostart;
}

RID ShowEngine::newInterface(const String interfaceType)
{
	
	// create a new interface object
	DigishowInterface* interface;

	if (interfaceType == "midi") {

		interface = new DgsMidiInterface();
		interface->setInterfaceOption("mode", "output");
		interface->setInterfaceOption("outputInterval", 20);

	}
	else if (interfaceType == "rioc") {

		interface = new DgsRiocInterface();
		interface->setInterfaceOption("mode", "general");
		interface->setInterfaceOption("outputInterval", 20);

	}
	else if (interfaceType == "modbus") {

		interface = new DgsModbusInterface();
		interface->setInterfaceOption("mode", "rtu");
		interface->setInterfaceOption("comBaud", 9600);
		interface->setInterfaceOption("comParity", "8N1");
		interface->setInterfaceOption("outputInterval", 20);

	}
	else if (interfaceType == "hue") {

		interface = new DgsHueInterface();
		interface->setInterfaceOption("mode", "");
		interface->setInterfaceOption("outputInterval", 150);

	}
	else if (interfaceType == "dmx") {

		interface = new DgsDmxInterface();
		interface->setInterfaceOption("mode", "enttec");
		interface->setInterfaceOption("outputInterval", 20);

	}
	else if (interfaceType == "osc") {

		interface = new DgsOscInterface();
		interface->setInterfaceOption("mode", "input");
		interface->setInterfaceOption("udpPort", 8000);

	}
	else if (interfaceType == "artnet") {

		interface = new DgsArtnetInterface();
		interface->setInterfaceOption("mode", "input");
		interface->setInterfaceOption("udpPort", 6454);

	}
	else if (interfaceType == "pipe") {

		interface = new DgsPipeInterface();
		interface->setInterfaceOption("mode", "local");

	}
	else if (interfaceType == "launch") {

		interface = new DgsLaunchInterface();
		interface->setInterfaceOption("mode", "");

	}
	else if (interfaceType == "hotkey") {

		interface = new DgsHotkeyInterface();
		interface->setInterfaceOption("mode", "");

	}
	else if (interfaceType == "metronome") {

		interface = new DgsMetronomeInterface(m_metronome.get());
		interface->setInterfaceOption("mode", "");

	}
	else if (interfaceType == "audioin") {

		interface = new DgsAudioinInterface();
		interface->setInterfaceOption("mode", "");

	}
	else if (interfaceType == "camera") {

		interface = new DigishowInterface();
		//interface = new DgsCameraInterface();
		//interface->setInterfaceOption("mode", "");

	}
	else if (interfaceType == "screen") {

		interface = new DgsScreenInterface();
		interface->setInterfaceOption("mode", "");

		// each screen interface must have a player endpoint
		Dictionary endpointOptions;
		endpointOptions["type"] = "player";
		endpointOptions["name"] = "player1";
		interface->addEndpoint(endpointOptions);

#ifdef DIGISHOW_EXPERIMENTAL

	}
	else if (interfaceType == "aplay") {

		interface = new DgsAPlayInterface();
		interface->setInterfaceOption("mode", "");

	}
	else if (interfaceType == "mplay") {

		interface = new DgsMPlayInterface();
		interface->setInterfaceOption("mode", "");
#endif

	}
	else
	{

		interface = new DigishowInterface();
	}

	// assign a name to the new interface
	if (m_interfaces.size() > 0)
	{
		int interfaceidx = 0;
		{
			String name = interfaceType + String::num(interfaceidx);
			bool nameExisted = false;
			for (int i = 0; i < m_interfaces.size(); i++)
			{
				auto _interface = m_interfaceOwner.get_or_null(m_interfaces[i]);
				if (!_interface)
					continue;

				const auto options = _interface->getInterfaceOptions();
				if (options["name"] == name)
				{
					name = interfaceType + String::num(interfaceidx);
				}
			}
			interface->setInterfaceOption("name", name);
			
		}
	}

	interface->updateMetadata();

	RID ret = m_interfaceOwner.make_rid(*interface);
	m_interfaces.insert(ret);

	return ret; // return new interface index
}
bool ShowEngine::deleteInterface(RID interfaceIndex)
{
	return false;
}

int ShowEngine::duplicateSlot(int slotIndex)
{
	return false;
}

String ShowEngine::filepath()
{
	return m_filepath;
}
bool ShowEngine::isStarting()
{
	return m_starting;
}
bool ShowEngine::isRunning()
{
	return m_running;
}
bool ShowEngine::isPaused()
{
	return m_paused;
}

int ShowEngine::interfaceCount()
{
	return m_interfaces.size();
}
Dictionary ShowEngine::interfaceInfo(int index)
{
	if (index >= 0 && index < m_interfaces.size())
	{
		auto interface_obj = m_interfaceOwner.get_or_null(m_interfaces[index]);
		return interface_obj->getInterfaceInfo();
	}

	return {};
}
Array ShowEngine::interfaceInfoList()
{
	Array result;
	for (auto i = 0; i < m_interfaces.size(); ++i)
	{
		auto interface_obj = m_interfaceOwner.get_or_null(m_interfaces[i]);
		result.push_back(interface_obj->getInterfaceInfo());
	}
	return result;
}

int ShowEngine::slotCount()
{
	return m_slots.size();
}
/*Ref<DigishowSlot> ShowEngine::slotAt(int p_index)
{
	return m_slots[p_index];
}*/

Dictionary ShowEngine::getSlotInfo(const int p_idx)
{
	if (auto slot = m_slotsOwner.get_or_null(m_slots[p_idx]))
	{
		Dictionary slotinfo = slot->getSlotInfo();
		// for some reason set_self is not stored
		slotinfo["rid"] = m_slots[p_idx];
		return slotinfo;
	}

	return Dictionary();
}

Dictionary ShowEngine::getSlotInfo(const RID p_rid)
{
	if (auto slot = m_slotsOwner.get_or_null(p_rid))
	{
		Dictionary slotinfo = slot->getSlotInfo();
		// for some reason set_self is not stored
		slotinfo["rid"] = p_rid;
		return slotinfo;
	}

	return Dictionary();
}

void ShowEngine::setSlotEnabled(const RID p_rid, const bool enabled)
{
	if (auto slot = m_slotsOwner.get_or_null(p_rid))
		slot->setEnabled(enabled);
}

const bool ShowEngine::getSlotEnabled(const RID p_rid)
{
	if (auto slot = m_slotsOwner.get_or_null(p_rid))
		return slot->isEnabled();

	return false;
}

Array ShowEngine::getSlotList()
{
	Array result;
	for (auto i = 0; i < m_slots.size(); ++i)
	{
		if (auto slot = m_slotsOwner.get_or_null(m_slots[i]))
		{
			Dictionary slotinfo = slot->getSlotInfo();
			// for some reason set_self is not stored
			slotinfo["rid"] = m_slots[i];
			result.append(slotinfo);
		}
	}
	return result;
}

bool ShowEngine::updateLaunch(String launchName, Array slotLaunchOptions)
{ // clear launch item details if it already exists
	Dictionary launchOptions = getLaunchOptions(launchName);
	deleteLaunch(launchName);
	launchOptions["assigned"] = true;
	m_launches[launchName] = launchOptions;

	// update launch item details
	if (slotLaunchOptions.size() != m_slots.size())
		return false;

	for (int n = 0; n < m_slots.size(); n++)
	{
		DigishowSlot* slot = m_slotsOwner.get_or_null(m_slots[n]);
		if (!slot)
		{
			assert(false);
			continue;
		}

		// make launch detail for the slot
		Dictionary launchDetail;
		Dictionary _sln = slotLaunchOptions[n];

		bool optRememberLink = _sln["rememberLink"];
		if (optRememberLink)
			launchDetail["linked"] = slot->isLinked();

		bool optRememberOutput = _sln["rememberOutput"];
		if (optRememberOutput)
			launchDetail["outputValue"] = slot->getEndpointOutValue();

		// write launch details into the slot
		Dictionary launchDetails;
		if (slot->slotOptions()->find_key("launchDetails"))
			launchDetails = (*slot->slotOptions())["launchDetails"];

		launchDetails[launchName] = launchDetail;
		slot->setSlotOption("launchDetails", launchDetails);
	}

	return true;
}
bool ShowEngine::deleteLaunch(String launchName)
{// delete the launch item with the specific name
	m_launches.erase(launchName);

	// delete the launch item details stored in slots
	for (int n = 0; n < m_slots.size(); n++)
	{
		DigishowSlot* slot = m_slotsOwner.get_or_null(m_slots[n]);
		if (!slot)
		{
			assert(false);
			continue;
		}
		if (slot->slotOptions()->find_key("launchDetails")) {
			Dictionary launchDetails = (*slot->slotOptions())["launchDetails"];
			launchDetails.erase(launchName);
			slot->setSlotOption("launchDetails", launchDetails);
		}
	}

	return true;
}
bool ShowEngine::startLaunch(String launchName)
{
	if (!m_launches.find_key(launchName))
		return false;

	for (int n = 0; n < m_slots.size(); n++)
	{
		DigishowSlot* slot = m_slotsOwner.get_or_null(m_slots[n]);
		if (!slot)
		{
			assert(false);
			continue;
		}

		if (slot->slotOptions()->find_key("launchDetails"))
		{
			Dictionary launchDetails = (*slot->slotOptions())["launchDetails"];
			if (launchDetails.find_key(launchName))
			{
				Dictionary launchDetail = launchDetails[launchName];
				if (launchDetail.find_key("linked"))
					slot->setLinked(launchDetail["linked"]);
				if (launchDetail.find_key("outputValue"))
					slot->setEndpointOutValue(launchDetail["outputValue"]);
			}
		}
	}

	return true;
}
Dictionary ShowEngine::getLaunchOptions(String launchName)
{
	if (!m_launches.find_key(launchName))
		return Dictionary();

	return m_launches[launchName];
}
bool ShowEngine::setLaunchOption(String launchName, String optionName, Variant optionValue)
{
	Dictionary launch;
	if (m_launches.find_key(launchName))
		launch = m_launches[launchName];

	launch[optionName] = optionValue;
	m_launches[launchName] = launch;

	return true;
}
Array ShowEngine::getSlotLaunchDetails(String launchName)
{
	Array listDetails;

	if (!m_launches.find_key(launchName))
		return listDetails;

	for (int n = 0; n < m_slots.size(); n++)
	{
		DigishowSlot* slot = m_slotsOwner.get_or_null(m_slots[n]);
		if (!slot)
		{
			assert(false);
			continue;
		}

		Dictionary details;
		if (slot->slotOptions()->find_key("launchDetails"))
		{
			Dictionary launchDetails = (*slot->slotOptions())["launchDetails"];
			if (launchDetails.find_key(launchName))
				details = launchDetails[launchName];
		}
		listDetails.append(details);
	}

	return listDetails;
}

Array ShowEngine::getSlotLaunchOptions(String launchName)
{
	Array listDetails = getSlotLaunchDetails(launchName);
	Array listOptions;

	if (listDetails.size() == 0)
		return listOptions;

	for (int n = 0; n < listDetails.size(); n++)
	{
		Dictionary launchDetails = listDetails[n];
		Dictionary launchOptions;
		launchOptions["rememberLink"] = launchDetails.find_key("linked");
		launchOptions["rememberOutput"] = launchDetails.find_key("outputValue");
		listOptions.append(launchOptions);
	}
	return listOptions;
}

int ShowEngine::findInterfaceAndEndpointIndex(String name, int pInterfaceIndex)
{
	int result = 0;

	PackedStringArray parts = name.rsplit("/");
	if (parts.size() != 2) return false;

	String interfaceName = parts[0];
	String endpointName = parts[1];

	// find interface by name
	for (int n = 0; n < m_interfaces.size(); n++)
	{
		auto interface_obj = m_interfaceOwner.get_or_null(m_interfaces[n]);
		Dictionary interfaceOptions = *(interface_obj->interfaceOptions());
		if (interfaceOptions["name"] == interfaceName) {

			result = n;

			// find endpoint by name
			List<Dictionary> endpointOptionsList = *(interface_obj->endpointOptionsList());
			for (int m = 0; m < endpointOptionsList.size(); m++) {

				Dictionary endpointOptions = endpointOptionsList[m];
				if (endpointOptions["name"] == endpointName) {

					result = m;
					return result;
				}
			}
			print_line(name + " Interface Index " + String::num(pInterfaceIndex, 0) + " coul not be found!");
			assert(false);
			return 0;
		}
	}

	print_line(name + " Interface Index " + String::num(pInterfaceIndex, 0) + " coul not be found!");
	assert(false);
	return 0;
}
bool ShowEngine::confirmEndpointIsEmployedIndex(int interfaceIndex, int endpointIndex)
{
	if (interfaceIndex < 0 && interfaceIndex >= m_interfaces.size())
		return false;

	auto interface_obj = m_interfaceOwner.get_or_null(m_interfaces[interfaceIndex]);
	DigishowInterface* interface = interface_obj;

	if (endpointIndex < 0 && endpointIndex >= interface->endpointCount())
		return false;

	String interfaceName = (*interface->interfaceOptions())["name"];
	String endpointName = (*interface->endpointOptionsList())[endpointIndex]["name"];

	return confirmEndpointIsEmployedName(interfaceName, endpointName);
}

bool ShowEngine::confirmEndpointIsEmployedName(String interfaceName, String endpointName)
{
	String name = interfaceName + "/" + endpointName;

	for (int n = 0; n < m_slots.size(); n++)
	{
		DigishowSlot* slot = m_slotsOwner.get_or_null(m_slots[n]);
		if (!slot)
		{
			assert(false);
			continue;
		}
		const auto slotoptions = (*slot->slotOptions());
		String src = slotoptions["source"];
		String dst = slotoptions["destination"];
		if (src == name || dst == name)
			return true;
	}
	return false;
}


// =====================================================================
void ShowEngine::_onInterfaceListChanged()
{
	Array interfacelist;
	const auto ilist = m_interfaces;

	for (auto i = 0; i < m_interfaces.size(); ++i)
	{
		auto interface_obj = m_interfaceOwner.get_or_null(m_interfaces[i]);
		interfacelist.push_back(interface_obj->getInterfaceInfo());
	}
	emit_signal("interface_list_changed");
}

void ShowEngine::_onSlotListChanged()
{
	emit_signal("slot_list_changed");
}

void ShowEngine::_onLaunchListChanged()
{
	emit_signal("interface_list_changed");
}

void ShowEngine::_onStarting()
{
	emit_signal("starting");
}

void ShowEngine::_onRunning()
{
	emit_signal("running");
}

void ShowEngine::_onPaused()
{
	emit_signal("paused");
}
