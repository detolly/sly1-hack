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
	stdHook* retAddress;
	char hookString[14];

	HookMember(void*, void*, stdHook*, int);
	void Hook();
};

class HookManager {
private:
public:
	map<char, HookMember> hooks;
	int count;
	int AddHook(void*, void*, stdHook*);
	HookMember* Get(char);
	void HookAll(HMODULE);
	HookManager();
};