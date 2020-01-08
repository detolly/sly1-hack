#pragma once
#include "pch.h"
#include <map>

using std::map;

struct HookMember {
private:
	bool is_hooked = false;
public:
	void* baseAddress;
	void* functionPointer;
	int len;
	BYTE* oldBytes;
	DWORD retAddress;

	HookMember(void*, void*, int);
	~HookMember();

	void* Hook();
	bool Unhook();
};

class HookManager {
private:
	map<int, HookMember> hooks;
	int count;
public:
	int AddHook(HookMember);
	HookMember* Get(int);
	void UnhookAll();
	HookManager();
};