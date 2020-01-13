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
#include "rgb.h"
#include "hooks.h"
#include "MenuManager.h"


// this function has to do with animations: 00124fc0

// 001749A4: jumped to if ledgegrabbing
// 0018FCF0: stores the opacity of stuff - this one will be fun
#pragma region declarations
bool showCustomMenu = false;
Menu* m = (Menu*)0x2026FF68;

MenuManager* menuManager;
Strings* gameStrings;
Strings originalStrings;
Strings myStrings;

LPVOID Param;

Vector3* slyPosition;
Rotation* slyRotation;
void exit_app();
#pragma endregion declarations

#pragma region hooks

HookManager hookManager;
stdHook oPickUpCoin;
void hookPickUpCoin() {
	r->v0.UW[0] = 69;
	oPickUpCoin();
}

stdHook oCharmDamage;
void charmDamageHook() {
	r->v0.UW[0] = 2;
	oCharmDamage();
}

bool godmode = false;
stdHook oSlyHit;
void hookedSlyHit() {
	if (godmode) {
		DWORD temp = r->s1.UD[0];
		while (*(DWORD*)(temp + 0x20000000) != 0)
			temp += 4;
		printf("found a temp: 0x%x, original s1: 0x%x\r\n", temp, r->s1.UW[0]);
		r->a0.UD[0] = temp;
	}
	else  r->a0.UD[0] = r->s1.UD[0];
	oSlyHit();
}

bool unlimitedFish = false;
stdHook oFishTimer;
void fishHook() {
	if (!unlimitedFish) {
		*_f01 = *_f01 + *_f00;
	}
	oFishTimer();
}

stdHook oSelectInMenu;
void selectInMenu() {
	r->v0.UW[0] -= (u32)0x62B0;
	oSelectInMenu();
}

hsv h;
rgba oldrgb;
DWORD rgbaddress;

stdHook oRenderMenu;
void renderMenuHook() {
	r->v0.UW[0] = *(u32*)((DWORD)(r->s2.UW[0] + 0x250) + 0x20000000);
	if (showCustomMenu) {
		h.h++; if (h.h > 360) h.h = 0;
		hsvrgb(&h, (rgba*)rgbaddress);
		((rgba*)rgbaddress)->a = 0x80;
		if (m->highlightedIndex > 4) {
			menuManager->setIndex(true);
			m->highlightedIndex = 4;
		}
		else if (m->highlightedIndex < 0) {
			menuManager->setIndex(false);
			m->highlightedIndex = 0;
		}
		m->x = 25;
		m->y = 50;
		m->menuScale = 0.7f;
	}
	oRenderMenu();
}

stdHook oChangeOpacity;
void hookedChangeOpacity() {
	//idk what this is or was
	oChangeOpacity();
}
#pragma endregion hooks


DWORD WINAPI MainThread(LPVOID param) {
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "w", stdin);

	// 205 205 205 128 205 205 205 17
	if (SignatureScanner::FindSignature(&rgbaddress, 0x20600000, 0x100000, "\xCD\xCD\xCD\x80\xCD\xCD\xCD\x11", "xxxxxxxx", 0))
		printf("found rgb\r\n");
	else
		printf("didn't find rgb\r\n"); 

	h.h = 0;
	h.s = 1.f;
	h.v = 1.f;
	oldrgb = *(rgba*)rgbaddress;

	DWORD pGameStrings = 0x0;
	SignatureScanner::FindSignature(&pGameStrings , 0x20000000, 0x10000000, "Paused", "xxxxxx", 0);
	gameStrings = (Strings*)pGameStrings;

	Param = param;
	memcpy(&originalStrings, gameStrings, sizeof(Strings));
	menuManager = new MenuManager(&myStrings, gameStrings);

	MenuEntry* godmodee = new DelegateEntry((char*)"Godmode: Off", [](char* a) {
		godmode = !godmode;
		char c[16] = "Godmode: ";
		strcat(c, godmode ? "On" : "Off");
		n(a, c, 16);
	});

	MenuEntry* fish = new DelegateEntry((char*)"Fish timer: On", [](char* a) {
		unlimitedFish = !unlimitedFish;
		char c[16] = "Fish timer: ";
		strcat(c, unlimitedFish ? "Off" : "On");
		n(a, c, 16);
	});

	menuManager->AddMenuEntry(godmodee);
	menuManager->AddMenuEntry(fish);

	SubMenu* s = new SubMenu("Sonic", menuManager);
	MenuEntry* x = new DelegateEntry("Sonic",		[](char* a) { printf("asd\r\n"); });
	MenuEntry* x1 = new DelegateEntry("likes",		[](char* a) { printf("asd\r\n"); });
	MenuEntry* x2 = new DelegateEntry("grass",		[](char* a) { printf("asd\r\n"); });
	MenuEntry* x3 = new DelegateEntry("rhymes",		[](char* a) { printf("asd\r\n"); });
	MenuEntry* x4 = new DelegateEntry("with ass",	[](char* a) { printf("asd\r\n"); });
	s->AddMenuEntry(x);
	s->AddMenuEntry(x1);
	s->AddMenuEntry(x2);
	s->AddMenuEntry(x3);
	s->AddMenuEntry(x4);
	menuManager->AddMenuEntry(s);

	SubMenu* s2 = new SubMenu("tSparkles", menuManager);
	MenuEntry* y1 = new DelegateEntry("likely", [](char* a) { printf("asd2\r\n"); });
	MenuEntry* y2 = new DelegateEntry("the",	[](char* a) { printf("asd2\r\n"); });
	MenuEntry* y3 = new DelegateEntry("best",	[](char* a) { printf("asd2\r\n"); });
	MenuEntry* y4 = new DelegateEntry("sly",	[](char* a) { printf("asd2\r\n"); });
	MenuEntry* y5 = new DelegateEntry("player", [](char* a) { printf("asd2\r\n"); });
	MenuEntry* y6 = new DelegateEntry("ever",	[](char* a) { printf("asd2\r\n"); });
	s2->AddMenuEntry(y1);
	s2->AddMenuEntry(y2);
	s2->AddMenuEntry(y3);
	s2->AddMenuEntry(y4);
	s2->AddMenuEntry(y5);
	s2->AddMenuEntry(y6);
	menuManager->AddMenuEntry(s2);

	MenuEntry* best = new DelegateEntry("What comes next?", [](char* a) { printf("asd\r\n"); });
	menuManager->AddMenuEntry(best);

	//  addresses are not hard coded so to speak, they're just references to the actual MIPS game code,
	//  and not the translated x86 msvc that you see in cheat engine f. ex.
	
	/*
	int fishHandle = hookManager.AddHook(HookMember((void*)0x201ABB58, &fishHook));
	oFishTimer = (stdHook)hookManager.Get(fishHandle)->Hook();
	

	int opacityHookHandle = hookManager.AddHook(HookMember((void*)0x2018FCF0, &hookedChangeOpacity));
	oChangeOpacity = (stdHook)hookManager.Get(opacityHookHandle)->Hook();
	*/

	int pressedMenuHandle	= hookManager.AddHook((void*)0x20195964, &selectInMenu,		&oSelectInMenu);
	int charmHookHandle		= hookManager.AddHook((void*)0x20192C8C, &charmDamageHook,	&oCharmDamage);
	int coinHookHandle		= hookManager.AddHook((void*)0x201481C4, &hookPickUpCoin,	&oPickUpCoin);
	int renderMenuHandle	= hookManager.AddHook((void*)0x20194FDC, &renderMenuHook,	&oRenderMenu);
	int slyHitHandle		= hookManager.AddHook((void*)0x2013BF30, &hookedSlyHit,		&oSlyHit);
	hookManager.HookAll(param);

	bool registeredDOWN		= false;
	bool registeredUP		= false;
	bool registeredPGDN		= false;
	bool registeredENTER	= false;
	bool registeredLEFT		= false;

	while (true) {
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
					*(rgba*)rgbaddress = showCustomMenu ? *(rgba*)rgbaddress : oldrgb;
					m->menuStatus = showCustomMenu ? 1 : 3;
					m->isMenuOpen = showCustomMenu ? 1 : 0;
					m->menuFade = showCustomMenu ? 0.f : 2.f;
					*gameStrings = showCustomMenu ? myStrings : originalStrings;
				}
			}
		} else registeredPGDN = false;
		if (GetAsyncKeyState(VK_LEFT)) {
			if (!registeredLEFT) {
				printf("left\r\n");
				if (showCustomMenu)
					menuManager->Back();
				registeredLEFT = true;
			}
		} else registeredLEFT = false;
		if (GetAsyncKeyState(VK_RETURN)) {
			if (!registeredENTER) {
				registeredENTER = true;
				if (showCustomMenu)
					menuManager->executeAt(m->highlightedIndex);
			}
		} else registeredENTER = false;
		Sleep(1);
	}

	exit_app();
}

void exit_app() {
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


