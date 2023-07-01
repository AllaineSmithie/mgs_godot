//-----------------------------------------------------------------------------
// Project     : SDK Core
//
// Category    : SDK Core Interface IID definitions
// Filename    : pluginterfaces/base/coreiids.cpp
// Created by  : Steinberg, 01/2004
// Description : Basic Interface
//
//------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution.
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//------------------------------------------------------------------------

#include "thirdparty/vst3sdk/pluginterfaces/base/funknown.h"
#include "thirdparty/vst3sdk/pluginterfaces/base/ibstream.h"
#include "thirdparty/vst3sdk/pluginterfaces/base/icloneable.h"
#include "thirdparty/vst3sdk/pluginterfaces/base/ipluginbase.h"
#include "thirdparty/vst3sdk/pluginterfaces/base/iupdatehandler.h"

//------------------------------------------------------------------------
namespace Steinberg {
DEF_CLASS_IID (IPluginBase)
DEF_CLASS_IID (IPluginFactory)
DEF_CLASS_IID (IPluginFactory2)
DEF_CLASS_IID (IPluginFactory3)

DEF_CLASS_IID (FUnknown)

DEF_CLASS_IID (ICloneable)

DEF_CLASS_IID (IDependent)
DEF_CLASS_IID (IUpdateHandler)

DEF_CLASS_IID (IBStream)
DEF_CLASS_IID (ISizeableStream)

//------------------------------------------------------------------------
} // Steinberg
