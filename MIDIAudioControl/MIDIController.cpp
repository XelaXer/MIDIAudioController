#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include "Actuator.h"
#include "Knob.h"
#include "Pad.h"
#include "MIDIController.h"

MIDIController::MIDIController(std::string type) : actuatorChanged(16,0), volumeknobs(8), pads(8){
	newSignal = false;
	if (type == "AKAIMPKMINI") {
		int id = 0x1b0;
		for (auto& itr : volumeknobs) {
			itr.setID(id);
			id += 256;
		}
		id = 0x2c90;
		for (auto& itr : pads) {
			//std::cout << "Pad set to " << id << std::endl;
			itr.setID(id);
			id += 256;
		}


	}
	// Knob 1
	// 2c90 -> On
	// 2c80 -> Off
	// Knob 2
	// 2d90 -> On
	// 2d80 -> Off
	
	// Pass actuatorId to constructor
	//actuators.push_back(new Pad());
}

VolumeKnob& MIDIController::getVolumeKnob(int index) {
	return volumeknobs[index];
}

int MIDIController::getVolumeKnobValue(std::string pr) {
	for (std::vector<VolumeKnob>::iterator itr = volumeknobs.begin(); itr != volumeknobs.end(); itr++) {
		if (itr->getProcessName() == pr) {
			return itr->getValue();
		}
	}
}

bool MIDIController::getNewSignal() {
	return newSignal;
}

void MIDIController::setNewSignal(bool s) {
	newSignal = s;
}

void MIDIController::parseMessage(UINT wMsg) {
	std::cout << "Message Parsed: ";
	std::cout << wMsg << std::endl;
}

void MIDIController::parseSignal(int key) {
	int act = key & 0xffff;	// Actuator ID
	int value = (key & 0xffff0000) >> 16;	// Range: 0 to 127 in decimal
	float iValue = ((float)value / 127.0); // * 100.0;
	int index = 0;
	for (std::vector<VolumeKnob>::iterator itr = volumeknobs.begin(); itr != volumeknobs.end(); itr++, index++) {
		if (itr->getID() == act) {
			itr->setValue(value);
			actuatorChanged[index] = true;
		}
	}
	for (std::vector<Pad>::iterator itr = pads.begin(); itr != pads.end(); itr++, index++) {
		if (itr->getID() == act) {
			itr->setValue(value);
			actuatorChanged[index] = true;
		}
	}
	/*
	for (std::vector<Knob>::iterator itr = knobs.begin(); itr != knobs.end(); itr++) {
		if (itr->getID() == act) {
			itr->setValue(value);
			std::cout << "Knob " << itr->getID() << " set to" << itr->getValue() << std::endl;
		}
	}
	*/

}

void MIDIController::setVolumeKnobProcessName(int index, std::string pn) {
	volumeknobs[index].setProcessName(pn);
}

unsigned int MIDIController::getVolumeKnobSize() {
	return volumeknobs.size();
}