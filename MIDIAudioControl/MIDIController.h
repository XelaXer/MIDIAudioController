#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include "Actuator.h"
#include "Knob.h"
#include "Pad.h"

#ifndef midicontroller_hpp
#define midicontroller_hpp

class MIDIController {
	bool newSignal;
	std::string type;
	std::vector<Knob> knobs;
	std::vector<VolumeKnob> volumeknobs;

public:
	//std::vector<Actuator*> actuators;
	std::vector<Pad> pads;
	std::vector<bool> actuatorChanged;

	// Constructors
	MIDIController();
	MIDIController(std::string);
	
	// Access Type
	std::string getType();
	void setType(std::string);
	bool getNewSignal();
	void setNewSignal(bool);
	int getVolumeKnobValue(std::string);
	VolumeKnob& getVolumeKnob(int);

	// Functions

	void parseSignal(int);
	void parseMessage(UINT);

	void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	
	void setVolumeKnobProcessName(int, std::string);	// Index of volume knob, Process Name
	unsigned int getVolumeKnobSize();
	
};

#endif

