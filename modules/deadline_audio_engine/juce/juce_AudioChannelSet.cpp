/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/
#include "juce_AudioChannelSet.h"
#include "juce_MathsFunctions.h"
#include <core/string/ustring.h>
#include <numeric>
namespace juce
{



	AudioChannelSet::AudioChannelSet(uint32 c) : channels(static_cast<int64> (c))
	{
	}

	AudioChannelSet::AudioChannelSet(const std::initializer_list<ChannelType>& c)
	{
		for (auto channel : c)
			addChannel(channel);
	}

	bool AudioChannelSet::operator== (const AudioChannelSet& other) const noexcept { return channels == other.channels; }
	bool AudioChannelSet::operator!= (const AudioChannelSet& other) const noexcept { return channels != other.channels; }
	bool AudioChannelSet::operator<  (const AudioChannelSet& other) const noexcept { return channels < other.channels; }

	String AudioChannelSet::getChannelTypeName(AudioChannelSet::ChannelType type)
	{
		if (type >= discreteChannel0)
			return "Discrete " + String::num_int64(type - discreteChannel0 + 1);

		switch (type)
		{
		case left:                return TTR("Left");
		case right:               return TTR("Right");
		case centre:              return TTR("Centre");
		case LFE:                 return TTR("LFE");
		case leftSurround:        return TTR("Left Surround");
		case rightSurround:       return TTR("Right Surround");
		case leftCentre:          return TTR("Left Centre");
		case rightCentre:         return TTR("Right Centre");
		case centreSurround:      return TTR("Centre Surround");
		case leftSurroundRear:    return TTR("Left Surround Rear");
		case rightSurroundRear:   return TTR("Right Surround Rear");
		case topMiddle:           return TTR("Top Middle");
		case topFrontLeft:        return TTR("Top Front Left");
		case topFrontCentre:      return TTR("Top Front Centre");
		case topFrontRight:       return TTR("Top Front Right");
		case topRearLeft:         return TTR("Top Rear Left");
		case topRearCentre:       return TTR("Top Rear Centre");
		case topRearRight:        return TTR("Top Rear Right");
		case wideLeft:            return TTR("Wide Left");
		case wideRight:           return TTR("Wide Right");
		case LFE2:                return TTR("LFE 2");
		case leftSurroundSide:    return TTR("Left Surround Side");
		case rightSurroundSide:   return TTR("Right Surround Side");
		case ambisonicW:          return TTR("Ambisonic W");
		case ambisonicX:          return TTR("Ambisonic X");
		case ambisonicY:          return TTR("Ambisonic Y");
		case ambisonicZ:          return TTR("Ambisonic Z");
		case topSideLeft:         return TTR("Top Side Left");
		case topSideRight:        return TTR("Top Side Right");
		case ambisonicACN4:       return TTR("Ambisonic 4");
		case ambisonicACN5:       return TTR("Ambisonic 5");
		case ambisonicACN6:       return TTR("Ambisonic 6");
		case ambisonicACN7:       return TTR("Ambisonic 7");
		case ambisonicACN8:       return TTR("Ambisonic 8");
		case ambisonicACN9:       return TTR("Ambisonic 9");
		case ambisonicACN10:      return TTR("Ambisonic 10");
		case ambisonicACN11:      return TTR("Ambisonic 11");
		case ambisonicACN12:      return TTR("Ambisonic 12");
		case ambisonicACN13:      return TTR("Ambisonic 13");
		case ambisonicACN14:      return TTR("Ambisonic 14");
		case ambisonicACN15:      return TTR("Ambisonic 15");
		case ambisonicACN16:      return TTR("Ambisonic 16");
		case ambisonicACN17:      return TTR("Ambisonic 17");
		case ambisonicACN18:      return TTR("Ambisonic 18");
		case ambisonicACN19:      return TTR("Ambisonic 19");
		case ambisonicACN20:      return TTR("Ambisonic 20");
		case ambisonicACN21:      return TTR("Ambisonic 21");
		case ambisonicACN22:      return TTR("Ambisonic 22");
		case ambisonicACN23:      return TTR("Ambisonic 23");
		case ambisonicACN24:      return TTR("Ambisonic 24");
		case ambisonicACN25:      return TTR("Ambisonic 25");
		case ambisonicACN26:      return TTR("Ambisonic 26");
		case ambisonicACN27:      return TTR("Ambisonic 27");
		case ambisonicACN28:      return TTR("Ambisonic 28");
		case ambisonicACN29:      return TTR("Ambisonic 29");
		case ambisonicACN30:      return TTR("Ambisonic 30");
		case ambisonicACN31:      return TTR("Ambisonic 31");
		case ambisonicACN32:      return TTR("Ambisonic 32");
		case ambisonicACN33:      return TTR("Ambisonic 33");
		case ambisonicACN34:      return TTR("Ambisonic 34");
		case ambisonicACN35:      return TTR("Ambisonic 35");
		case bottomFrontLeft:     return TTR("Bottom Front Left");
		case bottomFrontCentre:   return TTR("Bottom Front Centre");
		case bottomFrontRight:    return TTR("Bottom Front Right");
		case proximityLeft:       return TTR("Proximity Left");
		case proximityRight:      return TTR("Proximity Right");
		case bottomSideLeft:      return TTR("Bottom Side Left");
		case bottomSideRight:     return TTR("Bottom Side Right");
		case bottomRearLeft:      return TTR("Bottom Rear Left");
		case bottomRearCentre:    return TTR("Bottom Rear Centre");
		case bottomRearRight:     return TTR("Bottom Rear Right");
		case discreteChannel0:
		case unknown:
		default:                  break;
		}

		return "Unknown";
	}

	String AudioChannelSet::getAbbreviatedChannelTypeName(AudioChannelSet::ChannelType type)
	{
		if (type >= discreteChannel0)
			return String::num_int64(type - discreteChannel0 + 1);

		switch (type)
		{
		case left:                return "L";
		case right:               return "R";
		case centre:              return "C";
		case LFE:                 return "Lfe";
		case leftSurround:        return "Ls";
		case rightSurround:       return "Rs";
		case leftCentre:          return "Lc";
		case rightCentre:         return "Rc";
		case centreSurround:      return "Cs";
		case leftSurroundRear:    return "Lrs";
		case rightSurroundRear:   return "Rrs";
		case topMiddle:           return "Tm";
		case topFrontLeft:        return "Tfl";
		case topFrontCentre:      return "Tfc";
		case topFrontRight:       return "Tfr";
		case topRearLeft:         return "Trl";
		case topRearCentre:       return "Trc";
		case topRearRight:        return "Trr";
		case wideLeft:            return "Wl";
		case wideRight:           return "Wr";
		case LFE2:                return "Lfe2";
		case leftSurroundSide:    return "Lss";
		case rightSurroundSide:   return "Rss";
		case ambisonicACN0:       return "ACN0";
		case ambisonicACN1:       return "ACN1";
		case ambisonicACN2:       return "ACN2";
		case ambisonicACN3:       return "ACN3";
		case ambisonicACN4:       return "ACN4";
		case ambisonicACN5:       return "ACN5";
		case ambisonicACN6:       return "ACN6";
		case ambisonicACN7:       return "ACN7";
		case ambisonicACN8:       return "ACN8";
		case ambisonicACN9:       return "ACN9";
		case ambisonicACN10:      return "ACN10";
		case ambisonicACN11:      return "ACN11";
		case ambisonicACN12:      return "ACN12";
		case ambisonicACN13:      return "ACN13";
		case ambisonicACN14:      return "ACN14";
		case ambisonicACN15:      return "ACN15";
		case ambisonicACN16:      return "ACN16";
		case ambisonicACN17:      return "ACN17";
		case ambisonicACN18:      return "ACN18";
		case ambisonicACN19:      return "ACN19";
		case ambisonicACN20:      return "ACN20";
		case ambisonicACN21:      return "ACN21";
		case ambisonicACN22:      return "ACN22";
		case ambisonicACN23:      return "ACN23";
		case ambisonicACN24:      return "ACN24";
		case ambisonicACN25:      return "ACN25";
		case ambisonicACN26:      return "ACN26";
		case ambisonicACN27:      return "ACN27";
		case ambisonicACN28:      return "ACN28";
		case ambisonicACN29:      return "ACN29";
		case ambisonicACN30:      return "ACN30";
		case ambisonicACN31:      return "ACN31";
		case ambisonicACN32:      return "ACN32";
		case ambisonicACN33:      return "ACN33";
		case ambisonicACN34:      return "ACN34";
		case ambisonicACN35:      return "ACN35";
		case topSideLeft:         return "Tsl";
		case topSideRight:        return "Tsr";
		case bottomFrontLeft:     return "Bfl";
		case bottomFrontCentre:   return "Bfc";
		case bottomFrontRight:    return "Bfr";
		case proximityLeft:       return "Pl";
		case proximityRight:      return "Pr";
		case bottomSideLeft:      return "Bsl";
		case bottomSideRight:     return "Bsr";
		case bottomRearLeft:      return "Brl";
		case bottomRearCentre:    return "Brc";
		case bottomRearRight:     return "Brr";
		case discreteChannel0:
		case unknown:
		default:                  break;
		}

		if (type >= ambisonicACN4 && type <= ambisonicACN35)
			return "ACN" + String::num_int64(type - ambisonicACN4 + 4);

		return {};
	}

	AudioChannelSet::ChannelType AudioChannelSet::getChannelTypeFromAbbreviation(const String& abbr)
	{
		if (abbr.length() > 0 && (abbr[0] >= '0' && abbr[0] <= '9'))
			return static_cast<AudioChannelSet::ChannelType> (static_cast<int> (discreteChannel0)
				+ abbr.to_int() - 1);

		if (abbr == "L")     return left;
		if (abbr == "R")     return right;
		if (abbr == "C")     return centre;
		if (abbr == "Lfe")   return LFE;
		if (abbr == "Ls")    return leftSurround;
		if (abbr == "Rs")    return rightSurround;
		if (abbr == "Lc")    return leftCentre;
		if (abbr == "Rc")    return rightCentre;
		if (abbr == "Cs")    return centreSurround;
		if (abbr == "Lrs")   return leftSurroundRear;
		if (abbr == "Rrs")   return rightSurroundRear;
		if (abbr == "Tm")    return topMiddle;
		if (abbr == "Tfl")   return topFrontLeft;
		if (abbr == "Tfc")   return topFrontCentre;
		if (abbr == "Tfr")   return topFrontRight;
		if (abbr == "Trl")   return topRearLeft;
		if (abbr == "Trc")   return topRearCentre;
		if (abbr == "Trr")   return topRearRight;
		if (abbr == "Wl")    return wideLeft;
		if (abbr == "Wr")    return wideRight;
		if (abbr == "Lfe2")  return LFE2;
		if (abbr == "Lss")   return leftSurroundSide;
		if (abbr == "Rss")   return rightSurroundSide;
		if (abbr == "W")     return ambisonicW;
		if (abbr == "X")     return ambisonicX;
		if (abbr == "Y")     return ambisonicY;
		if (abbr == "Z")     return ambisonicZ;
		if (abbr == "ACN0")  return ambisonicACN0;
		if (abbr == "ACN1")  return ambisonicACN1;
		if (abbr == "ACN2")  return ambisonicACN2;
		if (abbr == "ACN3")  return ambisonicACN3;
		if (abbr == "ACN4")  return ambisonicACN4;
		if (abbr == "ACN5")  return ambisonicACN5;
		if (abbr == "ACN6")  return ambisonicACN6;
		if (abbr == "ACN7")  return ambisonicACN7;
		if (abbr == "ACN8")  return ambisonicACN8;
		if (abbr == "ACN9")  return ambisonicACN9;
		if (abbr == "ACN10") return ambisonicACN10;
		if (abbr == "ACN11") return ambisonicACN11;
		if (abbr == "ACN12") return ambisonicACN12;
		if (abbr == "ACN13") return ambisonicACN13;
		if (abbr == "ACN14") return ambisonicACN14;
		if (abbr == "ACN15") return ambisonicACN15;
		if (abbr == "ACN16") return ambisonicACN16;
		if (abbr == "ACN17") return ambisonicACN17;
		if (abbr == "ACN18") return ambisonicACN18;
		if (abbr == "ACN19") return ambisonicACN19;
		if (abbr == "ACN20") return ambisonicACN20;
		if (abbr == "ACN21") return ambisonicACN21;
		if (abbr == "ACN22") return ambisonicACN22;
		if (abbr == "ACN23") return ambisonicACN23;
		if (abbr == "ACN24") return ambisonicACN24;
		if (abbr == "ACN25") return ambisonicACN25;
		if (abbr == "ACN26") return ambisonicACN26;
		if (abbr == "ACN27") return ambisonicACN27;
		if (abbr == "ACN28") return ambisonicACN28;
		if (abbr == "ACN29") return ambisonicACN29;
		if (abbr == "ACN30") return ambisonicACN30;
		if (abbr == "ACN31") return ambisonicACN31;
		if (abbr == "ACN32") return ambisonicACN32;
		if (abbr == "ACN33") return ambisonicACN33;
		if (abbr == "ACN34") return ambisonicACN34;
		if (abbr == "ACN35") return ambisonicACN35;
		if (abbr == "Tsl")   return topSideLeft;
		if (abbr == "Tsr")   return topSideRight;
		if (abbr == "Bfl")   return bottomFrontLeft;
		if (abbr == "Bfc")   return bottomFrontCentre;
		if (abbr == "Bfr")   return bottomFrontRight;
		if (abbr == "Bsl")   return bottomSideLeft;
		if (abbr == "Bsr")   return bottomSideRight;
		if (abbr == "Brl")   return bottomRearLeft;
		if (abbr == "Brc")   return bottomRearCentre;
		if (abbr == "Brr")   return bottomRearRight;
		return unknown;
	}

	String AudioChannelSet::getSpeakerArrangementAsString() const
	{
		PackedStringArray speakerTypes;

		for (auto& speaker : getChannelTypes())
		{
			String name = getAbbreviatedChannelTypeName(speaker);

			if (!name.is_empty())
				speakerTypes.append(name);
		}

		String result;
		for (auto s : speakerTypes)
			result += s + " ";
		result.remove_at(result.size() - 1);
		return result;
	}

	AudioChannelSet AudioChannelSet::fromAbbreviatedString(const String& str)
	{
		AudioChannelSet set;
		PackedStringArray arr = str.split("\n\r\t");
		for (auto& abbr : arr)
		{
			auto type = getChannelTypeFromAbbreviation(abbr);

			if (type != unknown)
				set.addChannel(type);
		}

		return set;
	}

	String AudioChannelSet::getDescription() const
	{
		if (isDiscreteLayout())            return "Discrete #" + String::num(size(), 0);
		if (*this == disabled())           return "Disabled";
		if (*this == mono())               return "Mono";
		if (*this == stereo())             return "Stereo";

		if (*this == createLCR())          return "LCR";
		if (*this == createLRS())          return "LRS";
		if (*this == createLCRS())         return "LCRS";

		if (*this == create5point0())       return "5.0 Surround";
		if (*this == create5point1())       return "5.1 Surround";
		if (*this == create5point1point2()) return "5.1.2 Surround";
		if (*this == create5point1point4()) return "5.1.4 Surround";
		if (*this == create6point0())       return "6.0 Surround";
		if (*this == create6point1())       return "6.1 Surround";
		if (*this == create6point0Music())  return "6.0 (Music) Surround";
		if (*this == create6point1Music())  return "6.1 (Music) Surround";
		if (*this == create7point0())       return "7.0 Surround";
		if (*this == create7point1())       return "7.1 Surround";
		if (*this == create7point0SDDS())   return "7.0 Surround SDDS";
		if (*this == create7point1SDDS())   return "7.1 Surround SDDS";
		if (*this == create7point0point2()) return "7.0.2 Surround";
		if (*this == create7point0point4()) return "7.0.4 Surround";
		if (*this == create7point1point2()) return "7.1.2 Surround";
		if (*this == create7point1point4()) return "7.1.4 Surround";
		if (*this == create7point1point6()) return "7.1.6 Surround";
		if (*this == create9point1point6()) return "9.1.6 Surround";

		if (*this == quadraphonic())       return "Quadraphonic";
		if (*this == pentagonal())         return "Pentagonal";
		if (*this == hexagonal())          return "Hexagonal";
		if (*this == octagonal())          return "Octagonal";

		// ambisonics
		{
			auto order = getAmbisonicOrder();

			if (order >= 0)
			{
				String suffix;

				switch (order)
				{
				case 1:  suffix = "st"; break;
				case 2:  suffix = "nd"; break;
				case 3:  suffix = "rd"; break;
				default: suffix = "th"; break;
				}

				return String::num(order, 0) + suffix + " Order Ambisonics";
			}
		}

		return "Unknown";
	}

	bool AudioChannelSet::isDiscreteLayout() const noexcept
	{
		for (auto& speaker : getChannelTypes())
			if (speaker <= ambisonicACN35)
				return false;

		return true;
	}

	int AudioChannelSet::size() const noexcept
	{
		return channels.countNumberOfSetBits();
	}

	AudioChannelSet::ChannelType AudioChannelSet::getTypeOfChannel(int index) const noexcept
	{
		int bit = channels.findNextSetBit(0);

		for (int i = 0; i < index && bit >= 0; ++i)
			bit = channels.findNextSetBit(bit + 1);

		return static_cast<ChannelType> (bit);
	}

	int AudioChannelSet::getChannelIndexForType(AudioChannelSet::ChannelType type) const noexcept
	{
		int idx = 0;

		for (int bit = channels.findNextSetBit(0); bit >= 0; bit = channels.findNextSetBit(bit + 1))
		{
			if (static_cast<ChannelType> (bit) == type)
				return idx;

			idx++;
		}

		return -1;
	}

	Vector<AudioChannelSet::ChannelType> AudioChannelSet::getChannelTypes() const
	{
		Vector<ChannelType> result;

		for (int bit = channels.findNextSetBit(0); bit >= 0; bit = channels.findNextSetBit(bit + 1))
			result.append(static_cast<ChannelType> (bit));

		return result;
	}

	void AudioChannelSet::addChannel(ChannelType newChannel)
	{
		const int bit = static_cast<int> (newChannel);
		jassert(bit >= 0 && bit < 1024);
		channels.setBit(bit);
	}

	void AudioChannelSet::removeChannel(ChannelType newChannel)
	{
		const int bit = static_cast<int> (newChannel);
		jassert(bit >= 0 && bit < 1024);
		channels.clearBit(bit);
	}

	AudioChannelSet AudioChannelSet::disabled() { return {}; }
	AudioChannelSet AudioChannelSet::mono() { return AudioChannelSet({ centre }); }
	AudioChannelSet AudioChannelSet::stereo() { return AudioChannelSet({ left, right }); }
	AudioChannelSet AudioChannelSet::createLCR() { return AudioChannelSet({ left, right, centre }); }
	AudioChannelSet AudioChannelSet::createLRS() { return AudioChannelSet({ left, right, surround }); }
	AudioChannelSet AudioChannelSet::createLCRS() { return AudioChannelSet({ left, right, centre, surround }); }
	AudioChannelSet AudioChannelSet::create5point0() { return AudioChannelSet({ left, right, centre, leftSurround, rightSurround }); }
	AudioChannelSet AudioChannelSet::create5point1() { return AudioChannelSet({ left, right, centre, LFE, leftSurround, rightSurround }); }
	AudioChannelSet AudioChannelSet::create6point0() { return AudioChannelSet({ left, right, centre, leftSurround, rightSurround, centreSurround }); }
	AudioChannelSet AudioChannelSet::create6point1() { return AudioChannelSet({ left, right, centre, LFE, leftSurround, rightSurround, centreSurround }); }
	AudioChannelSet AudioChannelSet::create6point0Music() { return AudioChannelSet({ left, right, leftSurround, rightSurround, leftSurroundSide, rightSurroundSide }); }
	AudioChannelSet AudioChannelSet::create6point1Music() { return AudioChannelSet({ left, right, LFE, leftSurround, rightSurround, leftSurroundSide, rightSurroundSide }); }
	AudioChannelSet AudioChannelSet::create7point0() { return AudioChannelSet({ left, right, centre, leftSurroundSide, rightSurroundSide, leftSurroundRear, rightSurroundRear }); }
	AudioChannelSet AudioChannelSet::create7point0SDDS() { return AudioChannelSet({ left, right, centre, leftSurround, rightSurround, leftCentre, rightCentre }); }
	AudioChannelSet AudioChannelSet::create7point1() { return AudioChannelSet({ left, right, centre, LFE, leftSurroundSide, rightSurroundSide, leftSurroundRear, rightSurroundRear }); }
	AudioChannelSet AudioChannelSet::create7point1SDDS() { return AudioChannelSet({ left, right, centre, LFE, leftSurround, rightSurround, leftCentre, rightCentre }); }
	AudioChannelSet AudioChannelSet::quadraphonic() { return AudioChannelSet({ left, right, leftSurround, rightSurround }); }
	AudioChannelSet AudioChannelSet::pentagonal() { return AudioChannelSet({ left, right, centre, leftSurroundRear, rightSurroundRear }); }
	AudioChannelSet AudioChannelSet::hexagonal() { return AudioChannelSet({ left, right, centre, centreSurround, leftSurroundRear, rightSurroundRear }); }
	AudioChannelSet AudioChannelSet::octagonal() { return AudioChannelSet({ left, right, centre, leftSurround, rightSurround, centreSurround, wideLeft, wideRight }); }
	AudioChannelSet AudioChannelSet::create5point1point2() { return AudioChannelSet({ left, right, centre, LFE, leftSurround, rightSurround, topSideLeft, topSideRight }); }
	AudioChannelSet AudioChannelSet::create5point1point4() { return AudioChannelSet({ left, right, centre, LFE, leftSurround, rightSurround, topFrontLeft, topFrontRight, topRearLeft, topRearRight }); }
	AudioChannelSet AudioChannelSet::create7point0point2() { return AudioChannelSet({ left, right, centre, leftSurroundSide, rightSurroundSide, leftSurroundRear, rightSurroundRear, topSideLeft, topSideRight }); }
	AudioChannelSet AudioChannelSet::create7point1point2() { return AudioChannelSet({ left, right, centre, LFE, leftSurroundSide, rightSurroundSide, leftSurroundRear, rightSurroundRear, topSideLeft, topSideRight }); }
	AudioChannelSet AudioChannelSet::create7point0point4() { return AudioChannelSet({ left, right, centre, leftSurroundSide, rightSurroundSide, leftSurroundRear, rightSurroundRear, topFrontLeft, topFrontRight, topRearLeft, topRearRight }); }
	AudioChannelSet AudioChannelSet::create7point1point4() { return AudioChannelSet({ left, right, centre, LFE, leftSurroundSide, rightSurroundSide, leftSurroundRear, rightSurroundRear, topFrontLeft, topFrontRight, topRearLeft, topRearRight }); }
	AudioChannelSet AudioChannelSet::create7point1point6() { return AudioChannelSet({ left, right, centre, LFE, leftSurroundSide, rightSurroundSide, leftSurroundRear, rightSurroundRear, topFrontLeft, topFrontRight, topSideLeft, topSideRight, topRearLeft, topRearRight }); }
	AudioChannelSet AudioChannelSet::create9point1point6() { return AudioChannelSet({ left, right, centre, LFE, leftSurroundSide, rightSurroundSide, leftSurroundRear, rightSurroundRear, wideLeft, wideRight, topFrontLeft, topFrontRight, topSideLeft, topSideRight, topRearLeft, topRearRight }); }

	AudioChannelSet AudioChannelSet::ambisonic(int order)
	{
		if (order < 0)
			order = 0;
		if (order > 5)
			order = 5;
		//jassert(isPositiveAndBelow(order, 6));

		if (order == 0)
			return AudioChannelSet((uint32)(1 << ambisonicACN0));

		AudioChannelSet set((1u << ambisonicACN0) | (1u << ambisonicACN1) | (1u << ambisonicACN2) | (1u << ambisonicACN3));

		auto numAmbisonicChannels = (order + 1) * (order + 1);
		set.channels.setRange(ambisonicACN4, numAmbisonicChannels - 4, true);

		return set;
	}

	int AudioChannelSet::getAmbisonicOrder() const
	{
		auto ambisonicOrder = getAmbisonicOrderForNumChannels(size());

		if (ambisonicOrder >= 0)
			return (*this == ambisonic(ambisonicOrder) ? ambisonicOrder : -1);

		return -1;
	}

	AudioChannelSet AudioChannelSet::discreteChannels(int numChannels)
	{
		AudioChannelSet s;
		s.channels.setRange(discreteChannel0, numChannels, true);
		return s;
	}

	AudioChannelSet AudioChannelSet::canonicalChannelSet(int numChannels)
	{
		if (numChannels == 1)  return AudioChannelSet::mono();
		if (numChannels == 2)  return AudioChannelSet::stereo();
		if (numChannels == 3)  return AudioChannelSet::createLCR();
		if (numChannels == 4)  return AudioChannelSet::quadraphonic();
		if (numChannels == 5)  return AudioChannelSet::create5point0();
		if (numChannels == 6)  return AudioChannelSet::create5point1();
		if (numChannels == 7)  return AudioChannelSet::create7point0();
		if (numChannels == 8)  return AudioChannelSet::create7point1();

		return discreteChannels(numChannels);
	}

	AudioChannelSet AudioChannelSet::namedChannelSet(int numChannels)
	{
		if (numChannels == 1)  return AudioChannelSet::mono();
		if (numChannels == 2)  return AudioChannelSet::stereo();
		if (numChannels == 3)  return AudioChannelSet::createLCR();
		if (numChannels == 4)  return AudioChannelSet::quadraphonic();
		if (numChannels == 5)  return AudioChannelSet::create5point0();
		if (numChannels == 6)  return AudioChannelSet::create5point1();
		if (numChannels == 7)  return AudioChannelSet::create7point0();
		if (numChannels == 8)  return AudioChannelSet::create7point1();

		return {};
	}

	Vector<AudioChannelSet> AudioChannelSet::channelSetsWithNumberOfChannels(int numChannels)
	{
		Vector<AudioChannelSet> retval;

		if (numChannels != 0)
		{
			retval.append(AudioChannelSet::discreteChannels(numChannels));

			retval.append_array([numChannels]() -> Vector<AudioChannelSet>
				{
					switch (numChannels)
					{
					case 1:
						return { AudioChannelSet::mono() };
					case 2:
						return { AudioChannelSet::stereo() };
					case 3:
						return { AudioChannelSet::createLCR(),
								 AudioChannelSet::createLRS() };
					case 4:
						return { AudioChannelSet::quadraphonic(),
								 AudioChannelSet::createLCRS() };
					case 5:
						return { AudioChannelSet::create5point0(),
								 AudioChannelSet::pentagonal() };
					case 6:
						return { AudioChannelSet::create5point1(),
								 AudioChannelSet::create6point0(),
								 AudioChannelSet::create6point0Music(),
								 AudioChannelSet::hexagonal() };
					case 7:
						return { AudioChannelSet::create7point0(),
								 AudioChannelSet::create7point0SDDS(),
								 AudioChannelSet::create6point1(),
								 AudioChannelSet::create6point1Music() };
					case 8:
						return { AudioChannelSet::create7point1(),
								 AudioChannelSet::create7point1SDDS(),
								 AudioChannelSet::octagonal(),
								 AudioChannelSet::create5point1point2() };
					case 9:
						return { AudioChannelSet::create7point0point2() };
					case 10:
						return { AudioChannelSet::create5point1point4(),
								 AudioChannelSet::create7point1point2() };
					case 11:
						return { AudioChannelSet::create7point0point4() };
					case 12:
						return { AudioChannelSet::create7point1point4() };
					case 14:
						return { AudioChannelSet::create7point1point6() };
					case 16:
						return { AudioChannelSet::create9point1point6() };
					}

					return {};
				}());

			auto order = getAmbisonicOrderForNumChannels(numChannels);
			if (order >= 0)
				retval.append(AudioChannelSet::ambisonic(order));
		}

		return retval;
	}

	AudioChannelSet JUCE_CALLTYPE AudioChannelSet::channelSetWithChannels(const Vector<ChannelType>& channelArray)
	{
		AudioChannelSet set;

		for (auto ch : channelArray)
		{
			if (!set.channels[static_cast<int> (ch)])
			{
				ERR_PRINT("Channel Set Error");
				continue;
			}

			set.addChannel(ch);
		}

		return set;
	}

	//==============================================================================
	AudioChannelSet JUCE_CALLTYPE AudioChannelSet::fromWaveChannelMask(int32 dwChannelMask)
	{
		return AudioChannelSet(static_cast<uint32> ((dwChannelMask & ((1 << 18) - 1)) << 1));
	}

	int32 AudioChannelSet::getWaveChannelMask() const noexcept
	{
		if (channels.getHighestBit() > topRearRight)
			return -1;

		return (channels.toInteger() >> 1);
	}

	//==============================================================================
	int JUCE_CALLTYPE AudioChannelSet::getAmbisonicOrderForNumChannels(int numChannels)
	{
		auto sqrtMinusOne = std::sqrt(static_cast<float> (numChannels)) - 1.0f;
		auto ambisonicOrder = jmax(0, static_cast<int> (std::floor(sqrtMinusOne)));

		if (ambisonicOrder > 5)
			return -1;

		return (static_cast<float> (ambisonicOrder) == sqrtMinusOne ? ambisonicOrder : -1);
	}


	//==============================================================================
	//==============================================================================
#if JUCE_UNIT_TESTS

	class AudioChannelSetUnitTest : public UnitTest
	{
	public:
		AudioChannelSetUnitTest()
			: UnitTest("AudioChannelSetUnitTest", UnitTestCategories::audio)
		{}

		void runTest() override
		{
			auto max = AudioChannelSet::maxChannelsOfNamedLayout;

			beginTest("maxChannelsOfNamedLayout is non-discrete");
			expect(AudioChannelSet::channelSetsWithNumberOfChannels(max).size() >= 2);

			beginTest("channelSetsWithNumberOfChannels returns correct speaker count");
			{
				for (auto ch = 1; ch <= max; ++ch)
				{
					auto channelSets = AudioChannelSet::channelSetsWithNumberOfChannels(ch);

					for (auto set : channelSets)
						expect(set.size() == ch);
				}
			}

			beginTest("Ambisonics");
			{
				uint64 mask = 0;

				mask |= (1ull << AudioChannelSet::ambisonicACN0);
				checkAmbisonic(mask, 0, "0th Order Ambisonics");

				mask |= (1ull << AudioChannelSet::ambisonicACN1) | (1ull << AudioChannelSet::ambisonicACN2) | (1ull << AudioChannelSet::ambisonicACN3);
				checkAmbisonic(mask, 1, "1st Order Ambisonics");

				mask |= (1ull << AudioChannelSet::ambisonicACN4) | (1ull << AudioChannelSet::ambisonicACN5) | (1ull << AudioChannelSet::ambisonicACN6)
					| (1ull << AudioChannelSet::ambisonicACN7) | (1ull << AudioChannelSet::ambisonicACN8);
				checkAmbisonic(mask, 2, "2nd Order Ambisonics");

				mask |= (1ull << AudioChannelSet::ambisonicACN9) | (1ull << AudioChannelSet::ambisonicACN10) | (1ull << AudioChannelSet::ambisonicACN11)
					| (1ull << AudioChannelSet::ambisonicACN12) | (1ull << AudioChannelSet::ambisonicACN13) | (1ull << AudioChannelSet::ambisonicACN14)
					| (1ull << AudioChannelSet::ambisonicACN15);
				checkAmbisonic(mask, 3, "3rd Order Ambisonics");

				mask |= (1ull << AudioChannelSet::ambisonicACN16) | (1ull << AudioChannelSet::ambisonicACN17) | (1ull << AudioChannelSet::ambisonicACN18)
					| (1ull << AudioChannelSet::ambisonicACN19) | (1ull << AudioChannelSet::ambisonicACN20) | (1ull << AudioChannelSet::ambisonicACN21)
					| (1ull << AudioChannelSet::ambisonicACN22) | (1ull << AudioChannelSet::ambisonicACN23) | (1ull << AudioChannelSet::ambisonicACN24);
				checkAmbisonic(mask, 4, "4th Order Ambisonics");

				mask |= (1ull << AudioChannelSet::ambisonicACN25) | (1ull << AudioChannelSet::ambisonicACN26) | (1ull << AudioChannelSet::ambisonicACN27)
					| (1ull << AudioChannelSet::ambisonicACN28) | (1ull << AudioChannelSet::ambisonicACN29) | (1ull << AudioChannelSet::ambisonicACN30)
					| (1ull << AudioChannelSet::ambisonicACN31) | (1ull << AudioChannelSet::ambisonicACN32) | (1ull << AudioChannelSet::ambisonicACN33)
					| (1ull << AudioChannelSet::ambisonicACN34) | (1ull << AudioChannelSet::ambisonicACN35);
				checkAmbisonic(mask, 5, "5th Order Ambisonics");
			}
		}

	private:
		void checkAmbisonic(uint64 mask, int order, const char* layoutName)
		{
			auto expected = AudioChannelSet::ambisonic(order);
			auto numChannels = expected.size();

			expect(numChannels == BigInteger((int64)mask).countNumberOfSetBits());
			expect(channelSetFromMask(mask) == expected);

			expect(order == expected.getAmbisonicOrder());
			expect(expected.getDescription() == layoutName);

			auto layouts = AudioChannelSet::channelSetsWithNumberOfChannels(numChannels);
			expect(layouts.contains(expected));

			for (auto layout : layouts)
				expect(layout.getAmbisonicOrder() == (layout == expected ? order : -1));
		}

		static AudioChannelSet channelSetFromMask(uint64 mask)
		{
			Array<AudioChannelSet::ChannelType> channels;
			for (int bit = 0; bit <= 62; ++bit)
				if ((mask & (1ull << bit)) != 0)
					channels.add(static_cast<AudioChannelSet::ChannelType> (bit));

			return AudioChannelSet::channelSetWithChannels(channels);
		}
	};

	static AudioChannelSetUnitTest audioChannelSetUnitTest;

#endif

} // namespace juce
