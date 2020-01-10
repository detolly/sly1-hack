// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "sigscan.h"
#include "HookManager.h"
#include "vector3.h"
#include "rotation.h"
#include "memorydump.h"
#include "Menu.h"
#include "registers.h"
#include "Strings.h"

bool showCustomMenu = false;
Menu* m = (Menu*)0x2026FF68;

MenuManager* menuManager;
Strings* gameStrings = (Strings*)0x2024B660;
Strings originalStrings;
Strings myStrings;

Vector3* slyPosition;
Rotation* slyRotation;
LPVOID Param;
void exit_app();


HookManager hookManager;
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

stdHook oPressedInMenu;
void selectInMenu() {
	*(UINT32*)_v0 -= 0x62B0;
	oPressedInMenu();
}

stdHook oRenderMenu;
int cooldown = 0;
void renderMenuHook() {
	*(UINT32*)_v0 = *(UINT32*)(_s2+0x250);
	if (showCustomMenu) {
		if (m->highlightedIndex > 4) {
			menuManager->setIndex(true);
			m->highlightedIndex = 4;
		} else if (m->highlightedIndex < 0) {
			menuManager->setIndex(false);
			m->highlightedIndex = 0;
		}
	}
	oRenderMenu();
}

bool godmode = false;

DWORD WINAPI MainThread(LPVOID param) {
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "w", stdin);
	
	Param = param;
	memcpy(&originalStrings, gameStrings, sizeof(Strings));

	//  addresses are not hard coded so to speak, they're just references to the actual MIPS game code,
	//  and not the translated x86 msvc that you see in cheat engine f. ex.
	/*
	int coinHookHandle = hookManager.AddHook(HookMember((void*)0x201481C4, &hookPickUpCoin));
	oPickUpCoin = (stdHook)hookManager.Get(coinHookHandle)->Hook();
	
	int charmHookHandle = hookManager.AddHook(HookMember((void*)0x20192C8C, &takeDamageHook));
	oTakeDamage = (stdHook)hookManager.Get(charmHookHandle)->Hook();
	*/
	menuManager = new MenuManager(&myStrings, gameStrings);

	menuManager->AddMenuEntry((char*)"Godmode: Off", [](char* a) {
		godmode = !godmode;
		n(a, "Godmode: " + godmode ? "On." : "Off.", 16);
	});
	menuManager->AddMenuEntry((char*)"from",			[](char* a) { printf("greetings\r\n");});
	menuManager->AddMenuEntry((char*)"beyond",			[](char* a) { printf("greetings\r\n");});
	menuManager->AddMenuEntry((char*)"the",				[](char* a) { printf("greetings\r\n");});
	menuManager->AddMenuEntry((char*)"grave",			[](char* a) { printf("greetings\r\n");});
	menuManager->AddMenuEntry((char*)"you",				[](char* a) { printf("greetings\r\n");});
	menuManager->AddMenuEntry((char*)"maggot",			[](char* a) { printf("greetings\r\n");});
	menuManager->AddMenuEntry((char*)"loving",			[](char* a) { printf("greetings\r\n");});
	menuManager->AddMenuEntry((char*)"tspark2Weird",	[](char* a) { printf("greetings\r\n");});

	int renderMenuHandle = hookManager.AddHook(HookMember((void*)0x20194FDC, &renderMenuHook));
	oRenderMenu = (stdHook)hookManager.Get(renderMenuHandle)->Hook();

	/*
	int pressedMenuHandle = hookManager.AddHook(HookMember((void*)0x20195964, &selectInMenu));
	oPressedInMenu = (stdHook)hookManager.Get(pressedMenuHandle)->Hook();
	*/

	bool registeredDOWN = false;
	bool registeredUP = false;
	bool registeredPGDN = false;
	bool registeredENTER = false;

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
				registeredDOWN = true;
				if (showCustomMenu)
					m->highlightedIndex++;
			}
		} else registeredDOWN = false;
		if (GetAsyncKeyState(VK_UP)) {
			if (!registeredUP) {
				registeredUP = true;
				if (showCustomMenu)
					m->highlightedIndex--;
			}
		} else registeredUP = false;
		if (GetAsyncKeyState(VK_NEXT)) {
			if (!registeredPGDN) {
				//MemoryDump::Dump("C:\\test\\dump.bin", 0x30000000, positionHookLocation-0x30000000);
				registeredPGDN = true;
				if (!m->isMenuOpen || showCustomMenu)
				{
					menuManager->Update();
					showCustomMenu = !showCustomMenu;
					*gameStrings = showCustomMenu ? myStrings : originalStrings;
					m->isMenuOpen = showCustomMenu ? 1 : 0;
					m->menuStatus = showCustomMenu ? 1 : 3;
				}
			}
		} else registeredPGDN = false;
		if (GetAsyncKeyState(VK_RETURN)) {
			if (!registeredENTER) {
				registeredENTER = true;
				if (showCustomMenu)
					menuManager->execute(m->highlightedIndex);
			}
		} else registeredENTER = false;
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


