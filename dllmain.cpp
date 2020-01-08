// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "sigscan.h"
#include "HookManager.h"
#include "vector3.h"
#include "rotation.h"
#include "memorydump.h"
#include "Menu.h"

Vector3* slyPosition;
Rotation* slyRotation;
HookManager hookManager;
LPVOID Param;
void exit_app();

typedef void(__fastcall* AddCoinFn)(DWORD ecx, int coins);
AddCoinFn oAddCoin;
void __fastcall hookedAddCoin(DWORD ecx, int coins) {
	coins = 69;	
	oAddCoin(ecx, coins);
}

typedef void(__fastcall* TakeDamageFn)(DWORD ecx, int charm);
TakeDamageFn oTakeDamage;
void __fastcall hookedTakeDamage(DWORD ecx, int charm) {
	charm = 2;
	oTakeDamage(ecx, charm);
}

typedef void(__cdecl* AccessSlyPositionFn)();
AccessSlyPositionFn oAccessSlyPosition;
void hookedAccessSlyPosition() {
	__asm {
		sub ecx, 0x8
		mov[slyPosition], ecx
		sub ecx, 0x30
		mov[slyRotation], ecx
		movd ecx, xmm2
		movd edx, xmm3
	}
	oAccessSlyPosition();
}

DWORD retjump4;
float gravity = 1;
void __declspec(naked) d() {
	__asm {
		movaps xmm5, [edx]
		movaps xmm1, xmm5
		mulss xmm1, [gravity]
		movaps xmm5, xmm1
		movaps [ecx], xmm5
		jmp [retjump4]
	}
}

//experimental
typedef void(__stdcall* RenderMenu)(Menu* menuBase);
RenderMenu renderMenu;

DWORD start = 0x30000000; //recompiler address space
DWORD size = 0x10000000;

DWORD WINAPI MainThread(LPVOID param) {
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	
	Param = param;

	const char* coinSignature = "\x75\x0D\x83\x3D\x00\x00\x00\x00\x00\x0F\x84\x00\x00\x00\x00\x8B\x15\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x81\xC1\x00\x00\x00\x00\x89\xC8\xC1\xE8\x0C\x8B\x04\x85\x00\x00\x00\x00\xBB\x00\x00\x00\x00\x01\xC1\x0F\x88\x00\x00\x00\x00\x89\x11\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x83\xC0\x13\xA3\x00\x00\x00\x00\x2B\x05\x00\x00\x00\x00\x0F\x88\x00\x00\x00\x00\xE9\x00\x00\x00\x00\x8B\x15\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x81\xC1\x00\x00\x00\x00\x89\xC8\xC1\xE8\x0C\x8B\x04\x85\x00\x00\x00\x00";
	const char* coinMask = "xxxx?????xx????xx????xx????xx????xxxxxxxx????x????xxxx????xxxx????????x????xxxx????xx????xx????x????xx????xx????xx????xxxxxxxx????";
	const char* charmSignature = "\x75\x14\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x8B\x15\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x81\xC1\x00\x00\x00\x00\x89\xC8\xC1\xE8\x0C\x8B\x04\x85\x00\x00\x00\x00\xBB\x00\x00\x00\x00\x01\xC1\x0F\x88\x00\x00\x00\x00\x89\x11\x8B\x0D\x00\x00\x00\x00\x81\xC1\x00\x00\x00\x00\x89\xC8\xC1\xE8\x0C\x8B\x04\x85\x00\x00\x00\x00";
	const char* charmMask = "xxxx????????xx????????xx????xx????xx????xxxxxxxx????x????xxxx????xxxx????xx????xxxxxxxx????";
	const char* positionSignature = "\xC1\xE8\x0C\x8B\x04\x85\x00\x00\x00\x00\xBB\x00\x00\x00\x00\x01\xC1\x0F\x88\x00\x00\x00\x00\x89\x11\xA1\x00\x00\x00\x00\xA3\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x81\xC1\x00\x00\x00\x00\x89\xC8\xC1\xE8\x0C\x8B\x04\x85\x00\x00\x00\x00\xBB\x00\x00\x00\x00\x01\xC1\x0F\x88\x00\x00\x00\x00\x8B\x01\xA3\x00\x00\x00\x00\xA1\x00\x00\x00\x00\xA3\x00\x00\x00\x00\xF3\x0F\x10\x15\x00\x00\x00\x00\xF3\x0F\x10\x1D\x00\x00\x00\x00\x66\x0F\x7E\xD1";
	const char* positionMask = "xxxxxx????x????xxxx????xxx????x????xx????xx????xxxxxxxx????x????xxxx????xxx????x????x????xxxx????xxxx????xxxx";

	//for this hook you have to pick up a coin before the code spawns for some reason..
	DWORD coinhookLocation = 0x0;
	if (!SignatureScanner::FindSignature(&coinhookLocation, start, size, coinSignature, coinMask, strlen(coinMask), 15)) {
		printf("Failed to find pattern signature for coin hook.");
	}
	else {
		printf("Coin hook found: 0x%x\r\n", coinhookLocation);
		int coinHookHandle = hookManager.AddHook(HookMember((void*)coinhookLocation, hookedAddCoin, 6));
		oAddCoin = (AddCoinFn)hookManager.Get(coinHookHandle)->Hook();
	}

	//you have to take one damage while having a charm.
	DWORD charmHookLocation = 0x0;
	if (!SignatureScanner::FindSignature(&charmHookLocation, start, size, charmSignature, charmMask, strlen(charmMask), 22)) {
		printf("Failed to find pattern signature for charm hook.");
		exit_app();
	}
	printf("Charm hook found: 0x%x\r\n", charmHookLocation);
	int charmHookHandle = hookManager.AddHook(HookMember((void*)charmHookLocation, hookedTakeDamage, 6));
	oTakeDamage = (TakeDamageFn)hookManager.Get(charmHookHandle)->Hook();
	

	//this one works all of the time, as long as you have been able to move your character at least once since launch of the game.
	DWORD positionHookLocation = 0x0;
	if (!SignatureScanner::FindSignature(&positionHookLocation, start, size, positionSignature, positionMask, strlen(charmMask), 105)) {
		printf("Failed to find pattern signature for charm hook.");
		exit_app();
	}
	printf("Position hook found: 0x%x\r\n", positionHookLocation);
	int positionHookHandle = hookManager.AddHook(HookMember((void*)positionHookLocation, hookedAccessSlyPosition, 8));
	oAccessSlyPosition = (AccessSlyPositionFn)hookManager.Get(positionHookHandle)->Hook();

	/*experimental
	Menu* m = copyMenu(0x2026FF68);
	renderMenu = (RenderMenu)0x305D3A3E;
	*/

	bool pressed = false;
	bool registeredDOWN = false;
	bool registeredUP = false;
	bool registeredPGDN = false;

	unsigned long frames = 0;
	while (true) {
		frames++;
		//do hacking 
		if (frames % 500 == 0) {
			printf("Position (0x%x): %.2f\t%.2f\t%.2f\r\n", (DWORD)slyPosition, slyPosition->x, slyPosition->y, slyPosition->z);
		}
		if (GetAsyncKeyState(VK_ESCAPE)) break;
		if (GetAsyncKeyState(VK_DOWN)) {
			if (!registeredDOWN) {
				gravity *= 0.5;
				//renderMenu(m);
				printf("%.2f\r\n", gravity);
				registeredDOWN = true;
			}
		} else registeredDOWN = false;
		if (GetAsyncKeyState(VK_UP)) {
			if (!registeredUP) {
				gravity *= 2;
				printf("%.2f\r\n", gravity);
				registeredUP = true;
			}
		} else registeredUP = false;
		if (GetAsyncKeyState(VK_NEXT)) {
			if (!registeredPGDN) {
				//MemoryDump::Dump("C:\\test\\dump.bin", 0x30000000, positionHookLocation-0x30000000);
				registeredPGDN = true;
			}
		}
		else registeredPGDN = false;
		Sleep(1);
	}

	exit_app();
}

void exit_app() {
	printf("Unhooking, don't close\r\n");
	hookManager.UnhookAll();
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


