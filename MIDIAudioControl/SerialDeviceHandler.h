#pragma once
#include <string>
#include <atlstr.h>
#include <windows.h>
#include <setupapi.h>

#ifndef serialdevicehandler_hpp
#define serialdevicehandler_hpp

class SerialDeviceHandler {
public:
	SerialDeviceHandler();
	~SerialDeviceHandler();

	void EnumerateSerialDevices();
	bool Connect(const std::string& deviceName, const int baudRate);
	void ReadData();

private:
	void PrintError(const char* errMsg);
	std::string GetDeviceName(const HDEVINFO& deviceInfoSet, const PSP_DEVINFO_DATA& deviceInfoData);

	HANDLE hSerial = INVALID_HANDLE_VALUE;
};
#endif