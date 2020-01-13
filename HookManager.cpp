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
	// for cheat engine reference: 8b 0d c0 9b 8d 00 81 c1 37
	static const char*	hookMask		= "xxxxxxxxxxxxxx";
	static int			length			= 39;
	static DWORD		start			= 0x30000000;
	static DWORD		size			= 0x01000000;
	DWORD				old				= 0x0;

	printf("inside hookthread\r\n");
	DWORD hookLocation = 0x0;
	std::vector<int> completed;
	while (true) {
		printf("doing iteration\r\n");
		for (int i = 0; i < manager->hooks.size(); i++) {
			if (find(i, &completed))
				continue;
			if (!SignatureScanner::FindSignature(&hookLocation, start, size, manager->hooks.at(i).hookString, hookMask, 0)) {
				continue;
			}
			printf("found hook for %d\r\n", i);

			completed.push_back(i);

			VirtualProtect((void*)hookLocation, length, PAGE_EXECUTE_READWRITE, &old);

			memset((void*)hookLocation, 0x90, length);
			*(BYTE*)hookLocation = 0xE9;
			*(DWORD*)(hookLocation + 1) = ((DWORD)manager->hooks.at(i).functionPointer - hookLocation - 5);
			VirtualProtect((void*)hookLocation, length, old, nullptr);

			*(manager->hooks.at(i).retAddress) = (stdHook)(hookLocation + 6);
		}
		if (completed.size() == manager->hooks.size())
			break;
		Sleep(200);
	}
}

void HookManager::HookAll(HMODULE hmodule) {
	for (int i = 0; i < hooks.size(); i++) {
		hooks.at(i).Hook();
	}
	manager = this;
	CreateThread(0, 0, HookThread, hmodule, 0, 0);
};


HookMember* HookManager::Get(char id) {
	return &this->hooks.at(id);
}

void HookMember::Hook() {
	static const char* bytesToReplace	= "\x37\x13\x9B\x8D"; /* 9b was 8f for t7 */
	sprintf(this->hookString, "\x8B\x0D\xC0\x9B\x8D\x00\x81\xC1%d\x13\x00\x00\x89\xC8", this->handle);
	DWORD old = 0x0;

	VirtualProtect(baseAddress, strlen(bytesToReplace), PAGE_EXECUTE_READWRITE, &old);
	memcpy(baseAddress, bytesToReplace, strlen(bytesToReplace));
	VirtualProtect(baseAddress, strlen(bytesToReplace), old, nullptr);
}

HookMember::HookMember(void* baseAddress, void* functionPointer, stdHook* ret, int handle) {
	this->baseAddress = baseAddress;
	this->handle = handle;
	this->functionPointer = functionPointer;
	this->retAddress = ret;
}