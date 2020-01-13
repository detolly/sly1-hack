#include "pch.h"
#include "HookManager.h"
#include "sigscan.h"
#include "vector"

HookManager::HookManager() { count = 0; }

int HookManager::AddHook(HookMember h) {
	int id = this->count++;
	this->hooks.insert(std::make_pair(id, h));
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
	static const char*	hookString		= "\x8B\x0D\xC0\x9B\x8D\x00\x81\xC1\x37\x13\x00\x00\x89\xC8";
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

			if (!SignatureScanner::FindSignature(&hookLocation, start, size, hookString, hookMask, 0)) {
				continue;
			}

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
	printf("got here 3");
	for (int i = 0; i < hooks.size(); i++) {
		hooks.at(i).Hook();
	}
	printf("got here 1.5");
	manager = this;
	printf("got here 2");
	CreateThread(0, 0, HookThread, hmodule, 0, 0);
	printf("got here 3");
};


HookMember* HookManager::Get(int id) {
	return &this->hooks.at(id);
}

void HookMember::Hook() {
	static const char* bytesToReplace	= "\x37\x13\x9B\x8D"; /* 9b was 8f for t7 */
	DWORD old = 0x0;
	printf("got here 5");

	VirtualProtect(baseAddress, strlen(bytesToReplace), PAGE_EXECUTE_READWRITE, &old);
	memcpy(oldBytes, baseAddress, strlen(bytesToReplace));
	memcpy(baseAddress, bytesToReplace, strlen(bytesToReplace));
	VirtualProtect(baseAddress, strlen(bytesToReplace), old, nullptr);
	printf("got here 6");
}

HookMember::HookMember(void* baseAddress, void* functionPointer, stdHook* ret) {
	this->baseAddress = baseAddress;
	this->functionPointer = functionPointer;
	this->retAddress = ret;
}