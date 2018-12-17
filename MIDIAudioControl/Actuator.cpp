#include "stdafx.h"
#include "Actuator.h"

void Actuator::setID(int x) {
	id = x;
}

int Actuator::getID() {
	return id;
}

void Actuator::setValue(int x) {
	value = x;
}

int Actuator::getValue() {
	return value;
}