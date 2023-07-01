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

#pragma once
#include <modules/deadline_audio_engine/juce/juce_AudioSampleBuffer.h>
#include <modules/deadline_audio_engine/juce/juce_AudioChannelSet.h>
#include <modules/deadline_audio_engine/juce/juce_BigInteger.h>
#include <modules/deadline_audio_engine/juce/juce_MathsFunctions.h>

#include <algorithm>
#include <set>
#include <iterator>

// Wow, those Steinberg guys really don't worry too much about compiler warnings.
#pragma warning(disable:0 4505 4702 6011 6031 6221 6386 6387 6330 6001 28199)

#if __GCC__
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
#endif
#undef DEVELOPMENT
#define DEVELOPMENT 0  // This avoids a Clang warning in Steinberg code about unused values

	/*  These files come with the Steinberg VST3 SDK - to get them, you'll need to
		visit the Steinberg website and agree to whatever is currently required to
		get them.

		Then, you'll need to make sure your include path contains your "VST3 SDK"
		directory (or whatever you've named it on your machine). The Projucer has
		a special box for setting this path.
	*/
#if INCLUDE_HEADERS_ONLY
#include <base/source/fstring.h>
#include <pluginterfaces/base/conststringtable.h>
#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/base/ipluginbase.h>
#include <pluginterfaces/base/ustring.h>
#include <pluginterfaces/gui/iplugview.h>
#include <pluginterfaces/gui/iplugviewcontentscalesupport.h>
#include <pluginterfaces/vst/ivstattributes.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivstcomponent.h>
#include <pluginterfaces/vst/ivstcontextmenu.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivstevents.h>
#include <pluginterfaces/vst/ivsthostapplication.h>
#include <pluginterfaces/vst/ivstmessage.h>
#include <pluginterfaces/vst/ivstmidicontrollers.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <pluginterfaces/vst/ivstplugview.h>
#include <pluginterfaces/vst/ivstprocesscontext.h>
#include <pluginterfaces/vst/vsttypes.h>
#include <pluginterfaces/vst/ivstunits.h>
#include <pluginterfaces/vst/ivstmidicontrollers.h>
#include <pluginterfaces/vst/ivstchannelcontextinfo.h>
#include <public.sdk/source/common/memorystream.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <public.sdk/source/vst/vstpresetfile.h>

#include "pslextensions/ipslviewembedding.h"
#else
	// needed for VST_VERSION
#include <thirdparty/vst3sdk/pluginterfaces/vst/vsttypes.h>

#include <thirdparty/vst3sdk/base/source/baseiids.cpp>
#include <thirdparty/vst3sdk/base/source/fbuffer.cpp>
#include <thirdparty/vst3sdk/base/source/fdebug.cpp>
#include <thirdparty/vst3sdk/base/source/fobject.cpp>
#include <thirdparty/vst3sdk/base/source/fstreamer.cpp>
#include <thirdparty/vst3sdk/base/source/fstring.cpp>

// The following shouldn't leak from fstring.cpp
#undef stricmp
#undef strnicmp
#undef snprintf
#undef vsnprintf
#undef snwprintf
#undef vsnwprintf

#if VST_VERSION >= 0x030608
#include <thirdparty/vst3sdk/base/thread/source/flock.cpp>
#include <thirdparty/vst3sdk/pluginterfaces/base/coreiids.cpp>
#else
#include <thirdparty/vst3sdk/base/source/flock.cpp>
#endif

#include <thirdparty/vst3sdk/base/source/updatehandler.cpp>
#include <thirdparty/vst3sdk/pluginterfaces/base/conststringtable.cpp>
#include <thirdparty/vst3sdk/pluginterfaces/base/funknown.cpp>
#include <thirdparty/vst3sdk/pluginterfaces/base/ipluginbase.h>
#include <thirdparty/vst3sdk/pluginterfaces/base/ustring.cpp>
#include <thirdparty/vst3sdk/pluginterfaces/gui/iplugview.h>
#include <thirdparty/vst3sdk/pluginterfaces/gui/iplugviewcontentscalesupport.h>
#include <thirdparty/vst3sdk/pluginterfaces/vst/ivstaudioprocessor.h>
#include <thirdparty/vst3sdk/pluginterfaces/vst/ivstchannelcontextinfo.h>
#include <thirdparty/vst3sdk/pluginterfaces/vst/ivstmidicontrollers.h>
#include <thirdparty/vst3sdk/public.sdk/source/common/memorystream.cpp>
#include <thirdparty/vst3sdk/public.sdk/source/common/pluginview.cpp>
#include <thirdparty/vst3sdk/public.sdk/source/vst/vsteditcontroller.cpp>
#include <thirdparty/vst3sdk/public.sdk/source/vst/vstbus.cpp>
#include <thirdparty/vst3sdk/public.sdk/source/vst/vstinitiids.cpp>
#include <thirdparty/vst3sdk/public.sdk/source/vst/vstcomponent.cpp>
#include <thirdparty/vst3sdk/public.sdk/source/vst/vstcomponentbase.cpp>
#include <thirdparty/vst3sdk/public.sdk/source/vst/vstparameters.cpp>
#include <thirdparty/vst3sdk/public.sdk/source/vst/vstpresetfile.cpp>
#include <thirdparty/vst3sdk/public.sdk/source/vst/hosting/hostclasses.cpp>

#if VST_VERSION >= 0x03060c   // 3.6.12
#include <thirdparty/vst3sdk/public.sdk/source/vst/hosting/pluginterfacesupport.cpp>
#endif

#include "pslextensions/ipslviewembedding.h"

#include <core/string/ustring.h>

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

//JUCE_END_IGNORE_WARNINGS_MSVC
//JUCE_END_IGNORE_WARNINGS_GCC_LIKE

#if WINDOWS_ENABLED
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


//==============================================================================
#define JUCE_DECLARE_VST3_COM_REF_METHODS \
    Steinberg::uint32 PLUGIN_API addRef() override   { return (Steinberg::uint32) ++refCount; } \
    Steinberg::uint32 PLUGIN_API release() override  { const int r = --refCount; if (r == 0) delete this; return (Steinberg::uint32) r; }

#define JUCE_DECLARE_VST3_COM_QUERY_METHODS \
    Steinberg::tresult PLUGIN_API queryInterface (const Steinberg::TUID, void** obj) override \
    { \
        jassertfalse; \
        *obj = nullptr; \
        return Steinberg::kNotImplemented; \
    }

		inline bool doUIDsMatch(const Steinberg::TUID a, const Steinberg::TUID b) noexcept
	{
		return std::memcmp(a, b, sizeof(Steinberg::TUID)) == 0;
	}

	/*  Holds a tresult and a pointer to an object.

		Useful for holding intermediate results of calls to queryInterface.
	*/
	class QueryInterfaceResult
	{
	public:
		QueryInterfaceResult() = default;

		QueryInterfaceResult(Steinberg::tresult resultIn, void* ptrIn)
			: result(resultIn), ptr(ptrIn) {}

		bool isOk() const noexcept { return result == Steinberg::kResultOk; }

		Steinberg::tresult extract(void** obj) const
		{
			*obj = result == Steinberg::kResultOk ? ptr : nullptr;
			return result;
		}

	private:
		Steinberg::tresult result = Steinberg::kResultFalse;
		void* ptr = nullptr;
	};

	/*  Holds a tresult and a pointer to an object.

		Calling InterfaceResultWithDeferredAddRef::extract() will also call addRef
		on the pointed-to object. It is expected that users will use
		InterfaceResultWithDeferredAddRef to hold intermediate results of a queryInterface
		call. When a suitable interface is found, the function can be exited with
		`return suitableInterface.extract (obj)`, which will set the obj pointer,
		add a reference to the interface, and return the appropriate result code.
	*/
	class InterfaceResultWithDeferredAddRef
	{
	public:
		InterfaceResultWithDeferredAddRef() = default;

		template <typename Ptr>
		InterfaceResultWithDeferredAddRef(Steinberg::tresult resultIn, Ptr* ptrIn)
			: result(resultIn, ptrIn),
			addRefFn(doAddRef<Ptr>) {}

		bool isOk() const noexcept { return result.isOk(); }

		Steinberg::tresult extract(void** obj) const
		{
			const auto toReturn = result.extract(obj);

			if (result.isOk() && addRefFn != nullptr && *obj != nullptr)
				addRefFn(*obj);

			return toReturn;
		}

	private:
		template <typename Ptr>
		static void doAddRef(void* obj) { static_cast<Ptr*> (obj)->addRef(); }

		QueryInterfaceResult result;
		void (*addRefFn) (void*) = nullptr;
	};

	template <typename ClassType>                                   struct UniqueBase {};
	template <typename CommonClassType, typename SourceClassType>   struct SharedBase {};

	template <typename ToTest, typename CommonClassType, typename SourceClassType>
	InterfaceResultWithDeferredAddRef testFor(ToTest& toTest,
		const Steinberg::TUID targetIID,
		SharedBase<CommonClassType, SourceClassType>)
	{
		if (!doUIDsMatch(targetIID, CommonClassType::iid))
			return {};

		return { Steinberg::kResultOk, static_cast<CommonClassType*> (static_cast<SourceClassType*> (std::addressof(toTest))) };
	}

	template <typename ToTest, typename ClassType>
	InterfaceResultWithDeferredAddRef testFor(ToTest& toTest,
		const Steinberg::TUID targetIID,
		UniqueBase<ClassType>)
	{
		return testFor(toTest, targetIID, SharedBase<ClassType, ClassType>{});
	}

	template <typename ToTest>
	InterfaceResultWithDeferredAddRef testForMultiple(ToTest&, const Steinberg::TUID) { return {}; }

	template <typename ToTest, typename Head, typename... Tail>
	InterfaceResultWithDeferredAddRef testForMultiple(ToTest& toTest, const Steinberg::TUID targetIID, Head head, Tail... tail)
	{
		const auto result = testFor(toTest, targetIID, head);

		if (result.isOk())
			return result;

		return testForMultiple(toTest, targetIID, tail...);
	}

	//==============================================================================
#if VST_VERSION < 0x030608
#define kAmbi1stOrderACN kBFormat
#endif

//==============================================================================
	inline String toString(const Steinberg::char8* string) noexcept { return String((const char*)string); }
	inline String toString(const Steinberg::char16* string) noexcept { return String((const char*)string); }

	// NB: The casts are handled by a Steinberg::UString operator
	inline String toString(const Steinberg::UString128& string) noexcept { return toString(static_cast<const Steinberg::char16*> (string)); }
	inline String toString(const Steinberg::UString256& string) noexcept { return toString(static_cast<const Steinberg::char16*> (string)); }

	inline Steinberg::Vst::TChar* toString(const String& source) noexcept { return reinterpret_cast<Steinberg::Vst::TChar*> (source.to_utf16_buffer().ptrw()); }

	inline void toString128(Steinberg::Vst::String128 result, const char* source)
	{
		Steinberg::UString(result, 128).fromAscii(source);
	}

	inline void toString128(Steinberg::Vst::String128 result, const String& source)
	{
		Steinberg::UString(result, 128).assign(source.ascii().get_data(), source.ascii().length());
	}

#if JUCE_WINDOWS
	static const Steinberg::FIDString defaultVST3WindowType = Steinberg::kPlatformTypeHWND;
#elif JUCE_MAC
	static const Steinberg::FIDString defaultVST3WindowType = Steinberg::kPlatformTypeNSView;
#elif JUCE_LINUX || JUCE_BSD
	static const Steinberg::FIDString defaultVST3WindowType = Steinberg::kPlatformTypeX11EmbedWindowID;
#endif


	//==============================================================================
	static inline Steinberg::Vst::SpeakerArrangement getArrangementForBus(Steinberg::Vst::IAudioProcessor* processor, bool isInput, int busIndex)
	{
		Steinberg::Vst::SpeakerArrangement arrangement = Steinberg::Vst::SpeakerArr::kEmpty;

		if (processor != nullptr)
			processor->getBusArrangement(isInput ? Steinberg::Vst::kInput : Steinberg::Vst::kOutput,
				(Steinberg::int32)busIndex, arrangement);

		return arrangement;
	}

	static Steinberg::Vst::Speaker getSpeakerType(const juce::AudioChannelSet& set, juce::AudioChannelSet::ChannelType type) noexcept
	{
		switch (type)
		{
		case juce::AudioChannelSet::left:              return Steinberg::Vst::kSpeakerL;
		case juce::AudioChannelSet::right:             return Steinberg::Vst::kSpeakerR;
		case juce::AudioChannelSet::centre:            return (set == juce::AudioChannelSet::mono() ? Steinberg::Vst::kSpeakerM : Steinberg::Vst::kSpeakerC);

		case juce::AudioChannelSet::LFE:               return Steinberg::Vst::kSpeakerLfe;
		case juce::AudioChannelSet::leftSurround:      return Steinberg::Vst::kSpeakerLs;
		case juce::AudioChannelSet::rightSurround:     return Steinberg::Vst::kSpeakerRs;
		case juce::AudioChannelSet::leftCentre:        return Steinberg::Vst::kSpeakerLc;
		case juce::AudioChannelSet::rightCentre:       return Steinberg::Vst::kSpeakerRc;
		case juce::AudioChannelSet::centreSurround:    return Steinberg::Vst::kSpeakerCs;
		case juce::AudioChannelSet::leftSurroundSide:  return Steinberg::Vst::kSpeakerSl;
		case juce::AudioChannelSet::rightSurroundSide: return Steinberg::Vst::kSpeakerSr;
		case juce::AudioChannelSet::topMiddle:         return Steinberg::Vst::kSpeakerTc; /* kSpeakerTm */
		case juce::AudioChannelSet::topFrontLeft:      return Steinberg::Vst::kSpeakerTfl;
		case juce::AudioChannelSet::topFrontCentre:    return Steinberg::Vst::kSpeakerTfc;
		case juce::AudioChannelSet::topFrontRight:     return Steinberg::Vst::kSpeakerTfr;
		case juce::AudioChannelSet::topRearLeft:       return Steinberg::Vst::kSpeakerTrl;
		case juce::AudioChannelSet::topRearCentre:     return Steinberg::Vst::kSpeakerTrc;
		case juce::AudioChannelSet::topRearRight:      return Steinberg::Vst::kSpeakerTrr;
		case juce::AudioChannelSet::LFE2:              return Steinberg::Vst::kSpeakerLfe2;
		case juce::AudioChannelSet::leftSurroundRear:  return Steinberg::Vst::kSpeakerLcs;
		case juce::AudioChannelSet::rightSurroundRear: return Steinberg::Vst::kSpeakerRcs;
		case juce::AudioChannelSet::proximityLeft:     return Steinberg::Vst::kSpeakerPl;
		case juce::AudioChannelSet::proximityRight:    return Steinberg::Vst::kSpeakerPr;
		case juce::AudioChannelSet::ambisonicACN0:     return Steinberg::Vst::kSpeakerACN0;
		case juce::AudioChannelSet::ambisonicACN1:     return Steinberg::Vst::kSpeakerACN1;
		case juce::AudioChannelSet::ambisonicACN2:     return Steinberg::Vst::kSpeakerACN2;
		case juce::AudioChannelSet::ambisonicACN3:     return Steinberg::Vst::kSpeakerACN3;
		case juce::AudioChannelSet::ambisonicACN4:     return Steinberg::Vst::kSpeakerACN4;
		case juce::AudioChannelSet::ambisonicACN5:     return Steinberg::Vst::kSpeakerACN5;
		case juce::AudioChannelSet::ambisonicACN6:     return Steinberg::Vst::kSpeakerACN6;
		case juce::AudioChannelSet::ambisonicACN7:     return Steinberg::Vst::kSpeakerACN7;
		case juce::AudioChannelSet::ambisonicACN8:     return Steinberg::Vst::kSpeakerACN8;
		case juce::AudioChannelSet::ambisonicACN9:     return Steinberg::Vst::kSpeakerACN9;
		case juce::AudioChannelSet::ambisonicACN10:    return Steinberg::Vst::kSpeakerACN10;
		case juce::AudioChannelSet::ambisonicACN11:    return Steinberg::Vst::kSpeakerACN11;
		case juce::AudioChannelSet::ambisonicACN12:    return Steinberg::Vst::kSpeakerACN12;
		case juce::AudioChannelSet::ambisonicACN13:    return Steinberg::Vst::kSpeakerACN13;
		case juce::AudioChannelSet::ambisonicACN14:    return Steinberg::Vst::kSpeakerACN14;
		case juce::AudioChannelSet::ambisonicACN15:    return Steinberg::Vst::kSpeakerACN15;
		case juce::AudioChannelSet::topSideLeft:       return Steinberg::Vst::kSpeakerTsl;
		case juce::AudioChannelSet::topSideRight:      return Steinberg::Vst::kSpeakerTsr;
		case juce::AudioChannelSet::bottomFrontLeft:   return Steinberg::Vst::kSpeakerBfl;
		case juce::AudioChannelSet::bottomFrontCentre: return Steinberg::Vst::kSpeakerBfc;
		case juce::AudioChannelSet::bottomFrontRight:  return Steinberg::Vst::kSpeakerBfr;
		case juce::AudioChannelSet::bottomSideLeft:    return Steinberg::Vst::kSpeakerBsl;
		case juce::AudioChannelSet::bottomSideRight:   return Steinberg::Vst::kSpeakerBsr;
		case juce::AudioChannelSet::bottomRearLeft:    return Steinberg::Vst::kSpeakerBrl;
		case juce::AudioChannelSet::bottomRearCentre:  return Steinberg::Vst::kSpeakerBrc;
		case juce::AudioChannelSet::bottomRearRight:   return Steinberg::Vst::kSpeakerBrr;

		case juce::AudioChannelSet::discreteChannel0:  return Steinberg::Vst::kSpeakerM;

		case juce::AudioChannelSet::ambisonicACN16:
		case juce::AudioChannelSet::ambisonicACN17:
		case juce::AudioChannelSet::ambisonicACN18:
		case juce::AudioChannelSet::ambisonicACN19:
		case juce::AudioChannelSet::ambisonicACN20:
		case juce::AudioChannelSet::ambisonicACN21:
		case juce::AudioChannelSet::ambisonicACN22:
		case juce::AudioChannelSet::ambisonicACN23:
		case juce::AudioChannelSet::ambisonicACN24:
		case juce::AudioChannelSet::ambisonicACN25:
		case juce::AudioChannelSet::ambisonicACN26:
		case juce::AudioChannelSet::ambisonicACN27:
		case juce::AudioChannelSet::ambisonicACN28:
		case juce::AudioChannelSet::ambisonicACN29:
		case juce::AudioChannelSet::ambisonicACN30:
		case juce::AudioChannelSet::ambisonicACN31:
		case juce::AudioChannelSet::ambisonicACN32:
		case juce::AudioChannelSet::ambisonicACN33:
		case juce::AudioChannelSet::ambisonicACN34:
		case juce::AudioChannelSet::ambisonicACN35:
		case juce::AudioChannelSet::wideLeft:
		case juce::AudioChannelSet::wideRight:
		case juce::AudioChannelSet::unknown:
			break;
		}

		auto channelIndex = static_cast<Steinberg::Vst::Speaker> (type) - (static_cast<Steinberg::Vst::Speaker> (juce::AudioChannelSet::discreteChannel0) + 6ull);
		return (1ull << (channelIndex + 33ull /* last speaker in vst layout + 1 */));
	}

	static juce::AudioChannelSet::ChannelType getChannelType(Steinberg::Vst::SpeakerArrangement arr, Steinberg::Vst::Speaker type) noexcept
	{
		switch (type)
		{
		case Steinberg::Vst::kSpeakerL:     return juce::AudioChannelSet::left;
		case Steinberg::Vst::kSpeakerR:     return juce::AudioChannelSet::right;
		case Steinberg::Vst::kSpeakerC:     return juce::AudioChannelSet::centre;
		case Steinberg::Vst::kSpeakerLfe:   return juce::AudioChannelSet::LFE;
		case Steinberg::Vst::kSpeakerLs:    return juce::AudioChannelSet::leftSurround;
		case Steinberg::Vst::kSpeakerRs:    return juce::AudioChannelSet::rightSurround;
		case Steinberg::Vst::kSpeakerLc:    return juce::AudioChannelSet::leftCentre;
		case Steinberg::Vst::kSpeakerRc:    return juce::AudioChannelSet::rightCentre;
		case Steinberg::Vst::kSpeakerCs:    return juce::AudioChannelSet::centreSurround;
		case Steinberg::Vst::kSpeakerSl:    return juce::AudioChannelSet::leftSurroundSide;
		case Steinberg::Vst::kSpeakerSr:    return juce::AudioChannelSet::rightSurroundSide;
		case Steinberg::Vst::kSpeakerTc:    return juce::AudioChannelSet::topMiddle;  /* kSpeakerTm */
		case Steinberg::Vst::kSpeakerTfl:   return juce::AudioChannelSet::topFrontLeft;
		case Steinberg::Vst::kSpeakerTfc:   return juce::AudioChannelSet::topFrontCentre;
		case Steinberg::Vst::kSpeakerTfr:   return juce::AudioChannelSet::topFrontRight;
		case Steinberg::Vst::kSpeakerTrl:   return juce::AudioChannelSet::topRearLeft;
		case Steinberg::Vst::kSpeakerTrc:   return juce::AudioChannelSet::topRearCentre;
		case Steinberg::Vst::kSpeakerTrr:   return juce::AudioChannelSet::topRearRight;
		case Steinberg::Vst::kSpeakerLfe2:  return juce::AudioChannelSet::LFE2;
		case Steinberg::Vst::kSpeakerM:     return ((arr & Steinberg::Vst::kSpeakerC) != 0 ? juce::AudioChannelSet::discreteChannel0 : juce::AudioChannelSet::centre);
		case Steinberg::Vst::kSpeakerACN0:  return juce::AudioChannelSet::ambisonicACN0;
		case Steinberg::Vst::kSpeakerACN1:  return juce::AudioChannelSet::ambisonicACN1;
		case Steinberg::Vst::kSpeakerACN2:  return juce::AudioChannelSet::ambisonicACN2;
		case Steinberg::Vst::kSpeakerACN3:  return juce::AudioChannelSet::ambisonicACN3;
		case Steinberg::Vst::kSpeakerACN4:  return juce::AudioChannelSet::ambisonicACN4;
		case Steinberg::Vst::kSpeakerACN5:  return juce::AudioChannelSet::ambisonicACN5;
		case Steinberg::Vst::kSpeakerACN6:  return juce::AudioChannelSet::ambisonicACN6;
		case Steinberg::Vst::kSpeakerACN7:  return juce::AudioChannelSet::ambisonicACN7;
		case Steinberg::Vst::kSpeakerACN8:  return juce::AudioChannelSet::ambisonicACN8;
		case Steinberg::Vst::kSpeakerACN9:  return juce::AudioChannelSet::ambisonicACN9;
		case Steinberg::Vst::kSpeakerACN10: return juce::AudioChannelSet::ambisonicACN10;
		case Steinberg::Vst::kSpeakerACN11: return juce::AudioChannelSet::ambisonicACN11;
		case Steinberg::Vst::kSpeakerACN12: return juce::AudioChannelSet::ambisonicACN12;
		case Steinberg::Vst::kSpeakerACN13: return juce::AudioChannelSet::ambisonicACN13;
		case Steinberg::Vst::kSpeakerACN14: return juce::AudioChannelSet::ambisonicACN14;
		case Steinberg::Vst::kSpeakerACN15: return juce::AudioChannelSet::ambisonicACN15;
		case Steinberg::Vst::kSpeakerTsl:   return juce::AudioChannelSet::topSideLeft;
		case Steinberg::Vst::kSpeakerTsr:   return juce::AudioChannelSet::topSideRight;
		case Steinberg::Vst::kSpeakerLcs:   return juce::AudioChannelSet::leftSurroundRear;
		case Steinberg::Vst::kSpeakerRcs:   return juce::AudioChannelSet::rightSurroundRear;
		case Steinberg::Vst::kSpeakerBfl:   return juce::AudioChannelSet::bottomFrontLeft;
		case Steinberg::Vst::kSpeakerBfc:   return juce::AudioChannelSet::bottomFrontCentre;
		case Steinberg::Vst::kSpeakerBfr:   return juce::AudioChannelSet::bottomFrontRight;
		case Steinberg::Vst::kSpeakerPl:    return juce::AudioChannelSet::proximityLeft;
		case Steinberg::Vst::kSpeakerPr:    return juce::AudioChannelSet::proximityRight;
		case Steinberg::Vst::kSpeakerBsl:   return juce::AudioChannelSet::bottomSideLeft;
		case Steinberg::Vst::kSpeakerBsr:   return juce::AudioChannelSet::bottomSideRight;
		case Steinberg::Vst::kSpeakerBrl:   return juce::AudioChannelSet::bottomRearLeft;
		case Steinberg::Vst::kSpeakerBrc:   return juce::AudioChannelSet::bottomRearCentre;
		case Steinberg::Vst::kSpeakerBrr:   return juce::AudioChannelSet::bottomRearRight;
		}

		auto channelType = juce::BigInteger(static_cast<int64> (type)).findNextSetBit(0);

		// VST3 <-> JUCE layout conversion error: report this bug to the JUCE forum
		jassert(channelType >= 33);

		return static_cast<juce::AudioChannelSet::ChannelType> (static_cast<int> (juce::AudioChannelSet::discreteChannel0) + 6 + (channelType - 33));
	}

	namespace detail
	{
		struct LayoutPair
		{
			Steinberg::Vst::SpeakerArrangement arrangement;
			std::initializer_list<juce::AudioChannelSet::ChannelType> channelOrder;
		};

		using namespace Steinberg::Vst::SpeakerArr;
		using X = juce::AudioChannelSet;

		/*  Maps VST3 layouts to the equivalent JUCE channels, in VST3 order.

			The channel types are taken from the equivalent JUCE juce::AudioChannelSet, and then reordered to
			match the VST3 speaker positions.
		*/
		const LayoutPair layoutTable[]
		{
			{ kEmpty,                       {} },
			{ kMono,                        { X::centre } },
			{ kStereo,                      { X::left, X::right } },
			{ k30Cine,                      { X::left, X::right, X::centre } },
			{ k30Music,                     { X::left, X::right, X::surround } },
			{ k40Cine,                      { X::left, X::right, X::centre, X::surround } },
			{ k50,                          { X::left, X::right, X::centre, X::leftSurround, X::rightSurround } },
			{ k51,                          { X::left, X::right, X::centre, X::LFE, X::leftSurround, X::rightSurround } },
			{ k60Cine,                      { X::left, X::right, X::centre, X::leftSurround, X::rightSurround, X::centreSurround } },
			{ k61Cine,                      { X::left, X::right, X::centre, X::LFE, X::leftSurround, X::rightSurround, X::centreSurround } },
			{ k60Music,                     { X::left, X::right, X::leftSurround, X::rightSurround, X::leftSurroundSide, X::rightSurroundSide } },
			{ k61Music,                     { X::left, X::right, X::LFE, X::leftSurround, X::rightSurround, X::leftSurroundSide, X::rightSurroundSide } },
			{ k70Music,                     { X::left, X::right, X::centre, X::leftSurroundRear, X::rightSurroundRear, X::leftSurroundSide, X::rightSurroundSide } },
			{ k70Cine,                      { X::left, X::right, X::centre, X::leftSurround, X::rightSurround, X::leftCentre, X::rightCentre } },
			{ k71Music,                     { X::left, X::right, X::centre, X::LFE, X::leftSurroundRear, X::rightSurroundRear, X::leftSurroundSide, X::rightSurroundSide } },
			{ k71Cine,                      { X::left, X::right, X::centre, X::LFE, X::leftSurround, X::rightSurround, X::leftCentre, X::rightCentre } },
			{ k40Music,                     { X::left, X::right, X::leftSurround, X::rightSurround } },

			{ k51_4,                        { X::left, X::right, X::centre, X::LFE, X::leftSurround, X::rightSurround, X::topFrontLeft, X::topFrontRight, X::topRearLeft, X::topRearRight } },
			{ k50_4,                        { X::left, X::right, X::centre,         X::leftSurround, X::rightSurround, X::topFrontLeft, X::topFrontRight, X::topRearLeft, X::topRearRight } },
			{ k71_2,                        { X::left, X::right, X::centre, X::LFE, X::leftSurroundRear, X::rightSurroundRear, X::leftSurroundSide, X::rightSurroundSide, X::topSideLeft, X::topSideRight } },
			{ k70_2,                        { X::left, X::right, X::centre,         X::leftSurroundRear, X::rightSurroundRear, X::leftSurroundSide, X::rightSurroundSide, X::topSideLeft, X::topSideRight } },
			{ k71_4,                        { X::left, X::right, X::centre, X::LFE, X::leftSurroundRear, X::rightSurroundRear, X::leftSurroundSide, X::rightSurroundSide, X::topFrontLeft, X::topFrontRight, X::topRearLeft, X::topRearRight } },
			{ k70_4,                        { X::left, X::right, X::centre,         X::leftSurroundRear, X::rightSurroundRear, X::leftSurroundSide, X::rightSurroundSide, X::topFrontLeft, X::topFrontRight, X::topRearLeft, X::topRearRight } },
			{ k71_6,                        { X::left, X::right, X::centre, X::LFE, X::leftSurroundRear, X::rightSurroundRear, X::leftSurroundSide, X::rightSurroundSide, X::topFrontLeft, X::topFrontRight, X::topRearLeft, X::topRearRight, X::topSideLeft, X::topSideRight } },
			{ k70_6,                        { X::left, X::right, X::centre,         X::leftSurroundRear, X::rightSurroundRear, X::leftSurroundSide, X::rightSurroundSide, X::topFrontLeft, X::topFrontRight, X::topRearLeft, X::topRearRight, X::topSideLeft, X::topSideRight } },

			// The VST3 layout uses 'left/right' and 'left-of-center/right-of-center', but the JUCE layout uses 'left/right' and 'wide-left/wide-right'.
			{ k91_6,                        { X::wideLeft, X::wideRight, X::centre, X::LFE, X::leftSurroundRear, X::rightSurroundRear, X::left, X::right, X::leftSurroundSide, X::rightSurroundSide, X::topFrontLeft, X::topFrontRight, X::topRearLeft, X::topRearRight, X::topSideLeft, X::topSideRight } },
			{ k90_6,                        { X::wideLeft, X::wideRight, X::centre,         X::leftSurroundRear, X::rightSurroundRear, X::left, X::right, X::leftSurroundSide, X::rightSurroundSide, X::topFrontLeft, X::topFrontRight, X::topRearLeft, X::topRearRight, X::topSideLeft, X::topSideRight } },
		};

#if JUCE_DEBUG
		static std::once_flag layoutTableCheckedFlag;
#endif
	}

	inline bool isLayoutTableValid()
	{
		for (const auto& item : detail::layoutTable)
			if ((size_t)juce::countNumberOfBits(item.arrangement) != item.channelOrder.size())
				return false;

		std::set<Steinberg::Vst::SpeakerArrangement> arrangements;

		for (const auto& item : detail::layoutTable)
			arrangements.insert(item.arrangement);

		if (arrangements.size() != (size_t)juce::numElementsInArray(detail::layoutTable))
			return false; // There's a duplicate speaker arrangement

		return std::all_of(std::begin(detail::layoutTable), std::end(detail::layoutTable), [](const auto& item)
			{
				return std::set<juce::AudioChannelSet::ChannelType>(item.channelOrder).size() == item.channelOrder.size();
			});
	}

	static Vector<juce::AudioChannelSet::ChannelType> getSpeakerOrder(Steinberg::Vst::SpeakerArrangement arr)
	{
		using namespace Steinberg::Vst;
		using namespace Steinberg::Vst::SpeakerArr;

#if JUCE_DEBUG
		std::call_once(detail::layoutTableCheckedFlag, [] { jassert(isLayoutTableValid()); });
#endif

		// Check if this is a layout with a hard-coded conversion
		const auto arrangementMatches = [arr](const auto& layoutPair) { return layoutPair.arrangement == arr; };
		const auto iter = std::find_if(std::begin(detail::layoutTable), std::end(detail::layoutTable), arrangementMatches);

		if (iter != std::end(detail::layoutTable))
			return iter->channelOrder;

		// There's no hard-coded conversion, so assume that the channels are in the same orders in both layouts.
		const auto channels = getChannelCount(arr);
		Vector<juce::AudioChannelSet::ChannelType> result;
		result.resize(channels);

		for (auto i = 0; i < channels; ++i)
			result.append(getChannelType(arr, getSpeaker(arr, i)));

		return result;
	}

	static Steinberg::Vst::SpeakerArrangement getVst3SpeakerArrangement(const juce::AudioChannelSet& channels) noexcept
	{
		using namespace Steinberg::Vst::SpeakerArr;

#if JUCE_DEBUG
		std::call_once(detail::layoutTableCheckedFlag, [] { jassert(isLayoutTableValid()); });
#endif

		const auto channelSetMatches = [&channels](const auto& layoutPair)
		{
			return juce::AudioChannelSet::channelSetWithChannels(layoutPair.channelOrder) == channels;
		};
		const auto iter = std::find_if(std::begin(detail::layoutTable), std::end(detail::layoutTable), channelSetMatches);

		if (iter != std::end(detail::layoutTable))
			return iter->arrangement;

		Steinberg::Vst::SpeakerArrangement result = 0;

		for (const auto& type : channels.getChannelTypes())
			result |= getSpeakerType(channels, type);

		return result;
	}

	inline juce::AudioChannelSet getChannelSetForSpeakerArrangement(Steinberg::Vst::SpeakerArrangement arr) noexcept
	{
		using namespace Steinberg::Vst::SpeakerArr;

		const auto result = juce::AudioChannelSet::channelSetWithChannels(getSpeakerOrder(arr));

		// VST3 <-> JUCE layout conversion error: report this bug to the JUCE forum
		jassert(result.size() == getChannelCount(arr));

		return result;
	}

	//==============================================================================
	/*
		Provides fast remapping of the channels on a single bus, from VST3 order to JUCE order.

		For multi-bus plugins, you'll need several instances of this, one per bus.
	*/
	struct ChannelMapping
	{
		ChannelMapping(const juce::AudioChannelSet& layout, bool activeIn)
			: indices(makeChannelIndices(layout)), active(activeIn) {}

		explicit ChannelMapping(const juce::AudioChannelSet& layout)
			: ChannelMapping(layout, true) {}

		explicit ChannelMapping(const AudioProcessor::Bus& bus)
			: ChannelMapping(bus.getLastEnabledLayout(), bus.isEnabled()) {}

		int getJuceChannelForVst3Channel(int vst3Channel) const { return indices[(size_t)vst3Channel]; }

		size_t size() const { return indices.size(); }

		void setActive(bool x) { active = x; }
		bool isActive() const { return active; }

	private:
		/*  Builds a table that provides the index of the corresponding JUCE channel, given a VST3 channel.

			Depending on the mapping, the VST3 arrangement and JUCE arrangement may not contain channels
			that map 1:1 via getChannelType. For example, the VST3 7.1 layout contains
			'kSpeakerLs' which maps to the 'leftSurround' channel type, but the JUCE 7.1 layout does not
			contain this channel type. As a result, we need to try to map the channels sensibly, even
			if there's not a 'direct' mapping.
		*/
		static std::vector<int> makeChannelIndices(const juce::AudioChannelSet& juceArrangement)
		{
			const auto order = getSpeakerOrder(getVst3SpeakerArrangement(juceArrangement));

			std::vector<int> result;

			for (const auto& type : order)
				result.push_back(juceArrangement.getChannelIndexForType(type));

			return result;
		}

		std::vector<int> indices;
		bool active = true;
	};

	class DynamicChannelMapping
	{
	public:
		DynamicChannelMapping(const juce::AudioChannelSet& channelSet, bool active)
			: set(channelSet), map(channelSet, active) {}

		explicit DynamicChannelMapping(const juce::AudioChannelSet& channelSet)
			: DynamicChannelMapping(channelSet, true) {}

		explicit DynamicChannelMapping(const AudioProcessor::Bus& bus)
			: DynamicChannelMapping(bus.getLastEnabledLayout(), bus.isEnabled()) {}

		juce::AudioChannelSet getAudioChannelSet() const { return set; }
		int getJuceChannelForVst3Channel(int vst3Channel) const { return map.getJuceChannelForVst3Channel(vst3Channel); }
		size_t size() const { return map.size(); }

		/*  Returns true if the host has activated this bus. */
		bool isHostActive()   const { return hostActive; }
		/*  Returns true if the AudioProcessor expects this bus to be active. */
		bool isClientActive() const { return map.isActive(); }

		void setHostActive(bool active) { hostActive = active; }
		void setClientActive(bool active) { map.setActive(active); }

	private:
		juce::AudioChannelSet set;
		ChannelMapping map;
		bool hostActive = false;
	};

	//==============================================================================
	inline auto& getAudioBusPointer(detail::Tag<float>, Steinberg::Vst::AudioBusBuffers& data) { return data.channelBuffers32; }
	inline auto& getAudioBusPointer(detail::Tag<double>, Steinberg::Vst::AudioBusBuffers& data) { return data.channelBuffers64; }

	static inline int countUsedClientChannels(const std::vector<DynamicChannelMapping>& inputMap,
		const std::vector<DynamicChannelMapping>& outputMap)
	{
		const auto countUsedChannelsInVector = [](const std::vector<DynamicChannelMapping>& map)
		{
			return std::accumulate(map.begin(), map.end(), 0, [](auto acc, const auto& item)
				{
					return acc + (item.isClientActive() ? (int)item.size() : 0);
				});
		};

		return jmax(countUsedChannelsInVector(inputMap), countUsedChannelsInVector(outputMap));
	}

	template <typename FloatType>
	class ScratchBuffer
	{
	public:
		void setSize(int numChannels, int blockSize)
		{
			buffer.setSize(numChannels, blockSize);
		}

		void clear() { channelCounter = 0; }

		auto* getNextChannelBuffer() { return buffer.getWritePointer(channelCounter++); }

		auto getArrayOfWritePointers() { return buffer.getArrayOfWritePointers(); }

	private:
		AudioBuffer<FloatType> buffer;
		int channelCounter = 0;
	};

	template <typename FloatType>
	static int countValidBuses(Steinberg::Vst::AudioBusBuffers* buffers, juce::int32 num)
	{
		return (int)std::distance(buffers, std::find_if(buffers, buffers + num, [](auto& buf)
			{
				return getAudioBusPointer(detail::Tag<FloatType>{}, buf) == nullptr && buf.numChannels > 0;
			}));
	}

	template <typename FloatType, typename Iterator>
	static bool validateLayouts(Iterator first, Iterator last, const std::vector<DynamicChannelMapping>& map)
	{
		if ((size_t)std::distance(first, last) > map.size())
			return false;

		auto mapIterator = map.begin();

		for (auto it = first; it != last; ++it, ++mapIterator)
		{
			auto** busPtr = getAudioBusPointer(detail::Tag<FloatType>{}, * it);
			const auto anyChannelIsNull = std::any_of(busPtr, busPtr + it->numChannels, [](auto* ptr) { return ptr == nullptr; });

			// Null channels are allowed if the bus is inactive
			if (mapIterator->isHostActive() && (anyChannelIsNull || (int)mapIterator->size() != it->numChannels))
				return false;
		}

		// If the host didn't provide the full complement of buses, it must be because the other
		// buses are all deactivated.
		return std::none_of(mapIterator, map.end(), [](const auto& item) { return item.isHostActive(); });
	}

	/*
		The main purpose of this class is to remap a set of buffers provided by the VST3 host into an
		equivalent JUCE AudioBuffer using the JUCE channel layout/order.

		An instance of this class handles input and output remapping for a single data type (float or
		double), matching the FloatType template parameter.

		This is in VST3Common.h, rather than in the VST3_Wrapper.cpp, so that we can test it.

		@see ClientBufferMapper
	*/
	template <typename FloatType>
	class ClientBufferMapperData
	{
	public:
		void prepare(int numChannels, int blockSize)
		{
			scratchBuffer.setSize(numChannels, blockSize);
			channels.reserve((size_t)jmin(128, numChannels));
		}

		AudioBuffer<FloatType> getMappedBuffer(Steinberg::Vst::ProcessData& data,
			const std::vector<DynamicChannelMapping>& inputMap,
			const std::vector<DynamicChannelMapping>& outputMap)
		{
			scratchBuffer.clear();
			channels.clear();

			const auto usedChannels = countUsedClientChannels(inputMap, outputMap);

			// WaveLab workaround: This host may report the wrong number of inputs/outputs so re-count here
			const auto vstInputs = countValidBuses<FloatType>(data.inputs, data.numInputs);

			if (!validateLayouts<FloatType>(data.inputs, data.inputs + vstInputs, inputMap))
				return getBlankBuffer(usedChannels, (int)data.numSamples);

			setUpInputChannels(data, (size_t)vstInputs, scratchBuffer, inputMap, channels);
			setUpOutputChannels(scratchBuffer, outputMap, channels);

			const auto channelPtr = channels.empty() ? scratchBuffer.getArrayOfWritePointers()
				: channels.data();

			return { channelPtr, (int)channels.size(), (int)data.numSamples };
		}

	private:
		static void setUpInputChannels(Steinberg::Vst::ProcessData& data,
			size_t vstInputs,
			ScratchBuffer<FloatType>& scratchBuffer,
			const std::vector<DynamicChannelMapping>& map,
			std::vector<FloatType*>& channels)
		{
			for (size_t busIndex = 0; busIndex < map.size(); ++busIndex)
			{
				const auto mapping = map[busIndex];

				if (!mapping.isClientActive())
					continue;

				const auto originalSize = channels.size();

				for (size_t channelIndex = 0; channelIndex < mapping.size(); ++channelIndex)
					channels.push_back(scratchBuffer.getNextChannelBuffer());

				if (mapping.isHostActive() && busIndex < vstInputs)
				{
					auto** busPtr = getAudioBusPointer(detail::Tag<FloatType>{}, data.inputs[busIndex]);

					for (size_t channelIndex = 0; channelIndex < mapping.size(); ++channelIndex)
					{
						FloatVectorOperations::copy(channels[(size_t)mapping.getJuceChannelForVst3Channel((int)channelIndex) + originalSize],
							busPtr[channelIndex],
							(size_t)data.numSamples);
					}
				}
				else
				{
					for (size_t channelIndex = 0; channelIndex < mapping.size(); ++channelIndex)
						FloatVectorOperations::clear(channels[originalSize + channelIndex], (size_t)data.numSamples);
				}
			}
		}

		static void setUpOutputChannels(ScratchBuffer<FloatType>& scratchBuffer,
			const std::vector<DynamicChannelMapping>& map,
			std::vector<FloatType*>& channels)
		{
			for (size_t i = 0, initialBusIndex = 0; i < (size_t)map.size(); ++i)
			{
				const auto& mapping = map[i];

				if (mapping.isClientActive())
				{
					for (size_t j = 0; j < mapping.size(); ++j)
					{
						if (channels.size() <= initialBusIndex + j)
							channels.push_back(scratchBuffer.getNextChannelBuffer());
					}

					initialBusIndex += mapping.size();
				}
			}
		}

		AudioBuffer<FloatType> getBlankBuffer(int usedChannels, int usedSamples)
		{
			// The host is ignoring the bus layout we requested, so we can't process sensibly!
			jassertfalse;

			// Return a silent buffer for the AudioProcessor to process
			for (auto i = 0; i < usedChannels; ++i)
			{
				channels.push_back(scratchBuffer.getNextChannelBuffer());
				FloatVectorOperations::clear(channels.back(), usedSamples);
			}

			return { channels.data(), (int)channels.size(), usedSamples };
		}

		std::vector<FloatType*> channels;
		ScratchBuffer<FloatType> scratchBuffer;
	};

	//==============================================================================
	/*
		Remaps a set of buffers provided by the VST3 host into an equivalent JUCE AudioBuffer using the
		JUCE channel layout/order.

		An instance of this class can remap to either a float or double JUCE buffer, as necessary.

		Although the VST3 spec requires that the bus layout does not change while the plugin is
		activated and processing, some hosts get this wrong and try to enable/disable buses during
		playback. This class attempts to be resilient, and should cope with buses being switched on and
		off during processing.

		This is in VST3Common.h, rather than in the VST3_Wrapper.cpp, so that we can test it.

		@see ClientBufferMapper
	*/
	class ClientBufferMapper
	{
	public:
		void updateFromProcessor(const AudioProcessor& processor)
		{
			struct Pair
			{
				std::vector<DynamicChannelMapping>& map;
				bool isInput;
			};

			for (const auto& pair : { Pair { inputMap, true }, Pair { outputMap, false } })
			{
				if (pair.map.empty())
				{
					for (auto i = 0; i < processor.getBusCount(pair.isInput); ++i)
						pair.map.emplace_back(*processor.getBus(pair.isInput, i));
				}
				else
				{
					// The number of buses cannot change after creating a VST3 plugin!
					jassert((size_t)processor.getBusCount(pair.isInput) == pair.map.size());

					for (size_t i = 0; i < (size_t)processor.getBusCount(pair.isInput); ++i)
					{
						pair.map[i] = [&]
						{
							DynamicChannelMapping replacement{ *processor.getBus(pair.isInput, (int)i) };
							replacement.setHostActive(pair.map[i].isHostActive());
							return replacement;
						}();
					}
				}
			}
		}

		void prepare(int blockSize)
		{
			const auto findNumChannelsWhenAllBusesEnabled = [](const auto& map)
			{
				return std::accumulate(map.cbegin(), map.cend(), 0, [](auto acc, const auto& item)
					{
						return acc + (int)item.size();
					});
			};

			const auto numChannels = jmax(findNumChannelsWhenAllBusesEnabled(inputMap),
				findNumChannelsWhenAllBusesEnabled(outputMap));

			floatData.prepare(numChannels, blockSize);
			doubleData.prepare(numChannels, blockSize);
		}

		void updateActiveClientBuses(const AudioProcessor::BusesLayout& clientBuses)
		{
			if ((size_t)clientBuses.inputBuses.size() != inputMap.size()
				|| (size_t)clientBuses.outputBuses.size() != outputMap.size())
			{
				jassertfalse;
				return;
			}

			const auto sync = [](auto& map, auto& client)
			{
				for (size_t i = 0; i < map.size(); ++i)
				{
					jassert(client[(int)i] == juce::AudioChannelSet::disabled() || client[(int)i] == map[i].getjuce::AudioChannelSet());
					map[i].setClientActive(client[(int)i] != juce::AudioChannelSet::disabled());
				}
			};

			sync(inputMap, clientBuses.inputBuses);
			sync(outputMap, clientBuses.outputBuses);
		}

		void setInputBusHostActive(size_t bus, bool state) { setHostActive(inputMap, bus, state); }
		void setOutputBusHostActive(size_t bus, bool state) { setHostActive(outputMap, bus, state); }

		auto& getData(detail::Tag<float>) { return floatData; }
		auto& getData(detail::Tag<double>) { return doubleData; }

		juce::AudioChannelSet getRequestedLayoutForInputBus(size_t bus) const
		{
			return getRequestedLayoutForBus(inputMap, bus);
		}

		juce::AudioChannelSet getRequestedLayoutForOutputBus(size_t bus) const
		{
			return getRequestedLayoutForBus(outputMap, bus);
		}

		const std::vector<DynamicChannelMapping>& getInputMap()  const { return inputMap; }
		const std::vector<DynamicChannelMapping>& getOutputMap() const { return outputMap; }

	private:
		static void setHostActive(std::vector<DynamicChannelMapping>& map, size_t bus, bool state)
		{
			if (bus < map.size())
				map[bus].setHostActive(state);
		}

		static juce::AudioChannelSet getRequestedLayoutForBus(const std::vector<DynamicChannelMapping>& map, size_t bus)
		{
			if (bus < map.size() && map[bus].isHostActive())
				return map[bus].getjuce::AudioChannelSet();

			return juce::AudioChannelSet::disabled();
		}

		ClientBufferMapperData<float> floatData;
		ClientBufferMapperData<double> doubleData;

		std::vector<DynamicChannelMapping> inputMap;
		std::vector<DynamicChannelMapping> outputMap;
	};

	//==============================================================================
	/*  Holds a buffer in the JUCE channel layout, and a reference to a Vst ProcessData struct, and
		copies each JUCE channel to the appropriate host output channel when this object goes
		out of scope.
	*/
	template <typename FloatType>
	class ClientRemappedBuffer
	{
	public:
		ClientRemappedBuffer(ClientBufferMapperData<FloatType>& mapperData,
			const std::vector<DynamicChannelMapping>* inputMapIn,
			const std::vector<DynamicChannelMapping>* outputMapIn,
			Steinberg::Vst::ProcessData& hostData)
			: buffer(mapperData.getMappedBuffer(hostData, *inputMapIn, *outputMapIn)),
			outputMap(outputMapIn),
			data(hostData)
		{}

		ClientRemappedBuffer(ClientBufferMapper& mapperIn, Steinberg::Vst::ProcessData& hostData)
			: ClientRemappedBuffer(mapperIn.getData(detail::Tag<FloatType>{}),
				&mapperIn.getInputMap(),
				&mapperIn.getOutputMap(),
				hostData)
		{}

		~ClientRemappedBuffer()
		{
			// WaveLab workaround: This host may report the wrong number of inputs/outputs so re-count here
			const auto vstOutputs = (size_t)countValidBuses<FloatType>(data.outputs, data.numOutputs);

			if (validateLayouts<FloatType>(data.outputs, data.outputs + vstOutputs, *outputMap))
				copyToHostOutputBuses(vstOutputs);
			else
				clearHostOutputBuses(vstOutputs);
		}

		AudioBuffer<FloatType> buffer;

	private:
		void copyToHostOutputBuses(size_t vstOutputs) const
		{
			for (size_t i = 0, juceBusOffset = 0; i < outputMap->size(); ++i)
			{
				const auto& mapping = (*outputMap)[i];

				if (mapping.isHostActive() && i < vstOutputs)
				{
					auto& bus = data.outputs[i];

					if (mapping.isClientActive())
					{
						for (size_t j = 0; j < mapping.size(); ++j)
						{
							auto* hostChannel = getAudioBusPointer(detail::Tag<FloatType>{}, bus)[j];
							const auto juceChannel = juceBusOffset + (size_t)mapping.getJuceChannelForVst3Channel((int)j);
							FloatVectorOperations::copy(hostChannel, buffer.getReadPointer((int)juceChannel), (size_t)data.numSamples);
						}
					}
					else
					{
						for (size_t j = 0; j < mapping.size(); ++j)
						{
							auto* hostChannel = getAudioBusPointer(detail::Tag<FloatType>{}, bus)[j];
							FloatVectorOperations::clear(hostChannel, (size_t)data.numSamples);
						}
					}
				}

				if (mapping.isClientActive())
					juceBusOffset += mapping.size();
			}
		}

		void clearHostOutputBuses(size_t vstOutputs) const
		{
			// The host provided us with an unexpected bus layout.
			jassertfalse;

			std::for_each(data.outputs, data.outputs + vstOutputs, [this](auto& bus)
				{
					auto** busPtr = getAudioBusPointer(detail::Tag<FloatType>{}, bus);
					std::for_each(busPtr, busPtr + bus.numChannels, [this](auto* ptr)
						{
							if (ptr != nullptr)
								FloatVectorOperations::clear(ptr, (int)data.numSamples);
						});
				});
		}

		const std::vector<DynamicChannelMapping>* outputMap = nullptr;
		Steinberg::Vst::ProcessData& data;

		JUCE_DECLARE_NON_COPYABLE(ClientRemappedBuffer)
			JUCE_DECLARE_NON_MOVEABLE(ClientRemappedBuffer)
	};

	//==============================================================================
	/*
		Remaps a JUCE buffer to an equivalent VST3 layout.

		An instance of this class handles mappings for both float and double buffers, but in a single
		direction (input or output).
	*/
	class HostBufferMapper
	{
	public:
		/*  Builds a cached map of juce <-> vst3 channel mappings. */
		void prepare(std::vector<ChannelMapping> arrangements)
		{
			mappings = std::move(arrangements);

			floatBusMap.resize(mappings.size());
			doubleBusMap.resize(mappings.size());
			busBuffers.resize(mappings.size());
		}

		/*  Applies the mapping to an AudioBuffer using JUCE channel layout. */
		template <typename FloatType>
		Steinberg::Vst::AudioBusBuffers* getVst3LayoutForJuceBuffer(juce::AudioBuffer<FloatType>& source)
		{
			int channelIndexOffset = 0;

			for (size_t i = 0; i < mappings.size(); ++i)
			{
				const auto& mapping = mappings[i];
				associateBufferTo(busBuffers[i], get(detail::Tag<FloatType>{})[i], source, mapping, channelIndexOffset);
				channelIndexOffset += mapping.isActive() ? (int)mapping.size() : 0;
			}

			return busBuffers.data();
		}

	private:
		template <typename FloatType>
		using Bus = std::vector<FloatType*>;

		template <typename FloatType>
		using BusMap = std::vector<Bus<FloatType>>;

		static void assignRawPointer(Steinberg::Vst::AudioBusBuffers& vstBuffers, float** raw) { vstBuffers.channelBuffers32 = raw; }
		static void assignRawPointer(Steinberg::Vst::AudioBusBuffers& vstBuffers, double** raw) { vstBuffers.channelBuffers64 = raw; }

		template <typename FloatType>
		void associateBufferTo(Steinberg::Vst::AudioBusBuffers& vstBuffers,
			Bus<FloatType>& bus,
			juce::AudioBuffer<FloatType>& buffer,
			const ChannelMapping& busMap,
			int channelStartOffset) const
		{
			bus.clear();

			for (size_t i = 0; i < busMap.size(); ++i)
			{
				bus.push_back(busMap.isActive() ? buffer.getWritePointer(channelStartOffset + busMap.getJuceChannelForVst3Channel((int)i))
					: nullptr);
			}

			assignRawPointer(vstBuffers, bus.data());
			vstBuffers.numChannels = (Steinberg::int32)busMap.size();
			vstBuffers.silenceFlags = busMap.isActive() ? 0 : std::numeric_limits<Steinberg::uint64>::max();
		}

		auto& get(detail::Tag<float>) { return floatBusMap; }
		auto& get(detail::Tag<double>) { return doubleBusMap; }

		BusMap<float>  floatBusMap;
		BusMap<double> doubleBusMap;

		std::vector<Steinberg::Vst::AudioBusBuffers> busBuffers;
		std::vector<ChannelMapping> mappings;
	};

	//==============================================================================
	template <class ObjectType>
	class VSTComSmartPtr
	{
	public:
		VSTComSmartPtr() noexcept : source(nullptr) {}
		VSTComSmartPtr(ObjectType* object, bool autoAddRef = true) noexcept : source(object) { if (source != nullptr && autoAddRef) source->addRef(); }
		VSTComSmartPtr(const VSTComSmartPtr& other) noexcept : source(other.source) { if (source != nullptr) source->addRef(); }
		~VSTComSmartPtr() { if (source != nullptr) source->release(); }

		operator ObjectType* () const noexcept { return source; }
		ObjectType* get() const noexcept { return source; }
		ObjectType& operator*() const noexcept { return *source; }
		ObjectType* operator->() const noexcept { return source; }

		VSTComSmartPtr& operator= (const VSTComSmartPtr& other) { return operator= (other.source); }

		VSTComSmartPtr& operator= (ObjectType* const newObjectToTakePossessionOf)
		{
			VSTComSmartPtr p(newObjectToTakePossessionOf);
			std::swap(p.source, source);
			return *this;
		}

		bool operator== (ObjectType* const other) noexcept { return source == other; }
		bool operator!= (ObjectType* const other) noexcept { return source != other; }

		bool loadFrom(Steinberg::FUnknown* o)
		{
			*this = nullptr;
			return o != nullptr && o->queryInterface(ObjectType::iid, (void**)&source) == Steinberg::kResultOk;
		}

		bool loadFrom(Steinberg::IPluginFactory* factory, const Steinberg::TUID& uuid)
		{
			jassert(factory != nullptr);
			*this = nullptr;
			return factory->createInstance(uuid, ObjectType::iid, (void**)&source) == Steinberg::kResultOk;
		}

	private:
		ObjectType* source;
	};

	//==============================================================================
	/*  This class stores a plugin's preferred MIDI mappings.

		The IMidiMapping is normally an extension of the IEditController which
		should only be accessed from the UI thread. If we're being strict about
		things, then we shouldn't call IMidiMapping functions from the audio thread.

		This code is very similar to that found in the audioclient demo code in the
		VST3 SDK repo.
	*/
	class StoredMidiMapping
	{
	public:
		StoredMidiMapping()
		{
			for (auto& channel : channels)
				channel.resize(Steinberg::Vst::kCountCtrlNumber);
		}

		void storeMappings(Steinberg::Vst::IMidiMapping& mapping)
		{
			for (size_t channelIndex = 0; channelIndex < channels.size(); ++channelIndex)
				storeControllers(mapping, channels[channelIndex], channelIndex);
		}

		/* Returns kNoParamId if there is no mapping for this controller. */
		Steinberg::Vst::ParamID getMapping(Steinberg::int16 channel,
			Steinberg::Vst::CtrlNumber controller) const noexcept
		{
			return channels[(size_t)channel][(size_t)controller];
		}

	private:
		// Maps controller numbers to ParamIDs
		using Controllers = std::vector<Steinberg::Vst::ParamID>;

		// Each channel may have a different CC mapping
		using Channels = std::array<Controllers, 16>;

		static void storeControllers(Steinberg::Vst::IMidiMapping& mapping, Controllers& channel, size_t channelIndex)
		{
			for (size_t controllerIndex = 0; controllerIndex < channel.size(); ++controllerIndex)
				channel[controllerIndex] = getSingleMapping(mapping, channelIndex, controllerIndex);
		}

		static Steinberg::Vst::ParamID getSingleMapping(Steinberg::Vst::IMidiMapping& mapping,
			size_t channelIndex,
			size_t controllerIndex)
		{
			Steinberg::Vst::ParamID result{};
			const auto returnCode = mapping.getMidiControllerAssignment(0,
				(juce::int16)channelIndex,
				(Steinberg::Vst::CtrlNumber)controllerIndex,
				result);

			return returnCode == Steinberg::kResultTrue ? result : Steinberg::Vst::kNoParamId;
		}

		Channels channels;
	};

	//==============================================================================
	class MidiEventList : public Steinberg::Vst::IEventList
	{
	public:
		MidiEventList() = default;
		virtual ~MidiEventList() = default;

		JUCE_DECLARE_VST3_COM_REF_METHODS
			JUCE_DECLARE_VST3_COM_QUERY_METHODS

			//==============================================================================
			void clear()
		{
			events.clearQuick();
		}

		Steinberg::int32 PLUGIN_API getEventCount() override
		{
			return (Steinberg::int32)events.size();
		}

		// NB: This has to cope with out-of-range indexes from some plugins.
		Steinberg::tresult PLUGIN_API getEvent(Steinberg::int32 index, Steinberg::Vst::Event& e) override
		{
			if (isPositiveAndBelow((int)index, events.size()))
			{
				e = events.getReference((int)index);
				return Steinberg::kResultTrue;
			}

			return Steinberg::kResultFalse;
		}

		Steinberg::tresult PLUGIN_API addEvent(Steinberg::Vst::Event& e) override
		{
			events.add(e);
			return Steinberg::kResultTrue;
		}

		//==============================================================================
		static void toMidiBuffer(MidiBuffer& result, Steinberg::Vst::IEventList& eventList)
		{
			const auto numEvents = eventList.getEventCount();

			for (Steinberg::int32 i = 0; i < numEvents; ++i)
			{
				Steinberg::Vst::Event e;

				if (eventList.getEvent(i, e) != Steinberg::kResultOk)
					continue;

				if (const auto message = toMidiMessage(e))
					result.addEvent(*message, e.sampleOffset);
			}
		}

		template <typename Callback>
		static void hostToPluginEventList(Steinberg::Vst::IEventList& result,
			MidiBuffer& midiBuffer,
			StoredMidiMapping& mapping,
			Callback&& callback)
		{
			toEventList(result, midiBuffer, &mapping, callback);
		}

		static void pluginToHostEventList(Steinberg::Vst::IEventList& result, MidiBuffer& midiBuffer)
		{
			toEventList(result, midiBuffer, nullptr, [](auto&&...) {});
		}

	private:
		enum class EventConversionKind
		{
			// Hosted plugins don't expect to receive LegacyMIDICCEvents messages from the host,
			// so if we're converting midi from the host to an eventlist, this mode will avoid
			// converting to Legacy events where possible.
			hostToPlugin,

			// If plugins generate MIDI internally, then where possible we should preserve
			// these messages as LegacyMIDICCOut events.
			pluginToHost
		};

		template <typename Callback>
		static bool sendMappedParameter(const MidiMessage& msg,
			StoredMidiMapping* midiMapping,
			Callback&& callback)
		{
			if (midiMapping == nullptr)
				return false;

			const auto controlEvent = toVst3ControlEvent(msg);

			if (!controlEvent.has_value())
				return false;

			const auto controlParamID = midiMapping->getMapping(createSafeChannel(msg.getChannel()),
				controlEvent->controllerNumber);

			if (controlParamID != Steinberg::Vst::kNoParamId)
				callback(controlParamID, controlEvent->paramValue);

			return true;
		}

		template <typename Callback>
		static void processMidiMessage(Steinberg::Vst::IEventList& result,
			const MidiMessageMetadata metadata,
			StoredMidiMapping* midiMapping,
			Callback&& callback)
		{
			const auto msg = metadata.getMessage();

			if (sendMappedParameter(msg, midiMapping, std::forward<Callback>(callback)))
				return;

			const auto kind = midiMapping != nullptr ? EventConversionKind::hostToPlugin
				: EventConversionKind::pluginToHost;

			auto maybeEvent = createVstEvent(msg, metadata.data, kind);

			if (!maybeEvent.hasValue())
				return;

			maybeEvent->busIndex = 0;
			maybeEvent->sampleOffset = metadata.samplePosition;
			result.addEvent(*maybeEvent);
		}

		/*  If mapping is non-null, the conversion is assumed to be host-to-plugin, or otherwise
			plugin-to-host.
		*/
		template <typename Callback>
		static void toEventList(Steinberg::Vst::IEventList& result,
			MidiBuffer& midiBuffer,
			StoredMidiMapping* midiMapping,
			Callback&& callback)
		{
			enum { maxNumEvents = 2048 }; // Steinberg's Host Checker states that no more than 2048 events are allowed at once
			int numEvents = 0;

			for (const auto metadata : midiBuffer)
			{
				if (++numEvents > maxNumEvents)
					break;

				processMidiMessage(result, metadata, midiMapping, std::forward<Callback>(callback));
			}
		}

		Array<Steinberg::Vst::Event, CriticalSection> events;
		Atomic<int> refCount;

		static Steinberg::int16 createSafeChannel(int channel) noexcept { return (Steinberg::int16)jlimit(0, 15, channel - 1); }
		static int createSafeChannel(Steinberg::int16 channel) noexcept { return (int)jlimit(1, 16, channel + 1); }

		static Steinberg::int16 createSafeNote(int note) noexcept { return (Steinberg::int16)jlimit(0, 127, note); }
		static int createSafeNote(Steinberg::int16 note) noexcept { return jlimit(0, 127, (int)note); }

		static float normaliseMidiValue(int value) noexcept { return jlimit(0.0f, 1.0f, (float)value / 127.0f); }
		static int denormaliseToMidiValue(float value) noexcept { return roundToInt(jlimit(0.0f, 127.0f, value * 127.0f)); }

		static Steinberg::Vst::Event createNoteOnEvent(const MidiMessage& msg) noexcept
		{
			Steinberg::Vst::Event e{};
			e.type = Steinberg::Vst::Event::kNoteOnEvent;
			e.noteOn.channel = createSafeChannel(msg.getChannel());
			e.noteOn.pitch = createSafeNote(msg.getNoteNumber());
			e.noteOn.velocity = normaliseMidiValue(msg.getVelocity());
			e.noteOn.length = 0;
			e.noteOn.tuning = 0.0f;
			e.noteOn.noteId = -1;
			return e;
		}

		static Steinberg::Vst::Event createNoteOffEvent(const MidiMessage& msg) noexcept
		{
			Steinberg::Vst::Event e{};
			e.type = Steinberg::Vst::Event::kNoteOffEvent;
			e.noteOff.channel = createSafeChannel(msg.getChannel());
			e.noteOff.pitch = createSafeNote(msg.getNoteNumber());
			e.noteOff.velocity = normaliseMidiValue(msg.getVelocity());
			e.noteOff.tuning = 0.0f;
			e.noteOff.noteId = -1;
			return e;
		}

		static Steinberg::Vst::Event createSysExEvent(const MidiMessage& msg, const juce::uint8* midiEventData) noexcept
		{
			Steinberg::Vst::Event e{};
			e.type = Steinberg::Vst::Event::kDataEvent;
			e.data.bytes = midiEventData + 1;
			e.data.size = (uint32)msg.getSysExDataSize();
			e.data.type = Steinberg::Vst::DataEvent::kMidiSysEx;
			return e;
		}

		static Steinberg::Vst::Event createLegacyMIDIEvent(int channel, int controlNumber, int value, int value2 = 0)
		{
			Steinberg::Vst::Event e{};
			e.type = Steinberg::Vst::Event::kLegacyMIDICCOutEvent;
			e.midiCCOut.channel = Steinberg::int8(createSafeChannel(channel));
			e.midiCCOut.controlNumber = juce::uint8(jlimit(0, 255, controlNumber));
			e.midiCCOut.value = Steinberg::int8(createSafeNote(value));
			e.midiCCOut.value2 = Steinberg::int8(createSafeNote(value2));
			return e;
		}

		static Steinberg::Vst::Event createPolyPressureEvent(const MidiMessage& msg)
		{
			Steinberg::Vst::Event e{};
			e.type = Steinberg::Vst::Event::kPolyPressureEvent;
			e.polyPressure.channel = createSafeChannel(msg.getChannel());
			e.polyPressure.pitch = createSafeNote(msg.getNoteNumber());
			e.polyPressure.pressure = normaliseMidiValue(msg.getAfterTouchValue());
			e.polyPressure.noteId = -1;
			return e;
		}

		static Steinberg::Vst::Event createChannelPressureEvent(const MidiMessage& msg) noexcept
		{
			return createLegacyMIDIEvent(msg.getChannel(),
				Steinberg::Vst::kAfterTouch,
				msg.getChannelPressureValue());
		}

		static Steinberg::Vst::Event createControllerEvent(const MidiMessage& msg) noexcept
		{
			return createLegacyMIDIEvent(msg.getChannel(),
				msg.getControllerNumber(),
				msg.getControllerValue());
		}

		static Steinberg::Vst::Event createCtrlPolyPressureEvent(const MidiMessage& msg) noexcept
		{
			return createLegacyMIDIEvent(msg.getChannel(),
				Steinberg::Vst::kCtrlPolyPressure,
				msg.getNoteNumber(),
				msg.getAfterTouchValue());
		}

		static Steinberg::Vst::Event createPitchWheelEvent(const MidiMessage& msg) noexcept
		{
			return createLegacyMIDIEvent(msg.getChannel(),
				Steinberg::Vst::kPitchBend,
				msg.getRawData()[1],
				msg.getRawData()[2]);
		}

		static Steinberg::Vst::Event createProgramChangeEvent(const MidiMessage& msg) noexcept
		{
			return createLegacyMIDIEvent(msg.getChannel(),
				Steinberg::Vst::kCtrlProgramChange,
				msg.getProgramChangeNumber());
		}

		static Steinberg::Vst::Event createCtrlQuarterFrameEvent(const MidiMessage& msg) noexcept
		{
			return createLegacyMIDIEvent(msg.getChannel(),
				Steinberg::Vst::kCtrlQuarterFrame,
				msg.getQuarterFrameValue());
		}

		static Optional<Steinberg::Vst::Event> createVstEvent(const MidiMessage& msg,
			const juce::uint8* midiEventData,
			EventConversionKind kind) noexcept
		{
			if (msg.isNoteOn())
				return createNoteOnEvent(msg);

			if (msg.isNoteOff())
				return createNoteOffEvent(msg);

			if (msg.isSysEx())
				return createSysExEvent(msg, midiEventData);

			if (msg.isChannelPressure())
				return createChannelPressureEvent(msg);

			if (msg.isPitchWheel())
				return createPitchWheelEvent(msg);

			if (msg.isProgramChange())
				return createProgramChangeEvent(msg);

			if (msg.isController())
				return createControllerEvent(msg);

			if (msg.isQuarterFrame())
				return createCtrlQuarterFrameEvent(msg);

			if (msg.isAftertouch())
			{
				switch (kind)
				{
				case EventConversionKind::hostToPlugin:
					return createPolyPressureEvent(msg);

				case EventConversionKind::pluginToHost:
					return createCtrlPolyPressureEvent(msg);
				}

				jassertfalse;
				return {};
			}

			return {};
		}

		static Optional<MidiMessage> toMidiMessage(const Steinberg::Vst::LegacyMIDICCOutEvent& e)
		{
			if (e.controlNumber <= 127)
				return MidiMessage::controllerEvent(createSafeChannel(juce::int16(e.channel)),
					createSafeNote(juce::int16(e.controlNumber)),
					createSafeNote(juce::int16(e.value)));

			switch (e.controlNumber)
			{
			case Steinberg::Vst::kAfterTouch:
				return MidiMessage::channelPressureChange(createSafeChannel(juce::int16(e.channel)),
					createSafeNote(juce::int16(e.value)));

			case Steinberg::Vst::kPitchBend:
				return MidiMessage::pitchWheel(createSafeChannel(juce::int16(e.channel)),
					(e.value & 0x7f) | ((e.value2 & 0x7f) << 7));

			case Steinberg::Vst::kCtrlProgramChange:
				return MidiMessage::programChange(createSafeChannel(juce::int16(e.channel)),
					createSafeNote(juce::int16(e.value)));

			case Steinberg::Vst::kCtrlQuarterFrame:
				return MidiMessage::quarterFrame(createSafeChannel(juce::int16(e.channel)),
					createSafeNote(juce::int16(e.value)));

			case Steinberg::Vst::kCtrlPolyPressure:
				return MidiMessage::aftertouchChange(createSafeChannel(juce::int16(e.channel)),
					createSafeNote(juce::int16(e.value)),
					createSafeNote(juce::int16(e.value2)));

			default:
				// If this is hit, we're trying to convert a LegacyMIDICCOutEvent with an unknown controlNumber.
				jassertfalse;
				return {};
			}
		}

		static Optional<MidiMessage> toMidiMessage(const Steinberg::Vst::Event& e)
		{
			switch (e.type)
			{
			case Steinberg::Vst::Event::kNoteOnEvent:
				return MidiMessage::noteOn(createSafeChannel(e.noteOn.channel),
					createSafeNote(e.noteOn.pitch),
					(Steinberg::juce::uint8)denormaliseToMidiValue(e.noteOn.velocity));

			case Steinberg::Vst::Event::kNoteOffEvent:
				return MidiMessage::noteOff(createSafeChannel(e.noteOff.channel),
					createSafeNote(e.noteOff.pitch),
					(Steinberg::juce::uint8)denormaliseToMidiValue(e.noteOff.velocity));

			case Steinberg::Vst::Event::kPolyPressureEvent:
				return MidiMessage::aftertouchChange(createSafeChannel(e.polyPressure.channel),
					createSafeNote(e.polyPressure.pitch),
					(Steinberg::juce::uint8)denormaliseToMidiValue(e.polyPressure.pressure));

			case Steinberg::Vst::Event::kDataEvent:
				return MidiMessage::createSysExMessage(e.data.bytes, (int)e.data.size);

			case Steinberg::Vst::Event::kLegacyMIDICCOutEvent:
				return toMidiMessage(e.midiCCOut);

			case Steinberg::Vst::Event::kNoteExpressionValueEvent:
			case Steinberg::Vst::Event::kNoteExpressionTextEvent:
			case Steinberg::Vst::Event::kChordEvent:
			case Steinberg::Vst::Event::kScaleEvent:
				return {};

			default:
				break;
			}

			// If this is hit, we've been sent an event type that doesn't exist in the VST3 spec.
			jassertfalse;
			return {};
		}

		//==============================================================================
		struct Vst3MidiControlEvent
		{
			Steinberg::Vst::CtrlNumber controllerNumber;
			Steinberg::Vst::ParamValue paramValue;
		};

		static std::optional<Vst3MidiControlEvent> toVst3ControlEvent(const MidiMessage& msg)
		{
			if (msg.isController())
				return Vst3MidiControlEvent{ (Steinberg::Vst::CtrlNumber)msg.getControllerNumber(), msg.getControllerValue() / 127.0 };

			if (msg.isPitchWheel())
				return Vst3MidiControlEvent{ Steinberg::Vst::kPitchBend, msg.getPitchWheelValue() / 16383.0 };

			if (msg.isChannelPressure())
				return Vst3MidiControlEvent{ Steinberg::Vst::kAfterTouch, msg.getChannelPressureValue() / 127.0 };

			return {};
		}

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiEventList)
	};
	*/
	//==============================================================================
	/*  Provides very quick polling of all parameter states.

		We must iterate all parameters on each processBlock call to check whether any
		parameter value has changed. This class attempts to make this polling process
		as quick as possible.

		The indices here are of type Steinberg::int32, as they are expected to correspond
		to parameter information obtained from the IEditController. These indices may not
		match the indices of parameters returned from AudioProcessor::getParameters(), so
		be careful!
	*/
	/*
	class CachedParamValues
	{
	public:
		CachedParamValues() = default;

		explicit CachedParamValues(std::vector<Steinberg::Vst::ParamID> paramIdsIn)
			: paramIds(std::move(paramIdsIn)), floatCache(paramIds.size()) {}

		size_t size() const noexcept { return floatCache.size(); }

		Steinberg::Vst::ParamID getParamID(Steinberg::int32 index) const noexcept { return paramIds[(size_t)index]; }

		void set(Steinberg::int32 index, float value) { floatCache.setValueAndBits((size_t)index, value, 1); }
		void setWithoutNotifying(Steinberg::int32 index, float value) { floatCache.setValue((size_t)index, value); }

		float get(Steinberg::int32 index) const noexcept { return floatCache.get((size_t)index); }

		template <typename Callback>
		void ifSet(Callback&& callback)
		{
			floatCache.ifSet([&](size_t index, float value, uint32_t)
				{
					callback((Steinberg::int32)index, value);
				});
		}

	private:
		std::vector<Steinberg::Vst::ParamID> paramIds;
		FlaggedFloatCache<1> floatCache;
	};
	*/
	//==============================================================================
	/*  Ensures that a 'restart' call only ever happens on the main thread. */
	/*class ComponentRestarter : private AsyncUpdater
	{
	public:
		struct Listener
		{
			virtual ~Listener() = default;
			virtual void restartComponentOnMessageThread(int32 flags) = 0;
		};

		explicit ComponentRestarter(Listener& listenerIn)
			: listener(listenerIn) {}

		~ComponentRestarter() noexcept override
		{
			cancelPendingUpdate();
		}

		void restart(int32 newFlags)
		{
			if (newFlags == 0)
				return;

			flags.fetch_or(newFlags);

			if (MessageManager::getInstance()->isThisTheMessageThread())
				handleAsyncUpdate();
			else
				triggerAsyncUpdate();
		}

	private:
		void handleAsyncUpdate() override
		{
			listener.restartComponentOnMessageThread(flags.exchange(0));
		}

		Listener& listener;
		std::atomic<int32> flags { 0 };
	};*/
