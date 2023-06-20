/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#ifndef VST3_HEADERS_H
#define VST3_HEADERS_H
#include "juce/CompilerWarnings.h"

#if JUCE_BSD && ! JUCE_CUSTOM_VST3_SDK
#error To build JUCE VST3 plug-ins on BSD you must use an external BSD-compatible VST3 SDK with JUCE_CUSTOM_VST3_SDK=1
#endif

// Wow, those Steinberg guys really don't worry too much about compiler warnings.
JUCE_BEGIN_IGNORE_WARNINGS_LEVEL_MSVC(0, 4505 4702 6011 6031 6221 6386 6387 6330 6001 28199)

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE("-Wdeprecated-copy-dtor",
	"-Wnon-virtual-dtor",
	"-Wdeprecated",
	"-Wreorder",
	"-Wunsequenced",
	"-Wint-to-pointer-cast",
	"-Wunused-parameter",
	"-Wconversion",
	"-Woverloaded-virtual",
	"-Wshadow",
	"-Wdeprecated-register",
	"-Wunused-function",
	"-Wsign-conversion",
	"-Wsign-compare",
	"-Wdelete-non-virtual-dtor",
	"-Wdeprecated-declarations",
	"-Wextra-semi",
	"-Wmissing-braces",
	"-Wswitch-default",
	"-Wshadow-field",
	"-Wpragma-pack",
	"-Wcomma",
	"-Wzero-as-null-pointer-constant",
	"-Winconsistent-missing-destructor-override",
	"-Wcast-align",
	"-Wignored-qualifiers",
	"-Wmissing-field-initializers",
	"-Wformat=",
	"-Wformat",
	"-Wpedantic",
	"-Wextra",
	"-Wclass-memaccess",
	"-Wmissing-prototypes",
	"-Wtype-limits",
	"-Wcpp",
	"-W#warnings",
	"-Wmaybe-uninitialized",
	"-Wunused-but-set-variable")

#undef DEVELOPMENT
#define DEVELOPMENT 0  // This avoids a Clang warning in Steinberg code about unused values

	/*  These files come with the Steinberg VST3 SDK - to get them, you'll need to
		visit the Steinberg website and agree to whatever is currently required to
		get them.

		Then, you'll need to make sure your include path contains your "VST3 SDK"
		directory (or whatever you've named it on your machine). The Projucer has
		a special box for setting this path.
	*/
#if JUCE_VST3HEADERS_INCLUDE_HEADERS_ONLY
#include "vst/base/source/fstring.h"
#include "vst/pluginterfaces/base/conststringtable.h"
#include "vst/pluginterfaces/base/funknown.h"
#include "vst/pluginterfaces/base/ipluginbase.h"
#include "vst/pluginterfaces/base/ustring.h"
#include "vst/pluginterfaces/gui/iplugview.h"
#include "vst/pluginterfaces/gui/iplugviewcontentscalesupport.h"
#include "vst/pluginterfaces/vst/ivstattributes.h"
#include "vst/pluginterfaces/vst/ivstaudioprocessor.h"
#include "vst/pluginterfaces/vst/ivstcomponent.h"
#include "vst/pluginterfaces/vst/ivstcontextmenu.h"
#include "vst/pluginterfaces/vst/ivsteditcontroller.h"
#include "vst/pluginterfaces/vst/ivstevents.h"
#include "vst/pluginterfaces/vst/ivsthostapplication.h"
#include "vst/pluginterfaces/vst/ivstmessage.h"
#include "vst/pluginterfaces/vst/ivstmidicontrollers.h"
#include "vst/pluginterfaces/vst/ivstparameterchanges.h"
#include "vst/pluginterfaces/vst/ivstplugview.h"
#include "vst/pluginterfaces/vst/ivstprocesscontext.h"
#include "vst/pluginterfaces/vst/vsttypes.h"
#include "vst/pluginterfaces/vst/ivstunits.h"
#include "vst/pluginterfaces/vst/ivstmidicontrollers.h"
#include "vst/pluginterfaces/vst/ivstchannelcontextinfo.h"
#include "vst/public.sdk/source/common/memorystream.h"
#include "vst/public.sdk/source/vst/vsteditcontroller.h"
#include "vst/public.sdk/source/vst/vstpresetfile.h"

#include "vst/pslextensions/ipslviewembedding.h"
#else
	// needed for VST_VERSION
#include "vst/pluginterfaces/vst/vsttypes.h"

#include "vst/base/source/baseiids.cpp"
#include "vst/base/source/fbuffer.cpp"
#include "vst/base/source/fdebug.cpp"
#include "vst/base/source/fobject.cpp"
#include "vst/base/source/fstreamer.cpp"
#include "vst/base/source/fstring.cpp"

// The following shouldn't leak from fstring.cpp
#undef stricmp
#undef strnicmp
#undef snprintf
#undef vsnprintf
#undef snwprintf
#undef vsnwprintf

#if VST_VERSION >= 0x030608
#include "vst/base/thread/source/flock.cpp"
#include "vst/pluginterfaces/base/coreiids.cpp"
#else
#include "vst/base/source/flock.cpp"
#endif

#include "vst/base/source/updatehandler.cpp"
#include "vst/pluginterfaces/base/conststringtable.cpp"
#include "vst/pluginterfaces/base/funknown.cpp"
#include "vst/pluginterfaces/base/ipluginbase.h"
#include "vst/pluginterfaces/base/ustring.cpp"
#include "vst/pluginterfaces/gui/iplugview.h"
#include "vst/pluginterfaces/gui/iplugviewcontentscalesupport.h"
#include "vst/pluginterfaces/vst/ivstmidicontrollers.h"
#include "vst/pluginterfaces/vst/ivstchannelcontextinfo.h"
#include "vst/public.sdk/source/common/memorystream.cpp"
#include "vst/public.sdk/source/common/pluginview.cpp"
#include "vst/public.sdk/source/vst/vsteditcontroller.cpp"
#include "vst/public.sdk/source/vst/vstbus.cpp"
#include "vst/public.sdk/source/vst/vstinitiids.cpp"
#include "vst/public.sdk/source/vst/vstcomponent.cpp"
#include "vst/public.sdk/source/vst/vstcomponentbase.cpp"
#include "vst/public.sdk/source/vst/vstparameters.cpp"
#include "vst/public.sdk/source/vst/vstpresetfile.cpp"
#include "vst/public.sdk/source/vst/hosting/hostclasses.cpp"

#if VST_VERSION >= 0x03060c   // 3.6.12
#include "vst/public.sdk/source/vst/hosting/pluginterfacesupport.cpp"
#endif

#include "vst/pslextensions/ipslviewembedding.h"

//==============================================================================
namespace Steinberg
{
	/** Missing IIDs */
#if VST_VERSION < 0x03060d   // 3.6.13
	DEF_CLASS_IID(IPluginBase)
		DEF_CLASS_IID(IPluginFactory)
		DEF_CLASS_IID(IPluginFactory2)
		DEF_CLASS_IID(IPluginFactory3)
#if VST_VERSION < 0x030608
		DEF_CLASS_IID(IBStream)
#endif
#endif
		DEF_CLASS_IID(IPlugView)
		DEF_CLASS_IID(IPlugFrame)
		DEF_CLASS_IID(IPlugViewContentScaleSupport)

#if JUCE_LINUX || JUCE_BSD
		DEF_CLASS_IID(Linux::IRunLoop)
		DEF_CLASS_IID(Linux::IEventHandler)
#endif
}

namespace Presonus
{
	DEF_CLASS_IID(IPlugInViewEmbedding)
}

#endif // JUCE_VST3HEADERS_INCLUDE_HEADERS_ONLY

JUCE_END_IGNORE_WARNINGS_MSVC
JUCE_END_IGNORE_WARNINGS_GCC_LIKE

#if JUCE_WINDOWS
#include <windows.h>
#endif

//==============================================================================
#undef ASSERT
#undef WARNING
#undef PRINTSYSERROR
#undef DEBUGSTR
#undef DBPRT0
#undef DBPRT1
#undef DBPRT2
#undef DBPRT3
#undef DBPRT4
#undef DBPRT5
#undef min
#undef max
#undef MIN
#undef MAX
#undef calloc
#undef free
#undef malloc
#undef realloc
#undef NEW
#undef NEWVEC
#undef VERIFY
#undef VERIFY_IS
#undef VERIFY_NOT
#undef META_CREATE_FUNC
#undef CLASS_CREATE_FUNC
#undef SINGLE_CREATE_FUNC
#undef _META_CLASS
#undef _META_CLASS_IFACE
#undef _META_CLASS_SINGLE
#undef META_CLASS
#undef META_CLASS_IFACE
#undef META_CLASS_SINGLE
#undef SINGLETON
#undef OBJ_METHODS
#undef QUERY_INTERFACE
#undef LICENCE_UID
#undef BEGIN_FACTORY
#undef DEF_CLASS
#undef DEF_CLASS1
#undef DEF_CLASS2
#undef DEF_CLASS_W
#undef END_FACTORY

#ifdef atomic_thread_fence
#undef atomic_thread_fence
#endif

#endif // VST3_HEADERS_H
