//------------------------------------------------------------------------
// Copyright(c) 2023 Deadline Entertainment.
//------------------------------------------------------------------------

#include "mgscontroller.h"
#include "mgscids.h"

#include <main/main.h>

using namespace Steinberg;

//------------------------------------------------------------------------
// MetroGayaSystemVSTController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API MetroGayaSystemVSTController::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated

	//---do not forget to call parent ------
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk)
	{
		return result;
	}

	// Here you could register some parameters
	int size = 0;
	host_executable_path = get_host_executable_path(size);

	char args[2];
	args[0] = *get_host_executable_path(size);
	args[1] = "--display-driver";
	args[2] = "headless";

	setlocale(LC_CTYPE, "");

	//TEST_MAIN_PARAM_OVERRIDE(argc, argv_utf8)

	Error err = Main::setup(&args[0], 0, "", false);

	if (err != OK) {
		if (err == ERR_HELP) { // Returned by --help and --version, so success.
			return result;
		}
		return kInternalError;
	}

	return kInternalError;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MetroGayaSystemVSTController::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!
	Main::cleanup();

	//---do not forget to call parent ------
	return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API MetroGayaSystemVSTController::setComponentState (IBStream* state)
{
	// Here you get the state of the component (Processor part)
	if (!state)
		return kResultFalse;

	Error err = Main::setup2();
	ERR_FAIL_COND(err != OK)

	if (Main::start())
		return kResultOk;

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MetroGayaSystemVSTController::setState (IBStream* state)
{
	// Here you get the state of the controller

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MetroGayaSystemVSTController::getState (IBStream* state)
{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor

	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API MetroGayaSystemVSTController::createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor))
	{
		// create your editor here and return a IPlugView ptr of it
        return nullptr;
	}
	return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MetroGayaSystemVSTController::setParamNormalized (Vst::ParamID tag, Vst::ParamValue value)
{
	// called by host to update your parameters
	tresult result = EditControllerEx1::setParamNormalized (tag, value);
	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MetroGayaSystemVSTController::getParamStringByValue (Vst::ParamID tag, Vst::ParamValue valueNormalized, Vst::String128 string)
{
	// called by host to get a string for given normalized value of a specific parameter
	// (without having to set the value!)
	return EditControllerEx1::getParamStringByValue (tag, valueNormalized, string);
}

//------------------------------------------------------------------------
tresult PLUGIN_API MetroGayaSystemVSTController::getParamValueByString (Vst::ParamID tag, Vst::TChar* string, Vst::ParamValue& valueNormalized)
{
	// called by host to get a normalized value from a string representation of a specific parameter
	// (without having to set the value!)
	return EditControllerEx1::getParamValueByString (tag, string, valueNormalized);
}

