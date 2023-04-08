#pragma once
#ifndef SHOWENGINE_H
#define SHOWENGINE_H

#include <core/object/object.h>
#include <core/os/thread.h>
#include <core/os/mutex.h>
#include <core/templates/list.h>
#include <core/templates/rid.h>
#include <core/templates/rid_owner.h>
#include <core/variant/variant.h>
#include <core/templates/list.h>
#include <scene/gui/item_list.h>
#include <scene/main/timer.h>
#include "digishow/DigiShow/digishow_common.h"
#include "digishow/DigiShow/digishow_interface.h"
#include "digishow/DigiShow/dgs_metronome_interface.h"
#include "digishow/DigiShow/digishow_slot.h"
#include <core/templates/vset.h>
#include <modules/gdscript/gdscript.h>
#include <scene/animation/animation_player.h>
#include "ShowNode.h"

class ShowEditor;

// ===========================================================================================================================
class ShowInterface : public ShowNode
{
	GDCLASS(ShowInterface, ShowNode);
public:
	enum SocketType
	{
		INPUT,
		OUTPUT,
		IN_OUT
	};
	ShowInterface();
	ShowInterface(const SocketType p_input_only);
	~ShowInterface();


	enum InterfaceType
	{
		// controller interface type id
		INTERFACE_TYPE_NONE = 0,
		INTERFACE_TYPE_MIDI = INTERFACE_MIDI,
		INTERFACE_TYPE_ARDUINO = INTERFACE_RIOC,
		INTERFACE_TYPE_MODBUS_ROBOTICS = INTERFACE_MODBUS,
		INTERFACE_TYPE_SMARTLIGHT = INTERFACE_HUE,
		INTERFACE_TYPE_DMX_USB = INTERFACE_DMX,
		INTERFACE_TYPE_DMX_IP = INTERFACE_ARTNET,
		INTERFACE_TYPE_OSC = INTERFACE_OSC,
		INTERFACE_TYPE_AUDIOIN = INTERFACE_AUDIOIN,
		INTERFACE_TYPE_SCREEN = INTERFACE_SCREEN,
		INTERFACE_TYPE_APLAY = INTERFACE_APLAY,
		INTERFACE_TYPE_MPLAY = INTERFACE_MPLAY,
		INTERFACE_TYPE_REMOTE = INTERFACE_PIPE,
		INTERFACE_TYPE_LAUNCH = INTERFACE_LAUNCH,
		INTERFACE_TYPE_INPUT_EVENT = INTERFACE_HOTKEY,
		INTERFACE_TYPE_METRONOME = INTERFACE_METRONOME
	};

	void _notification(int notification);
	void _setEnabled(bool p_enabled);
	bool _getEnabled();
protected:
	Ref<DigishowInterface> m_interface;
	static void _bind_methods();
private:
	const SocketType m_input_option;
};
VARIANT_ENUM_CAST(ShowInterface::InterfaceType)

// ===========================================================================================================================
class AudioShowInterface : public ShowInterface
{
	GDCLASS(AudioShowInterface, ShowInterface);
public:
	AudioShowInterface();
	~AudioShowInterface();
protected:
	static void _bind_methods();
};

// ===========================================================================================================================
class MidiShowInterface : public ShowInterface
{
	GDCLASS(MidiShowInterface, ShowInterface);
public:
	MidiShowInterface();
	~MidiShowInterface();
protected:
	static void _bind_methods();
};

// ===========================================================================================================================
class InputEventShowInterface : public ShowInterface
{
	GDCLASS(InputEventShowInterface, ShowInterface);
public:
	InputEventShowInterface();
	~InputEventShowInterface();
protected:
	static void _bind_methods();
}; 

// ===========================================================================================================================
class DmxUsbShowInterface : public ShowInterface
{
	GDCLASS(DmxUsbShowInterface, ShowInterface);
public:
	DmxUsbShowInterface();
	~DmxUsbShowInterface();
protected:
	static void _bind_methods();
};

// ===========================================================================================================================
class DmxIPShowInterface : public ShowInterface
{
	GDCLASS(DmxIPShowInterface, ShowInterface);
public:
	DmxIPShowInterface();
	~DmxIPShowInterface();
protected:
	static void _bind_methods();
};

// ===========================================================================================================================
class OSCShowInterface : public ShowInterface
{
	GDCLASS(OSCShowInterface, ShowInterface);
public:
	OSCShowInterface();
	~OSCShowInterface();
protected:
	static void _bind_methods();
};

// ===========================================================================================================================
class SmartlightShowInterface : public ShowInterface
{
	GDCLASS(SmartlightShowInterface, ShowInterface);
public:
	SmartlightShowInterface();
	~SmartlightShowInterface();
protected:
	static void _bind_methods();
};

// ===========================================================================================================================
class MetronomeShowInterface : public ShowInterface
{
	GDCLASS(MetronomeShowInterface, ShowInterface);
public:
	MetronomeShowInterface();
	~MetronomeShowInterface();
protected:
	static void _bind_methods();
};

// ===========================================================================================================================
class RemoteShowInterface : public ShowInterface
{
	GDCLASS(RemoteShowInterface, ShowInterface);
public:
	RemoteShowInterface();
	~RemoteShowInterface();
protected:
	static void _bind_methods();
};

// ===========================================================================================================================
class CameraShowInterface : public ShowInterface
{
	GDCLASS(CameraShowInterface, ShowInterface);
public:
	CameraShowInterface();
	~CameraShowInterface();
protected:
	static void _bind_methods();
};

// ===========================================================================================================================
class ScreenShowInterface : public ShowInterface
{
	GDCLASS(ScreenShowInterface, ShowInterface);
public:
	ScreenShowInterface();
	~ScreenShowInterface();
protected:
	static void _bind_methods();
};

// ===========================================================================================================================
class ArduinoShowInterface : public ShowInterface
{
	GDCLASS(ArduinoShowInterface, ShowInterface);
public:
	ArduinoShowInterface();
	~ArduinoShowInterface();
protected:
	static void _bind_methods();
};

// ===========================================================================================================================
class RaspberryShowInterface : public ShowInterface
{
	GDCLASS(RaspberryShowInterface, ShowInterface);
public:
	RaspberryShowInterface();
	~RaspberryShowInterface();
protected:
	static void _bind_methods();
};

// ===========================================================================================================================
class ModbusShowInterface : public ShowInterface
{
	GDCLASS(ModbusShowInterface, ShowInterface);
public:
	ModbusShowInterface();
	~ModbusShowInterface();
protected:
	static void _bind_methods();
};


// ===========================================================================================================================
class NodeShowSlot : public ShowNode
{
	GDCLASS(NodeShowSlot, ShowNode);

public:
	NodeShowSlot();
	~NodeShowSlot();

	void _notification(int notification);
	void _setEnabled(bool p_enabled);
	const bool _getEnabled();
	void _setInputInterfaceType(ShowInterface::InterfaceType p_interface_type);
	void _setOutputInterfaceType(ShowInterface::InterfaceType p_interface_type);
	void _setScript(Ref<GDScript> p_script);
	void _setAnimationPlayer(NodePath p_animation_player);
	void _setAnimation(String animation);

	ShowInterface::InterfaceType _getInputInterfaceType();
	ShowInterface::InterfaceType _getOutputInterfaceType();
	Ref<GDScript> _getScript();
	NodePath _getAnimationPlayer();
	String _getAnimationName();
protected:
	static void _bind_methods();

	void _validate_property(PropertyInfo& p_property) const;
	void _get_property_list(List<PropertyInfo>* p_list) const;
private:

	//HashMap<StringName, Ref<Animation>> animation_player_set;
	RID m_slot;
	Ref<GDScript> m_script;
	NodePath m_animationPlayer;
	String m_animation;
};

// ===========================================================================================================================
class ShowEngine : public Object
{
	enum MsgType {
		MsgLog = 0,
		MsgAlert = 1,
		MsgToast = 2,
		MsgPopup = 3
	};

	GDCLASS(ShowEngine, Object);

	static ShowEngine* singleton;

	friend class ShowEditor;
private:
	bool thread_exited;
	mutable bool exit_thread;
	Thread* thread;
	Mutex* mutex;

	String m_filepath;

	VSet<RID> m_interfaces;
	Array m_interfaces_dictionaries;
	//List<DigishowSlot*> m_slots;
	Array m_slots_dictionaries;

	Dictionary m_launches;

	bool m_autostart;
	bool m_starting;
	bool m_running;
	bool m_paused;

	uint64_t counter;
	RID_Owner<DigishowSlot> m_slotsOwner;
	RID_Owner<DigishowInterface> m_interfaceOwner;
	VSet<RID> m_slots;

	std::unique_ptr<Timer> m_timer;
	std::unique_ptr<DigishowMetronome> m_metronome;

	void _onInterfaceListChanged();
	void _onSlotListChanged();
	void _onLaunchListChanged();
	void _onStarting();
	void _onRunning();
	void _onPaused();

protected:


	static void _bind_methods();

	void createNewShow();

	void importData(Dictionary data);
	Dictionary exportData(Array slotListOrder, bool onlySelection = false);

	String m_last_name = "Connection";
	int m_last_name_index = 0;
public:


	enum IconType
	{
		SHOW_ICONTYPE_ARTNET,
		SHOW_ICONTYPE_DMX,
		SHOW_ICONTYPE_MICROPHONE,
		SHOW_ICONTYPE_MIDI,
		SHOW_ICONTYPE_MDDBUS,
		SHOW_ICONTYPE_REMOTE,
		SHOW_ICONTYPE_RIOC,
		SHOW_ICONTYPE_SCREEN,
		SHOW_ICONTYPE_SMARTLIGHT
	};

	void create_icon(const String& icon_path, IconType icon_type);
	HashMap<IconType, Ref<Texture2D>> icons;

	Error init();

	void clear();

	bool start();
	void pause();
	void stop();

	// returns RID of new slot
	RID addSlot(String name = "");
	// returns count of slots
	void removeSlot(const RID rid);
	void removeSlotIdx(const int idx);



	void enableAutostart(bool autostart = true);

	RID newInterface(String interfaceType);
	bool deleteInterface(RID interfaceIndex);

	int duplicateSlot(int slotIndex);

	String filepath();
	bool isStarting();
	bool isRunning();
	bool isPaused();

	int interfaceCount();
	Dictionary interfaceInfo(int index);
	Array interfaceInfoList();

	int slotCount();

	bool updateLaunch(String launchName, Array slotLaunchOptions);
	bool deleteLaunch(String launchName);
	bool startLaunch(String launchName);
	Dictionary getLaunchOptions(String launchName);
	bool setLaunchOption(String launchName, String optionName, Variant optionValue);
	Array getSlotLaunchDetails(String launchName);
	Array getSlotLaunchOptions(String launchName);

	int findInterfaceAndEndpointIndex(String name, int pInterfaceIndex = -1);
	bool confirmEndpointIsEmployedIndex(int interfaceIndex, int endpointIndex);
	bool confirmEndpointIsEmployedName(String interfaceName, String endpointName);

	//Ref<DigishowSlot> slotAt(int index);
	Dictionary getSlotInfo(const int idx);
	Dictionary getSlotInfo(const RID rid);
	void setSlotEnabled(const RID p_rid, const bool enabled);
	const bool getSlotEnabled(const RID p_rid);
	Array getSlotList();

	static ShowEngine* get_singleton();



	ShowEngine();
	~ShowEngine();

};

#endif // SHOWENGINE_H
