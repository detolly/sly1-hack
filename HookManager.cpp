#include "pch.h"
#include "HookManager.h"
#include "sigscan.h"

HookManager::HookManager() { count = 0; }

int HookManager::AddHook(HookMember h) {
	int id = this->count++;
	this->hooks.insert(std::make_pair(id, h));
	return id;
}

HookMember* HookManager::Get(int id) {
	return &this->hooks.at(id);
}

void HookManager::UnhookAll() {
	for (size_t i = 0; i < this->hooks.size(); i++) {
		this->hooks.at(i).Unhook();
	}
}

/* deprecated
void* HookMember::Hook() {
	if (this->is_hooked) {
		return (void*)retAddress;
	}

	if (len < 5) {
		return nullptr;
	}

	DWORD old;
	VirtualProtect(this->baseAddress, len, PAGE_EXECUTE_READWRITE, &old);

	this->oldBytes = new BYTE[len];
	memcpy(this->oldBytes, (void*)this->baseAddress, len);
	printf("Hooking %x with %d length. (fp: %x)\r\n", (DWORD)this->baseAddress, len, (DWORD)this->functionPointer);
	memset((void*)this->baseAddress, 0x90, len);

	DWORD relativeAddress = (DWORD)this->functionPointer-(DWORD)this->baseAddress-5;
	*(BYTE*)(this->baseAddress) = 0xE9;
	*(DWORD*)((DWORD)this->baseAddress + 1) = relativeAddress;

	VirtualProtect(this->baseAddress, len, old, NULL);

	this->is_hooked = true;

	return (void*)retAddress;
}
*/

DWORD HookMember::Hook() {
	if (this->is_hooked) {
		return this->retAddress;
	}

	oldBytes = new BYTE[strlen(bytesToReplace)];

	DWORD start = 0x30000000; //recompiler address space
	DWORD size = 0x1000000;

	DWORD old = 0x0;
	VirtualProtect(baseAddress, strlen(bytesToReplace), PAGE_EXECUTE_READWRITE, &old);
	memcpy(oldBytes, baseAddress, strlen(bytesToReplace));
	memcpy(baseAddress, bytesToReplace, strlen(bytesToReplace));
	VirtualProtect(baseAddress, strlen(bytesToReplace), old, nullptr);
	
	DWORD hookLocation = 0x0;
	while (true) {
		printf("sigscanning for hook...\r\n");
		if (!SignatureScanner::FindSignature(&hookLocation, start, size, hookString, hookMask, 0)) {
			Sleep(100);
		}
		else break;
	}
	printf("Hook location found at: 0x%x\r\n", hookLocation);
	
	VirtualProtect((void*)hookLocation, length, PAGE_EXECUTE_READWRITE, &old);

	memset((void*)hookLocation, 0x90, length);
	*(BYTE*)hookLocation = 0xE9;
	*(DWORD*)(hookLocation + 1) = ((DWORD)functionPointer - hookLocation - 5);


	VirtualProtect((void*)hookLocation, length, old, nullptr);

	printf("return ptr: 0x%x\r\n", hookLocation + 6);
	retAddress = hookLocation+6;
	

	return retAddress;
}

bool HookMember::Unhook() {
	if (!is_hooked) {
		return false;
	}

	DWORD old;
	VirtualProtect(baseAddress, strlen(bytesToReplace), PAGE_EXECUTE_READWRITE, &old);
	memcpy(baseAddress, oldBytes, strlen(bytesToReplace));
	VirtualProtect(baseAddress, strlen(bytesToReplace), old, nullptr);
	
	this->is_hooked = false;

	return true;
}

HookMember::~HookMember() {
	if (is_hooked)
		Unhook();
}

HookMember::HookMember(void* baseAddress, void* functionPointer) {
	this->baseAddress = baseAddress;
	this->functionPointer = functionPointer;
}