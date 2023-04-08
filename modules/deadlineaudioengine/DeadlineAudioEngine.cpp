/*
  ==============================================================================

    JuceLib.cpp
    Created: 15 Mar 2023 11:48:07pm
    Author:  RedneckJack

  ==============================================================================
*/


#include "DeadlineAudioEngine.h"
//#include <JuceHeader.h>

DeadlineAudioEngine::DeadlineAudioEngine() {}


//Bind all your methods used in this class
void DeadlineAudioEngine::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("initProcessor"), &DeadlineAudioEngine::initProcessor);
}

void DeadlineAudioEngine::initProcessor()
{

}
