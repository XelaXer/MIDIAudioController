#pragma once
#include <iostream>
#include <string>
#include <conio.h>
#include <windows.h>
#include "Actuator.h"

#ifndef knob_hpp
#define knob_hpp

class Knob : public Actuator {

public:

};

class VolumeKnob : public Knob {
	std::string processName;
	unsigned int audioSessionIndex;

public:
	VolumeKnob();
	// Access Process Name
	std::string getProcessName();
	void setProcessName(std::string);
	// Access Audio Session Index
	unsigned int getAudioSessionIndex();
	void setAudioSessionIndex(unsigned int);

};

#endif