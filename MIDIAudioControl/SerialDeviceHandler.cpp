#include "SerialDeviceHandler.h"
#include <iostream>
#include <devguid.h>

SerialDeviceHandler::SerialDeviceHandler() = default;
SerialDeviceHandler::~SerialDeviceHandler() {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
    }
}

void SerialDeviceHandler::PrintError(const char* errMsg) {
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&lpMsgBuf,
        0,
        NULL
    );
    std::cerr << errMsg << ": " << (LPCSTR)lpMsgBuf << std::endl;
    LocalFree(lpMsgBuf);
}

std::string SerialDeviceHandler::GetDeviceName(const HDEVINFO& deviceInfoSet, const PSP_DEVINFO_DATA& deviceInfoData) {
    DWORD propertyDataType;
    BYTE propertyBuffer[1024];
    if (!SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, deviceInfoData, SPDRP_FRIENDLYNAME, &propertyDataType, propertyBuffer, sizeof(propertyBuffer), NULL) &&
        !SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, deviceInfoData, SPDRP_DEVICEDESC, &propertyDataType, propertyBuffer, sizeof(propertyBuffer), NULL)) {
        PrintError("Failed to get device name");
        return "";
    }

    std::string deviceFriendlyName((char*)propertyBuffer);
    size_t start = deviceFriendlyName.find("(COM");
    if (start == std::string::npos) {
        return "";
    }

    size_t end = deviceFriendlyName.find(")", start);
    if (end == std::string::npos) {
        return "";
    }

    return deviceFriendlyName.substr(start + 1, end - start - 1);
}

void SerialDeviceHandler::EnumerateSerialDevices() {
    HDEVINFO deviceInfoSet = SetupDiGetClassDevsA(&GUID_DEVCLASS_PORTS, NULL, NULL, DIGCF_PRESENT);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        PrintError("Failed to get device information set");
        return;
    }

    for (int i = 0; ; ++i) {
        SP_DEVINFO_DATA deviceInfoData;
        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        if (!SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData)) {
            if (GetLastError() == ERROR_NO_MORE_ITEMS) {
                break;
            }
            else {
                PrintError("Failed to get device info data");
                continue;
            }
        }

        std::string deviceName = GetDeviceName(deviceInfoSet, &deviceInfoData);
        if (!deviceName.empty()) {
            std::cout << i << ": " << deviceName << std::endl;
        }
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
}

bool SerialDeviceHandler::Connect(const std::string& deviceName, const int baudRate) {
    std::string fullDeviceName = "\\\\.\\" + deviceName;
    HANDLE hSerial = CreateFileA(fullDeviceName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        PrintError("Failed to open serial port");
        return INVALID_HANDLE_VALUE;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        PrintError("Failed to get serial port state");
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        PrintError("Failed to set serial port state");
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        PrintError("Failed to set serial port timeouts");
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    return hSerial;
}

void SerialDeviceHandler::ReadData() {
    const DWORD bufferSize = 1024;
    char buffer[bufferSize];
    DWORD bytesRead;
    std::string receivedData = "";
    while (true) {
        if (!ReadFile(hSerial, buffer, bufferSize - 1, &bytesRead, NULL)) {
            PrintError("Failed to read from serial port");
            break;
        }

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            receivedData += buffer;
            size_t pos;
            while ((pos = receivedData.find('\n')) != std::string::npos) {
                std::string event = receivedData.substr(0, pos);
                receivedData.erase(0, pos + 1);
                std::cout << "Event: " << event << std::endl;
            }
        }

        Sleep(50); // Poll every 50 millisecond // TODO: Use WaitForSingleObjectEx // TODO: make a param
    }
}