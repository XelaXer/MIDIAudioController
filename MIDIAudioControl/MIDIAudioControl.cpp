/*
	MIDIAudioControl.cpp (Main File) : 
		Defines the entry point for the console application.

	WindowAudioController .cpp/.h : 
		Defines a class (C_WindowsAudioVolumeController) that interfaces with the Windows Core Audio API.
		Defines a class (S_AudioSession) 





*/




#include "stdafx.h"

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

C_WindowsAudioVolumeController audioController;
std::vector<S_AudioSession> audioSessions;
MIDIController midi("AKAIMPKMINI");
HMIDIIN hMidiDevice = NULL;
UINT nMidiDeviceNum;
bool retryConnect = false;

std::vector<std::string> games = { "RelicCoH2.exe", "RainbowSix.exe" , "bf4.exe", "forhonor.exe" , "ForHonor.exe" , "InsurgencyClient-Win64-Shipping.exe" , "BlackOps.exe" , "BlackOpsMP.exe", "TotallyAccurateBattlegrounds.exe", "Gang Beasts.exe" };
std::vector<std::string> programs = { "Video.UI.exe" , "chrome.exe", "Discord.exe" };

// ToDo Functions
/*
void restartMIDI() {}
void hide() {}
*/


void PrintEndpointNames() {
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	HRESULT hr = S_OK;
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDeviceCollection *pCollection = NULL;
	IMMDevice *pEndpoint = NULL;
	IPropertyStore *pProps = NULL;
	LPWSTR pwszID = NULL;

	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

		hr = pEnumerator->EnumAudioEndpoints(
			eRender, DEVICE_STATE_ACTIVE,
			&pCollection);
	EXIT_ON_ERROR(hr)

		UINT  count;
	hr = pCollection->GetCount(&count);
	EXIT_ON_ERROR(hr)

		if (count == 0)
		{
			printf("No endpoints found.\n");
		}


	

	// Each loop prints the name of an endpoint device.
	for (ULONG i = 0; i < count; i++)
	{
		// Get pointer to endpoint number i.
		hr = pCollection->Item(i, &pEndpoint);
		EXIT_ON_ERROR(hr);

		// Get the endpoint ID string.
		hr = pEndpoint->GetId(&pwszID);
		EXIT_ON_ERROR(hr);

		

		// Get Interface
		//hr = pEndpoint->QueryInterface()
		//EXIT_ON_ERROR(hr);



		hr = pEndpoint->OpenPropertyStore(
			STGM_READ, &pProps);
		EXIT_ON_ERROR(hr);

		PROPVARIANT varName;
		// Initialize container for property value.
		PropVariantInit(&varName);

		// Get the endpoint's friendly-name property.
		hr = pProps->GetValue(
			PKEY_Device_FriendlyName, &varName);
		EXIT_ON_ERROR(hr);

		// Print endpoint friendly name and endpoint ID.
		printf("Endpoint %d: \"%S\" (%S)\n",
			i, varName.pwszVal, pwszID);

		CoTaskMemFree(pwszID);
		pwszID = NULL;
		PropVariantClear(&varName);
		SAFE_RELEASE(pProps);
		SAFE_RELEASE(pEndpoint);
	}
	SAFE_RELEASE(pEnumerator);
	SAFE_RELEASE(pCollection);
	return;

Exit:
	printf("Error!\n");
	CoTaskMemFree(pwszID);
	SAFE_RELEASE(pEnumerator)
		SAFE_RELEASE(pCollection)
		SAFE_RELEASE(pEndpoint)
		SAFE_RELEASE(pProps)
}

void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	switch (wMsg) {
	case MIM_OPEN:
		printf("wMsg=MIM_OPEN\n");
		break;
	case MIM_CLOSE:
		printf("wMsg=MIM_CLOSE\n");
		//midi.setNewSignal(true);
		//midi.parseMessage(wMsg);
		break;
	case MIM_DATA:
		printf("wMsg=MIM_DATA, dwInstance=%08x, dwParam1=%08x, dwParam2=%08x\n", dwInstance, dwParam1, dwParam2);
		midi.setNewSignal(true);
		midi.parseSignal(dwParam1);
		break;
	case MIM_LONGDATA:
		printf("wMsg=MIM_LONGDATA\n");
		break;
	case MIM_ERROR:
		printf("wMsg=MIM_ERROR\n");
		break;
	case MIM_LONGERROR:
		printf("wMsg=MIM_LONGERROR\n");
		break;
	case MIM_MOREDATA:
		printf("wMsg=MIM_MOREDATA\n");
		break;
	default:
		printf("wMsg = unknown\n");
		break;
	}
	return;
}

void UpdateSessions() {
	std::vector<S_AudioSession> audioSessions_New;
	audioController.UpdateAudioSessions();
	audioController.GetAudioSessions(audioSessions_New);

	if (audioSessions_New.size() != audioSessions.size()) {
		//std::cout<<"Update sessions"<<std::endl;
		audioSessions.clear();
		audioSessions.shrink_to_fit();
		audioSessions = audioSessions_New;
	}

	for (int i = 0; i < audioSessions.size(); ++i) {
		if (audioSessions.at(i).displayName != audioSessions_New.at(i).displayName) {
			audioSessions.clear();
			audioSessions.shrink_to_fit();
			audioSessions = audioSessions_New;
		}
	}
}

/*
void assign()

Purpose:
Assigns actions to actuators

To-Do:
Currently the function maps knobs to
windows audio sessions. Need to create
a way to generate an action and assign
an actuator to an action very dynamically

*/

void assign() {

	// Iterates through volume knobs and clear their values
	for (int i = 0; i < midi.getVolumeKnobSize(); i++) {
		midi.setVolumeKnobProcessName(i, "NULL");
		midi.getVolumeKnob(i).setAudioSessionIndex(-1);
	}

	// Find current game and assign to first knob
	std::string currentGame = "N/A";
	for (int i = 0; i < audioSessions.size(); i++) {
		for (int j = 0; j < games.size(); j++) {
			if (audioSessions[i].displayName == games[j]) {
				currentGame = audioSessions[i].displayName;
				midi.setVolumeKnobProcessName(0, currentGame);
				midi.getVolumeKnob(0).setAudioSessionIndex(audioSessions[i].originalSessionIndex);
				break;
			}
		}
	}
	std::vector<bool> knobAssigned(midi.getVolumeKnobSize(), false);
	std::vector<bool> programAssigned(programs.size(), false);

	// Assigns desired programs to midi controller
	// Time Complexity (Worst Case): Volume Knobs (8) * Programs (usually max 10) * Sessions (usually max 15) = 1200
	// Time Complexity (Average Case): Volume Knobs (Programs(5))) * Programs (5) * Sessions (6) = 150
	// Time Complexity (User Tested): 32 iterations
	// Space Complexity (Worst Case): Volume Knobs (8) + Programs (usually max 10) = 18 boolean values
	int count = 0;
	for (int i = 1; i < midi.getVolumeKnobSize(); i++) {
		for (int k = 0; k < programs.size(); k++) {
			if (knobAssigned[i] == true) { break; }
			for (int j = 0; j < audioSessions.size(); j++) {
				if (programAssigned[k] == true) { break; }
				count++;
				if (audioSessions[j].displayName == programs[k] && (knobAssigned[i] == false) && (programAssigned[k] == false)) {
					knobAssigned[i] = true;
					programAssigned[k] = true;
					midi.setVolumeKnobProcessName(i, programs[k]);
					midi.getVolumeKnob(i).setAudioSessionIndex(audioSessions[j].originalSessionIndex);
					std::cout << "Knob " << i << " set to process " << programs[k] << " at session index " << audioSessions[j].originalSessionIndex << std::endl;
					break;
				}
			}
		}
	}
	//std::cout << "Count: " << count << std::endl;

	/*

	std::queue<std::string> programQueue;
	for (auto& itr : programs) {
		programQueue.push(itr);
	}

	for (int i = 1; i < midi.getVolumeKnobSize(); i++) {
		for (int j = 0; j < audioSessions.size(); j++) {
			if (programQueue.size() != 0)
				if (audioSessions[j].displayName == programQueue.front()) {
					midi.setVolumeKnobProcessName(i, programQueue.front());
					midi.getVolumeKnob(i).setAudioSessionIndex(audioSessions[j].originalSessionIndex);
					std::cout << "Knob " << i << " set to process " << programQueue.front() << " at session index " << audioSessions[j].originalSessionIndex << std::endl;
					programQueue.pop();
					break;
				}
		}
	}
	*/


}

void initWinMidi() {
	hMidiDevice = NULL;
	DWORD nMidiPort = 0;
	MMRESULT rv;
	nMidiDeviceNum = midiInGetNumDevs();
	if (nMidiDeviceNum == 0) {
		fprintf(stderr, "No MIDI Devices Found");
	}
	rv = midiInOpen(&hMidiDevice, nMidiPort, (DWORD)(void*)MidiInProc, 0, CALLBACK_FUNCTION);
	if (rv != MMSYSERR_NOERROR) {
		fprintf(stderr, "midiInOpen() failed...rv=%d", rv);
	}
	midiInStart(hMidiDevice);
	UINT midiID = 420;

	rv = midiInGetID(hMidiDevice, &midiID);
	std::cout << "MIDI ID: " << midiID << std::endl;

}

void checkMidiConnection() {

	while (true) {
		nMidiDeviceNum = midiInGetNumDevs();
		//std::cout << nMidiDeviceNum << std::endl;
		if (nMidiDeviceNum == 0) {
			std::cout << "Retrying MIDI Connection" << std::endl;
			//retryConnect = true;
			//initWinMidi();
		}
		//else {
		//	retryConnect = false;
		//}
		if (!_kbhit()) {
			Sleep(1000);
			continue;
		}
		/*
		if (nMidiDeviceNum == 0) {
			try {
				retryConnect = true;
				//initWinMidi();
			}
			catch (exception ex) {}
		}
		*/
	}
}

void hideConsole(HWND window) {
	ShowWindow(window, 0);
}

void printAudioSessions() {
	// Prints Name and Index of Audio Sessions
	for (int i = 0; i < audioSessions.size(); i++) {
		std::cout << audioSessions[i].displayName << std::endl;
		std::cout << audioSessions[i].originalSessionIndex << std::endl;
		audioController.SetVolume(audioSessions[i].originalSessionIndex, 1.0);
	}
}

void spareCode() {

	// START NEW CODE

	//const CLSID CLSID_MMDeviceEnumerator11 = __uuidof(MMDeviceEnumerator);
	//const IID IID_IMMDeviceEnumerator11 = __uuidof(IMMDeviceEnumerator);



	IMMDevice* device;
	IAudioClient* client;
	UINT deviceCount;
	HRESULT hr = S_OK;
	IMMDeviceEnumerator *pMMDeviceEnumerator;

	// activate a device enumerator
	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		(void**)&pMMDeviceEnumerator
	);

	if (FAILED(hr)) {
		printf("CoCreateInstance(IMMDeviceEnumerator) failed: hr = 0x%08x", hr);
		//return hr;
	}


	//hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, ppMMDevice);
	hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);

	IMMDeviceCollection *pMMDeviceCollection;

	hr = pMMDeviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pMMDeviceCollection);

	hr = pMMDeviceCollection->GetCount(&deviceCount);

	for (UINT i = 0; i < deviceCount; i++) {

	}

	if (FAILED(hr)) {
		printf("IMMDeviceEnumerator::GetDefaultAudioEndpoint failed: hr = 0x%08x", hr);
		//return hr;
	}

	//return S_OK;

	/*
	HRESULT hResult;
	IMMDevice* defaultAudioEndpointDevice11;
	IMMDeviceEnumerator* deviceEnumerator11;
	CoInitialize(nullptr);
	//Initialize deviceEnumerator
	hResult = CoCreateInstance(CLSID_MMDeviceEnumerator11, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator11, (void**)&deviceEnumerator11);
	ERROR_HANDLE(hResult);

	//Initialize defaultAudioEnpointDevice
	hResult = deviceEnumerator11->GetDefaultAudioEndpoint(eRender, eConsole, &defaultAudioEndpointDevice11);
	ERROR_HANDLE(hResult);
	*/


	// END NEW CODE





}

int main() {

	HWND window;
	AllocConsole();
	window = FindWindowA("ConsoleWindowClass", NULL);
	//hideConsole(std::ref(window));

	// Initialize the audio controller instance (audioController is a global variable)
	audioController.Initialize();

	// Fill the audioSessions vector with the active audio sessions from the Audio Controller
	audioController.GetAudioSessions(audioSessions);

	// Initialize a Windows MIDI controller (picks the first found MIDI Controller)
	// TODO: Update to pick from multiple MIDI devices
	initWinMidi();

	// Get number of MIDI devices (nMidiDeviceNum is a global variable)
	nMidiDeviceNum = midiInGetNumDevs();

	//PrintEndpointNames();

	// Assigns actions to 
	assign();

	bool midiCheck = false;
	while (true) {
		if (midi.getNewSignal()) {
			for (int i = 0; i < midi.actuatorChanged.size(); i++) {
				if (midi.actuatorChanged[i]) {
					// Volume Knobs
					if (i < 8) {
						std::cout << midi.getVolumeKnob(i).getProcessName() << " value = " << midi.getVolumeKnob(i).getValue() << std::endl;
						audioController.SetVolume(midi.getVolumeKnob(i).getAudioSessionIndex(), (midi.getVolumeKnob(i).getValue() / 127.0));
					}
					// Pads
					if (i >= 8 && i < 16) {
						UpdateSessions();
						assign();
					}
					midi.actuatorChanged[i] = 0;
				}
			}
			midi.setNewSignal(false);
		}
		/*
		if (retryConnect == true) {
			std::cout << "retry midi connect" << std::endl;
			initWinMidi();
			retryConnect = false;
			//goto start;

		}
		*/
		/*
		if (nMidiDeviceNum == 0) {
			try {
				initWinMidi();
			}
			catch (exception ex){	}
		}
		*/
		if (!_kbhit()) {
			Sleep(10);
			continue;
		}

		int c = _getch();
		if (c == VK_ESCAPE) break;

	}


	int exit = 0;
	std::cin >> exit;
	audioSessions.clear();
	return 0;
}

