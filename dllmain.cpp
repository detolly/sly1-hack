﻿// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "sigscan.h"
#include "HookManager.h"

struct Vector3 {
	float x, z, y;
};

struct Entity {
	float x, z, y;
	char buffer[52];
};

HookManager hookManager;

DWORD retjump;
void __declspec(naked) a() {
	__asm {
		mov edx, 69
		jmp [retjump]
	}
}

DWORD retjump2;
void __declspec(naked) b() {
	__asm {
		mov edx, 2
		jmp[retjump2]
	}
}

/*
DWORD retjump3;
void __declspec(naked) c() {
	__asm {
		movaps xmm1, [edx]
		movaps[ecx], xmm1
		mov[Ypointer], ecx
		jmp [retjump3]
	}
}
*/

DWORD retjump4;
Vector3* slyPosition;
void __declspec(naked) d() {
	__asm {
		sub ecx, 0x8
		mov [slyPosition], ecx
		movd ecx, xmm2
		movd edx, xmm3
		jmp [retjump4]
	}
}


LPVOID Param;
void exit_app();

DWORD WINAPI MainThread(LPVOID param) {
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	
	Param = param;
	//Ypointer = 0x0;

	int* coins = (int*)0x2027DC08;
	int* charm = (int*)0x2027DC04;
	//Entity* entitylist = (Entity*)0x008EB6B8;
	Entity* entitylist = (Entity*)0x21a7e100;
	
	// scanning for the address of the variable:
	printf("Coins: %d\r\nCharm: %d\r\n", *coins, *charm);

	const char* coinSignature = "\x75\x0D\x83\x3D\x00\x00\x00\x00\x00\x0F\x84\x00\x00\x00\x00\x8B\x15\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x81\xC1\x00\x00\x00\x00\x89\xC8\xC1\xE8\x0C\x8B\x04\x85\x00\x00\x00\x00\xBB\x00\x00\x00\x00\x01\xC1\x0F\x88\x00\x00\x00\x00\x89\x11\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x83\xC0\x13\xA3\x00\x00\x00\x00\x2B\x05\x00\x00\x00\x00\x0F\x88\x00\x00\x00\x00\xE9\x00\x00\x00\x00\x8B\x15\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x81\xC1\x00\x00\x00\x00\x89\xC8\xC1\xE8\x0C\x8B\x04\x85\x00\x00\x00\x00";
	const char* coinMask = "xxxx?????xx????xx????xx????xx????xxxxxxxx????x????xxxx????xxxx????????x????xxxx????xx????xx????x????xx????xx????xx????xxxxxxxx????";
	const char* charmSignature = "\x75\x14\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x8B\x15\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x81\xC1\x00\x00\x00\x00\x89\xC8\xC1\xE8\x0C\x8B\x04\x85\x00\x00\x00\x00\xBB\x00\x00\x00\x00\x01\xC1\x0F\x88\x00\x00\x00\x00\x89\x11\x8B\x0D\x00\x00\x00\x00\x81\xC1\x00\x00\x00\x00\x89\xC8\xC1\xE8\x0C\x8B\x04\x85\x00\x00\x00\x00";
	const char* charmMask = "xxxx????????xx????????xx????xx????xx????xxxxxxxx????x????xxxx????xxxx????xx????xxxxxxxx????";

	/*

	int len = 6;
	DWORD coinhookLocation = 0x0;
	if (!SignatureScanner::FindSignature(&coinhookLocation, 0x30000000, 0x10000000, coinSignature, coinMask, strlen(coinMask), 15)) {
		printf("Failed to find pattern signature for coin hook.");
		exit_app();
	}
	printf("Coin hook found: 0x%x\r\n", (DWORD)coinhookLocation);
	int coinHookHandle = hookManager.AddHook(HookMember(coinhookLocation, a, len, &retjump));
	hookManager.Get(coinHookHandle)->Hook();

	len = 6;
	DWORD charmHookLocation = 0x0;
	if (!SignatureScanner::FindSignature(&charmHookLocation, 0x30000000, 0x10000000, charmSignature, charmMask, strlen(charmMask), 22)) {
		printf("Failed to find pattern signature for charm hook.");
		exit_app();
	}
	printf("Charm hook found: 0x%x\r\n", (DWORD)charmHookLocation);
	int charmHookHandle = hookManager.AddHook(HookMember(charmHookLocation, b, len, &retjump2));
	hookManager.Get(charmHookHandle)->Hook();
	
	*/

	int len = 8;
	DWORD positionHookLocation = 0x3011F238;
	int positionHookHandle = hookManager.AddHook(HookMember(positionHookLocation, d, len, &retjump4));
	hookManager.Get(positionHookHandle)->Hook();


	bool pressed = false;
	unsigned long frames = 0;
	while (true) {
		frames++;
		//do hacking 
		if (frames % 20 == 0) {
			printf("%.2f\t%.2f\t%.2f\r\n", slyPosition->x, slyPosition->y, slyPosition->z);
		}
		if (GetAsyncKeyState(VK_ESCAPE)) break;
		if (GetAsyncKeyState(VK_END))
			if (!pressed) {
				printf("enabled hook on write y vel\r\n");
				pressed = true;
			}
			else {
				printf("disabled hook on write y vel\r\n");
				pressed = false;
			}
		Sleep(50);
	}

	exit_app();
}

void exit_app() {
	printf("Unhooking, don't close\r\n");
	hookManager.UnhookAll();
	Sleep(2500);
	printf("Now you can close\r\n");
	FreeConsole();
	FreeLibraryAndExitThread((HMODULE)Param, 0);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
		case DLL_PROCESS_ATTACH:
			CreateThread(0, 0, MainThread, hModule, 0, 0);
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


