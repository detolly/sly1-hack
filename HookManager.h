#pragma once
#include "pch.h"
#include <map>

using std::map;

struct HookMember {
private:
	bool is_hooked = false;
public:
	DWORD baseAddress;
	void* functionPointer;
	int len;
	BYTE* oldBytes;
	DWORD retAddress;

	HookMember(DWORD, void*, int, DWORD* retjump);
	~HookMember();

	bool Hook();
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