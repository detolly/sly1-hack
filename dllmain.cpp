// dllmain.cpp : Defines the entry point for the DLL application.

#include "Utilities/declarations.h"
#include "Utilities/sigscan.h"
#include "Utilities/memorydump.h"
#include "Utilities/Utility.h"

#include "PCSX2structs/registers.h"

#include "GameStructs/EntityList.h"
#include "GameStructs/Menu.h"
#include "GameStructs/Object.h"
#include "GameStructs/rotation.h"
#include "GameStructs/rgb.h"
#include "GameStructs/Strings.h"
#include "GameStructs/vector3.h"

#include "MenuSystem/MenuManager.h"

#include "Hooks/Hooks.h"
#include "Hooks/HookManager.h"

#include "psapi.h"


// this function has to do with animations: 00124fc0

// 001749A4: jumped to if ledgegrabbing
// 0018FCF0: stores the opacity of stuff - this one will be fun
// 00147ab0 - is called when you pick up a coin / maybe when entities are interacted with

// 20269C98 - pointer to controlled entity

// 0x260294
constexpr int injection_data[] = {
	0x27BDFFEC, // addiu sp, -0x14
	0x3C040026, // lui t0, 0x26
	0x3484EC70, // ori t0, t0, 0xec70
	//0x14880018, // bne a0, t0, 0x18
	0xFFBF0010, // sd ra, sp(0x10)

	0x3C040026, // lui a0, 0x26
	0x3484EC70, // ori a0, a0, 0xec70
	0xAFA50014,	// sw a1, 0x14(sp)
	0x3C050026, // lui a1, 0x26
	0x34A50204,	// ori a1, a1, 0x0204
	0x3C08001a, // lui t0, 0x1a
	0x3508c638, // ori t0, t0, 0xc638
	0x0100F809, // jal t0
	0x00000000, // nop
	0x8FA50014,	// lw a1, 0x14(sp)

	0x3C040026, // lui a0, 0x26
	0x3484EC70, // ori a0, a0, 0xec70
	0x3C050026, // lui at, 0x4040
	0x44816000, // mtcl at, f12
	0x0c06aa2e, // jal gui_set_popup_timeout
	0x00000000, // nop
	
	0x3C040026, // lui a0, 0x26
	0x3484EC70, // ori a0, a0, 0xec70
	0x3C08001a, // lui t0, 0x1a
	0x3508AB60, // ori t0, t0, 0xab60
	0x0100F809, // jal t0
	0x00000000, // nop

	0x3C040026, // lui a0, 0x26
	0x3484EC70, // ori a0, a0, 0xec70
	0x3C08001a, // lui t0, 0x1a
	0x3508A8F0, // ori t0, t0, 0xa8f0
	0x0100F809, // jal t0
	0x00000000, // nop
	0xDFBF0010, // ld ra, 0x4(sp)
	0x03E00008, // jr ra
	0x27BD0014  // addiu sp, 0x14
};

DWORD WINAPI MainThread(LPVOID param) {
	AllocConsole();
	_iobuf* fp;
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

	//*(int*)(0x2018d4f8) = 0x08260294;
	for (int i = 0; i < sizeof(injection_data)/4; i++) {
		*(int*)(ps2(0x260294 + i * 4)) = injection_data[i];
	}

	DWORD pGameStrings = 0x0;
	SignatureScanner::FindSignature(&pGameStrings , 0x20000000, 0x10000000, "Paused", "xxxxxx", 0);
	gameStrings = (Strings*)pGameStrings;
	memcpy(&originalStrings, gameStrings, sizeof(Strings));

	Param = param;
	HMODULE b = GetModuleHandleA("pcsx2.exe");
	MODULEINFO c; 
	GetModuleInformation(GetCurrentProcess(), b, &c, sizeof(c)); 
	DWORD a = 0x0;
	SignatureScanner::FindSignature(&a, (DWORD)c.lpBaseOfDll, c.SizeOfImage, "\x00\x80\xFF\xFF\x00\x80\x07\x00\x00\xC0\x07\x00\x00\x00\x00\x00", "xxxxxxxxxxxxxxxx", -1072);
	printf("Found registers! (0x%x)\r\n", a);
	r = (Regs*)a;



	MenuManager menuManager(":weed:", myStrings, gameStrings);
	MenuEntry placeholder("-", menuManager);
	SubMenu s("General", menuManager, menuManager);
	DelegateEntry godmodee("Godmode: Off", menuManager, [](MenuEntry& entry) {
		godmode = !godmode;
		char c[16] = "Godmode: ";
		strcat_s(c, godmode ? "On" : "Off");
		entry.SetName(c);
	});
	DelegateEntry charmss("Charms: Off", menuManager, [](MenuEntry& entry) {
		charms = !charms;	
		char c[16] = "Charms: ";
		strcat_s(c, charms ? "On" : "Off");
		entry.SetName(c);
		});
	DelegateEntry noclipp("Noclip: Off", menuManager, [](MenuEntry& entry) {
		noclip = !noclip;
		if (noclip) {
			storedSlyCollision = *(DWORD*)(slyEntity() + 0x14);
			*(DWORD*)(slyEntity() + 0x14) = 0;
			storedVehicleCollision = *(DWORD*)(slyEntity() + 0x14);
			*(DWORD*)(vehicleEntity + 0x14) = 0;
		}
		else if (!(*(DWORD*)(slyEntity() + 0x14))) {
			if (!(*(DWORD*)(vehicleEntity + 0x14)))
				*(DWORD*)(vehicleEntity + 0x14) = storedVehicleCollision;
			*(DWORD*)(slyEntity() + 0x14) = storedSlyCollision;
		}
		char c[16] = "Noclip: ";
		strcat_s(c, noclip ? "On" : "Off");
		entry.SetName(c);
	});
	DelegateEntry flymodee("Fly Mode: Off", menuManager, [](MenuEntry& entry) {
		flymode = !flymode;
		char c[16] = "Fly Mode: ";
		strcat_s(c, flymode ? "On" : "Off");
		entry.SetName(c);
		});
	DelegateEntry patchhitbox("Patch Hitbox", menuManager, [](MenuEntry& entry) {
		if (slyEntity)
		{
			DWORD p = *(DWORD*)(slyEntity() + 0x14);
			DWORD j = ps2(p);
			printf("0x%x", j);
			*(int*)(j + 0x34) = 12;
			*(int*)(j + 0x38) = 12;
		} else {
			printf("Don't have sly entity :(\r\n");
		}
	});
	s.AddMenuEntry(&godmodee);
	s.AddMenuEntry(&charmss);
	s.AddMenuEntry(&noclipp);
	s.AddMenuEntry(&flymodee);
	s.AddMenuEntry(&patchhitbox);
	s.AddMenuEntry(&placeholder);

	SubMenu s2("Misc", menuManager, menuManager);
	/*
	DelegateEntry fish("Fish timer: On", menuManager, [](MenuEntry& entry) {
		unlimitedFish = !unlimitedFish;
		char c[16] = "Fish timer: ";
		strcat_s(c, unlimitedFish ? "Off" : "On");
		entry.SetName(c);
	});
	*/
	DelegateEntry fuckedobjectss("Textures: Off", menuManager, [](MenuEntry& entry) {
		fuckedobjects = !fuckedobjects;
		if (fuckedobjects)
		{
			for (int i = 0; i < 1; i++) {
				for (unsigned int k = 0x10; k < 0x120; k++)
					objects[i].colors[k] = 255;
			}
		}
		char c[16] = "Textures: ";
		strcat_s(c, fuckedobjects ? "On" : "Off");
		entry.SetName(c);
	});
	DelegateEntry rainbowmenuu("Rainbow: Off", menuManager, [](MenuEntry& entry) {
		rainbowMenu = !rainbowMenu;
		if (!rainbowMenu)
			*(rgba*)rgbaddress = oldrgb;
		char c[16] = "Rainbow: ";
		strcat_s(c, rainbowMenu ? "On" : "Off");
		entry.SetName(c);
	});
	DelegateEntry show_thing("Show GUI Thing", menuManager, [](MenuEntry& entry) {
		printf("trying to call...\n");
		const char* __asd = "Greetings!";
		memcpy((void*)0x20260204, __asd, strlen(__asd));
		call_native_through_cheat(0x260294, 0x26ec70);
	});
	//s2.AddMenuEntry(&fish);
	s2.AddMenuEntry(&fuckedobjectss);
	s2.AddMenuEntry(&rainbowmenuu);
	s2.AddMenuEntry(&show_thing);
	s2.AddMenuEntry(&placeholder);
	s2.AddMenuEntry(&placeholder);

	SubMenu s3("Location", menuManager, menuManager);
	DelegateEntry savelocation("Save Location", menuManager, [](MenuEntry& entry) {
		if (slyEntity)
		{
			if (storedSlyLocation)
				delete storedSlyLocation;
			Vector3* slyPos = (Vector3*)(slyEntity() + 0x100);
			storedSlyLocation = new Vector3();
			storedSlyLocation->x = slyPos->x;
			storedSlyLocation->y = slyPos->y;
			storedSlyLocation->z = slyPos->z;
			if (!storedVehicleLocation) {
				storedVehicleLocation = new Vector3();
				*storedVehicleLocation = *storedSlyLocation;
			}
		}
		if (vehicleEntity) {
			if (storedVehicleLocation)
				delete storedVehicleLocation;
			if (vehicleEntity) 
			{
				Vector3* vehiclePos = (Vector3*)(vehicleEntity + 0x100);
				storedVehicleLocation = new Vector3();
				storedVehicleLocation->x = vehiclePos->x;
				storedVehicleLocation->y = vehiclePos->y;
				storedVehicleLocation->z = vehiclePos->z;
			}
		}
	});
	DelegateEntry loadlocation("Load Location", menuManager, [](MenuEntry& entry) {
		if (slyEntity && storedSlyLocation)
		{
			for (int i = 0; i < 3; i++) {
				Vector3* slyPos = (Vector3*)(slyEntity() + 0x100);
				*(float*)(slyEntity() + 0x150) = 0.f;
				*(float*)(slyEntity() + 0x154) = 0.f;
				*(float*)(slyEntity() + 0x158) = 0.f;
				*slyPos = *storedSlyLocation;
				if (!storedVehicleLocation && vehicleEntity)
				{
					Vector3* vehiclePos = (Vector3*)(vehicleEntity + 0x100);
					*(float*)(vehicleEntity + 0x150) = 0.f;
					*(float*)(vehicleEntity + 0x154) = 0.f;
					*(float*)(vehicleEntity + 0x158) = 0.f;
					*vehiclePos = *storedSlyLocation;
				}
			}
		}
		if (vehicleEntity && storedVehicleLocation)
		{
			for (int i = 0; i < 3; i++) {
				Vector3* vehiclePos = (Vector3*)(vehicleEntity + 0x100);
				*(float*)(vehicleEntity + 0x150) = 0.f;
				*(float*)(vehicleEntity + 0x154) = 0.f;
				*(float*)(vehicleEntity + 0x158) = 0.f;
				*vehiclePos = *storedVehicleLocation;
			}
		}
	});
	s3.AddMenuEntry(&savelocation);
	s3.AddMenuEntry(&loadlocation);
	s3.AddMenuEntry(&placeholder);
	s3.AddMenuEntry(&placeholder);
	s3.AddMenuEntry(&placeholder);

	SubMenu s5 = SubMenu("Level Warp", menuManager, menuManager);
	std::vector<DelegateEntry*> garbage;

	const uint32_t level_start_thing = 0x20247b74-2*0x2C;
	for (uint32_t current = level_start_thing; current <= 0x202482ac; current += 0x2C) {
		uint32_t a = *(uint32_t*)(current + 0x24);
		const char* b = (const char*)ps2(a);
		DelegateEntry* ent = new DelegateEntry(b, menuManager, [](MenuEntry& entry) {
			call_native_through_cheat(0x001F0428, entry.garbage_value1());
			showCustomMenu = false;
			m->menuStatus = showCustomMenu ? 1 : 3;
			m->isMenuOpen = false;
			*gameStrings = originalStrings;
		});
		garbage.push_back(ent);
		ent->set_garbage_value1(current);
		s5.AddMenuEntry(ent);
	}

	SubMenu s4("Entities", menuManager, menuManager);
	DelegateEntry launchEntities("Launch Up", menuManager, [](MenuEntry& entry) {
		LinkedEntity* entity = (LinkedEntity*)0x208AC1A0; //TODO: HARDCODED
		int i = 0;
		do {
			i++;
			if (entity->address) {
				printf("0x%x\r\n", entity->address);
				if (*(int*)(ps2(entity->address + 0x8)) != 5)
					*(float*)(ps2(entity->address + 0x158)) = 2000.f;
			}
			entity = (LinkedEntity*)(ps2(entity->NEXT));
		} while (entity && i < 75);
	});
	s4.AddMenuEntry(&launchEntities);
	s4.AddMenuEntry(&placeholder);
	s4.AddMenuEntry(&placeholder);
	s4.AddMenuEntry(&placeholder);
	s4.AddMenuEntry(&placeholder);

	menuManager.AddMenuEntry(&s);
	menuManager.AddMenuEntry(&s2);
	menuManager.AddMenuEntry(&s3);
	menuManager.AddMenuEntry(&s5);
	menuManager.AddMenuEntry(&s4);

	menuManager.AddMenuEntry(&placeholder);
	menuManager.AddMenuEntry(&placeholder);

	//  addresses are not hard coded so to speak, they're just references to the actual MIPS game code,
	//  and not the translated x86 that you see in cheat engine f. ex.
	
	/*
	int opacityHookHandle = hookManager.AddHook(HookMember((void*)0x2018FCF0, &hookedChangeOpacity));
	oChangeOpacity = (stdHook)hookManager.Get(opacityHookHandle)->Hook();
	*/

	//int pressedMenuHandle	= hookManager.AddHook((void*)0x20195964, &selectInMenu,		&oSelectInMenu);
	HookManager hookManager;

	//int renderMenuHandle	= hookManager.AddHook((void*)0x20194FDC, &renderMenuHook,		&oRenderMenu);
	//int charmHookHandle		= hookManager.AddHook((void*)0x20192C8C, &charmDamageHook,		&oCharmDamage);
	//int coinHookHandle		= hookManager.AddHook((void*)0x201481C4, &hookPickUpCoin,		&oPickUpCoin);
	//int slyHitHandle		= hookManager.AddHook((void*)0x2013BF30, &hookedSlyHit,			&oSlyHit); 
	//int slyPositionHandle	= hookManager.AddHook((void*)0x2012551C, &hkSlyPosition,		&oAccessSlyPosition);
	//int setVelocityHandle	= hookManager.AddHook((void*)0x20125510, &hkSetVelocity,		&oSetVelocity);
	//int displayTextHandle	= hookManager.AddHook((void*)0x201ac644, &hookedDisplayText,	&oDisplayText);
	//int updateDisplayHandle = hookManager.AddHook((void*)0x201aa8f0, &hookedUpdateDisplay,	&oUpdateDisplay);
	int compressionHook = hookManager.AddHook((void*)0x201378ac, &hookedCompression, &oCompress);

	hookManager.HookAll(param);

	bool registeredDOWN		= false;
	bool registeredUP		= false;
	bool registeredPGDN		= false;
	bool registeredENTER	= false;
	bool registeredLEFT		= false;
	bool registeredEND		= false;

	unsigned long long frames = 0;

	while (true) {
		//if (GetAsyncKeyState(VK_ESCAPE)) break;
		frames++;

		if (frames % 1000 == 0 && slyEntity)
		{
			Vector3* pos = (Vector3*)(slyEntity() + 0x100);
			//printf("Sly Pos: %.2f %.2f %.2f\r\n", pos->x, pos->y, pos->z); TODO: REMOVE COMMENT
			if (vehicleEntity) {
				Vector3* pos = (Vector3*)(vehicleEntity + 0x100);
				//printf("Vehicle Pos: %.2f %.2f %.2f\r\n", pos->x, pos->y, pos->z); //TODO: REMOVE COMMENT
			}
		}
		if (GetAsyncKeyState(VK_DOWN) && !registeredDOWN) {
			registeredDOWN = true;
			if (showCustomMenu) {
				m->highlightedIndex++;
				if (m->highlightedIndex > 4) {
					menuManager.setIndex(true);
					m->highlightedIndex = 4;
				}
			}
		} else registeredDOWN = false;
		if (GetAsyncKeyState(VK_UP) && !registeredUP) {
			registeredUP = true;
			if (showCustomMenu) {
				m->highlightedIndex--;
				if (m->highlightedIndex < 0) {
					menuManager.setIndex(false);
					m->highlightedIndex = 0;
				}
			}
		} else registeredUP = false;
		if (GetAsyncKeyState(VK_NEXT) && !registeredPGDN) {
			registeredPGDN = true;
			if (!m->isMenuOpen || showCustomMenu)
			{
				menuManager.Update();
				showCustomMenu = !showCustomMenu;
				m->menuStatus = showCustomMenu ? 1 : 3;
				m->isMenuOpen = showCustomMenu ? 1 : 0;
				m->menuFade = showCustomMenu ? -1.f : 0.f;
				//*(rgba*)rgbaddress = showCustomMenu ? *(rgba*)rgbaddress : oldrgb;
				*gameStrings = showCustomMenu ? myStrings : originalStrings;
			}
		} else registeredPGDN = false;
		if (GetAsyncKeyState(VK_LEFT) && !registeredLEFT) {
			if (showCustomMenu)
				menuManager.Back();
			registeredLEFT = true;
		} else registeredLEFT = false;
		if (GetAsyncKeyState(VK_RETURN) && !registeredENTER) {
			if (showCustomMenu)
				menuManager.executeAt(m->highlightedIndex);
			registeredENTER = true;
		} else registeredENTER = false;
		if (GetAsyncKeyState(VK_END) && !registeredEND) {
			if (!registeredEND) {
				registeredEND = true;
				goto asd;
				//printf("dumping memory\r\n");
				//MemoryDump::Dump("C:\\Users\\Thomas\\Desktop\\dump.bin", 0x20000000, 0x0FFFFFFF);
				//printf("memory dumped\r\n");
			}
		} else registeredEND = false;
		
		Sleep(50);
	}
asd:
	for (size_t i = 0; i < garbage.size(); i++) {
		delete garbage.at(i);
	}

	printf("Exiting! You can now close the window.\n");
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


