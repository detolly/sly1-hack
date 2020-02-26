#pragma once
#include "pch.h"
#include <map>

using std::map;
typedef void(__cdecl* stdHook)();

struct HookMember {
public:
	void* baseAddress;
	char handle;
	void* functionPointer;
	//char* bytesToReplace; // 0x37 0x13, 0x9B, 0x8D
	char hookString[14];	// \x8B\x0D\xC0\x9B\x8D\x00\x81\xC1\x37\x13\x00\x00\x89\xC8
	stdHook* retAddress;

	HookMember(void*, void*, stdHook*, char);
	void Hook();
};

class HookManager {
private:
public:
	map<char, HookMember*> hooks;
	char count;
	int AddHook(void*, void*, stdHook*);
	HookMember* Get(char);
	void HookAll(LPVOID asd);
	HookManager();
};