#include "pch.h"
#include "HookManager.h"

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


bool HookMember::Hook() {
	if (this->is_hooked) {
		return false;
	}

	if (len < 5) {
		return false;
	}

	DWORD old;
	VirtualProtect((LPVOID)this->baseAddress, len, PAGE_EXECUTE_READWRITE, &old);

	this->oldBytes = new BYTE[len];
	memcpy(this->oldBytes, (void*)this->baseAddress, len);
	printf("Hooking %x with %d length. (fp: %x)\r\n", (DWORD)this->baseAddress, len, (DWORD)this->functionPointer);
	memset((void*)this->baseAddress, 0x90, len);

	DWORD relativeAddress = (DWORD)this->functionPointer-this->baseAddress-5;
	*(BYTE*)(this->baseAddress) = 0xE9;
	*(DWORD*)(this->baseAddress + 1) = relativeAddress;

	VirtualProtect((void*)this->baseAddress, len, old, NULL);

	this->is_hooked = true;

	return true;
}

bool HookMember::Unhook() {
	
	if (!is_hooked) {
		return false;
	}

	DWORD old;

	VirtualProtect((void*)this->baseAddress, len, PAGE_EXECUTE_READWRITE, &old);
	memcpy((void*)this->baseAddress, this->oldBytes, len);
	VirtualProtect((void*)this->baseAddress, len, old, NULL);
	
	this->is_hooked = false;

	return true;
}

HookMember::~HookMember() {
	if (is_hooked) {
		Unhook();
	}
}

HookMember::HookMember(DWORD baseAddress, void* functionPointer, int len, DWORD* retjump) {
	this->baseAddress = baseAddress;
	this->functionPointer = functionPointer;
	this->len = len;
	this->retAddress = (DWORD)baseAddress + len;
	*retjump = this->retAddress;
}