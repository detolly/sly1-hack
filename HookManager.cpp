#include "pch.h"
#include "HookManager.h"
#include "sigscan.h"
#include "vector"

HookManager::HookManager() : count(0) {
	hooks = new map<char, HookMember*>();
}

HookManager::~HookManager() {
	delete hooks;
}

int HookManager::AddHook(void* baseAddr, void* functionAddr, stdHook* retAddr) {
	char id = this->count++;
	HookMember* member = new HookMember(baseAddr, functionAddr, retAddr, id);
	this->hooks->insert(std::make_pair(id, member));
	return id;
}

HookManager* hmanager;

bool find(int i, std::vector<int>* c) {
	for (int k = 0; k < c->size(); k++)
		if (c->at(k) == i)
			return true;
	return false;
}

DWORD WINAPI HookThread(LPVOID param) {
	// for cheat engine reference: 8b 0d c0 9b 8d 00 81 c1 37 13
	static const char*	hookMask		= "xx????xxxxxxxx";
	static int			length			= 39;
	static DWORD		start			= 0x30000000;
	static DWORD		size			= 0x01000000;
	DWORD				old				= 0x0;

	DWORD hookLocation = 0x0;
	std::vector<int> completed;
	while (true) {
		for (int i = 0; i < hmanager->hooks->size(); i++) {
			if (find(i, &completed))
				continue;
			if (!SignatureScanner::FindSignature(&hookLocation, 0x30000000, 0x01000000, hmanager->Get(i)->hookString, hookMask, 0)) {
				continue;
			}
			printf("found hook for %d\r\n", i);

			completed.push_back(i);
			*(hmanager->hooks->at(i)->retAddress) = (stdHook)(hookLocation + 6);

			VirtualProtect((void*)hookLocation, length, PAGE_EXECUTE_READWRITE, &old);
			memset((void*)hookLocation, 0x90, length);
			*(BYTE*)hookLocation = 0xE9;
			*(DWORD*)(hookLocation + 1) = ((DWORD)hmanager->Get(i)->functionPointer - hookLocation - 5);
			VirtualProtect((void*)hookLocation, length, old, &old);
			delete hmanager->Get(i);

		}
		if (completed.size() == hmanager->hooks->size())
			break;
		Sleep(200);
	}
	printf("Hooked everything! have fun\r\n");
	ExitThread(0);
}

void HookManager::HookAll(LPVOID param) {
	hmanager = this;
	printf("hooks size: %d\r\n", hooks->size());
	for (int i = 0; i < hooks->size(); i++) {
		Get(i)->Hook();
	}
	CreateThread(0, 0, HookThread, param, 0, 0);
};


HookMember* HookManager::Get(char id) {
	return hooks->at(id);
}

void HookMember::Hook() {
	DWORD old = 0x0;
	char bytesToReplace[]	=	{ (char)handle, (char)0x13, (char)0x9B, (char)0x8D };
	VirtualProtect(baseAddress, 4, PAGE_EXECUTE_READWRITE, &old);
	memcpy(baseAddress, bytesToReplace, 4);
	VirtualProtect(baseAddress, 4, old, &old);
}

HookMember::HookMember(void* baseAddress, void* functionPointer, stdHook* ret, char handle) :
	baseAddress(baseAddress),
	handle(handle),
	functionPointer(functionPointer),
	retAddress(ret),
	hookString()
{
	char a[] = { (char)0x8B, (char)0x0D, (char)0xC0, (char)0x9B, (char)0x8D, (char)0x0, (char)0x81, (char)0xC1, (char)handle, (char)0x13, (char)0x0, (char)0x0, (char)0x89, (char)0xC8 };
	for (int i = 0; i < 14; i++)
		hookString[i] = a[i];
};
