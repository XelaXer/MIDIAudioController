#pragma once
#include <iostream>
#include <string>

#ifndef actuator_hpp
#define actuator_hpp

class Actuator {
	int id;
	int value;
	int minValue;
	int maxValue;


public:

	// Access ID
	int getID();
	void setID(int);
	// Access Value
	int getValue();
	void setValue(int);
	// Access Minimum Value
	int getminValue();
	void setminValue(int);
	// Access Maximum Value
	int getmaxValue();
	void setmaxValue(int);

};








#endif