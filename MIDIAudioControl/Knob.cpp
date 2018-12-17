#include "stdafx.h"
#include "Knob.h"

unsigned int VolumeKnob::getAudioSessionIndex() {
	return audioSessionIndex;
}

void VolumeKnob::setAudioSessionIndex(unsigned int i) {
	audioSessionIndex = i;
}

std::string VolumeKnob::getProcessName() {
	return processName;
}
void VolumeKnob::setProcessName(std::string pr) {
	processName = pr;
}

VolumeKnob::VolumeKnob() {
	audioSessionIndex = 420;
	processName = "N/A";
}