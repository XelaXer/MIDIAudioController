#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include "Actuator.h"
#include "Knob.h"
#include "Pad.h"
#include "Controller.h"

Controller::Controller(std::string type) : actuatorChanged(16, 0), volumeknobs(8), pads(8) {
	newSignal = false;
	
	
	// loop setID
}

VolumeKnob& Controller::getVolumeKnob(int index) {
	return volumeknobs[index];
}

int Controller::getVolumeKnobValue(std::string pr) {
	for (std::vector<VolumeKnob>::iterator itr = volumeknobs.begin(); itr != volumeknobs.end(); itr++) {
		if (itr->getProcessName() == pr) {
			return itr->getValue();
		}
	}
}

bool Controller::getNewSignal() {
	return newSignal;
}

void Controller::setNewSignal(bool s) {
	newSignal = s;
}

void Controller::parseMessage(UINT wMsg) {
	std::cout << "Message Parsed: ";
	std::cout << wMsg << std::endl;
}

void Controller::parseSignal(int key) {
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

void Controller::setVolumeKnobProcessName(int index, std::string pn) {
	volumeknobs[index].setProcessName(pn);
}

unsigned int Controller::getVolumeKnobSize() {
	return volumeknobs.size();
}