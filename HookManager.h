#pragma once
#include "pch.h"
#include <map>

using std::map;
typedef void(__cdecl* stdHook)();

struct HookMember {
public:
	void* baseAddress;
	void* functionPointer;
	BYTE* oldBytes;
	stdHook* retAddress;

	HookMember(void*, void*, stdHook*);
	void Hook();
};

class HookManager {
private:
public:
	map<int, HookMember> hooks;
	int count;
	int AddHook(HookMember);
	HookMember* Get(int);
	void HookAll(HMODULE);
	HookManager();
};