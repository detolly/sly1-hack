#pragma once
#include "pch.h"
#include <map>

using std::map;

struct HookMember {
private:
							// for cheat engine reference: 8b 0d c0 9b 8d 00 81 c1 37
	const char* hookString = "\x8B\x0D\xC0\x9B\x8D\x00\x81\xC1\x37\x13\x00\x00\x89\xC8";
	int length = 51;
	const char* hookMask = "xxxxxxxxxxxxxx";
	const char* bytesToReplace = "\x37\x13\x9B\x8D"; /* 9b was 8f for t7 */
	
	bool is_hooked = false;
public:
	void* baseAddress;
	void* functionPointer;
	BYTE* oldBytes;
	DWORD retAddress;

	HookMember(void*, void*);
	~HookMember();

	//void* Hook();
	DWORD Hook();
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