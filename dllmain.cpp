// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "sigscan.h"
#include "HookManager.h"
#include "vector3.h"
#include "rotation.h"
#include "memorydump.h"
#include "Menu.h"
#include "registers.h"

Vector3* slyPosition;
Rotation* slyRotation;
HookManager hookManager;
LPVOID Param;
void exit_app();

//experimental
typedef void(__stdcall* RenderMenu)(Menu* menuBase);
RenderMenu renderMenu;

typedef void(__cdecl* stdHook)();

stdHook oPickUpCoin;
void hookPickUpCoin() {
	oPickUpCoin();
}

stdHook oTakeDamage;
void takeDamageHook() {
	*(UINT32*)_v0 = 0x2;
	
	oTakeDamage();
}

stdHook oRenderMenu;
Menu* m = copyMenu(0x2026FF68);

void renderMenuHook() {
	*(UINT32*)_v0 = *(DWORD*)(_s2-0x250);
	printf("0x%x\r\n", *(DWORD*)_v0);
	Menu* originalMenu = (Menu*)0x2026FF68;
	originalMenu->menuScale *= 0.99f;
	oRenderMenu();
}

DWORD WINAPI MainThread(LPVOID param) {
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "w", stdin);
	
	Param = param;

	int coinHookHandle = hookManager.AddHook(HookMember((void*)0x201481C4, &hookPickUpCoin));
	oPickUpCoin = (stdHook)hookManager.Get(coinHookHandle)->Hook();

	int charmHookHandle = hookManager.AddHook(HookMember((void*)0x20192C8C, &takeDamageHook));
	oTakeDamage = (stdHook)hookManager.Get(charmHookHandle)->Hook();

	int renderMenuHandle = hookManager.AddHook(HookMember((void*)0x20194FDC, &renderMenuHook));
	oRenderMenu = (stdHook)hookManager.Get(renderMenuHandle)->Hook();

	bool pressed = false;
	bool registeredDOWN = false;
	bool registeredUP = false;
	bool registeredPGDN = false;

	unsigned long long frames = 0;
	while (true) {
		frames++;
		//do hacking 
		if (frames % 500 == 0) {
			//printf("Position (0x%x): %.2f\t%.2f\t%.2f\r\n", (DWORD)slyPosition, slyPosition->x, slyPosition->y, slyPosition->z);
		}
		if (GetAsyncKeyState(VK_ESCAPE)) break;
		if (GetAsyncKeyState(VK_DOWN)) {
			if (!registeredDOWN) {
				//renderMenu(m);
				registeredDOWN = true;
			}
		} else registeredDOWN = false;
		if (GetAsyncKeyState(VK_UP)) {
			if (!registeredUP) {
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


