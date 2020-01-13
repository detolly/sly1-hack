#include "pch.h"
#include "HookManager.h"
#include "sigscan.h"
#include "vector"

HookManager::HookManager() { count = 0; }

int HookManager::AddHook(void* baseAddr, void* functionAddr, stdHook* retAddr) {
	char id = this->count++;
	this->hooks.insert(std::make_pair(id, HookMember(baseAddr, functionAddr, retAddr, id)));
	return id;
}

HookManager* manager;

bool find(int i, std::vector<int>* c) {
	for (int k = 0; k < c->size(); k++)
		if (c->at(k) == i)
			return true;
	return false;
}

DWORD WINAPI HookThread(LPVOID param) {
	// for cheat engine reference: 8b 0d c0 9b 8d 00 81 c1 37 13
	static const char*	hookMask		= "xxxxxxxxxxxxxx";
	static int			length			= 39;
	static DWORD		start			= 0x30000000;
	static DWORD		size			= 0x01000000;
	DWORD				old				= 0x0;

	DWORD hookLocation = 0x0;
	std::vector<int> completed;
	while (true) {
		for (int i = 0; i < manager->hooks.size(); i++) {
			if (find(i, &completed))
				continue;
			printf("doing iteration\r\n");
			if (!SignatureScanner::FindSignature(&hookLocation, start, size, manager->hooks.at(i).hookString, hookMask, 0)) {
				continue;
			}
			printf("found hook for %d\r\n", i);

			completed.push_back(i);
			*(manager->hooks.at(i).retAddress) = (stdHook)(hookLocation + 6);

			VirtualProtect((void*)hookLocation, length, PAGE_EXECUTE_READWRITE, &old);
			memset((void*)hookLocation, 0x90, length);
			*(BYTE*)hookLocation = 0xE9;
			*(DWORD*)(hookLocation + 1) = ((DWORD)manager->hooks.at(i).functionPointer - hookLocation - 5);
			VirtualProtect((void*)hookLocation, length, old, nullptr);

		}
		if (completed.size() == manager->hooks.size())
			break;
		Sleep(200);
	}
	printf("Hooked everything! have fun\r\n");
	ExitThread(0);
}

void HookManager::HookAll(LPVOID param) {
	manager = this;
	printf("hooks size: %d\r\n", hooks.size());
	for (int i = 0; i < hooks.size(); i++) {
		hooks.at(i).Hook();
	}
	CreateThread(0, 0, HookThread, param, 0, 0);
};


HookMember* HookManager::Get(char id) {
	return &this->hooks.at(id);
}

void HookMember::Hook() {
	DWORD old = 0x0;
	hookString = new char[14] {
		(char)0x8B, (char)0x0D, (char)0xC0, (char)0x9B, (char)0x8D, (char)0x0, (char)0x81, (char)0xC1, handle, (char)0x13, (char)0x0, (char)0x0, (char)0x89, (char)0xC8
	};
	bytesToReplace = new char[4] {
		handle, (char)0x13, (char)0x9B, (char)0x8D
	};
	VirtualProtect(baseAddress, 4, PAGE_EXECUTE_READWRITE, &old);
	memcpy(baseAddress, bytesToReplace, 4);
	VirtualProtect(baseAddress, 4, old, nullptr);
}

HookMember::HookMember(void* baseAddress, void* functionPointer, stdHook* ret, char handle) {
	this->baseAddress = baseAddress;
	this->functionPointer = functionPointer;
	this->retAddress = ret;
	this->handle = handle;
}