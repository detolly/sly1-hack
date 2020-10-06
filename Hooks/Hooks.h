#pragma once
#include "../Utilities/Utility.h"
#include "../Utilities/declarations.h"
#include "../PackedData.h"

static bool rainbowMenu{ false };
static bool noclip{ false };
static bool flymode{ false };
static bool charms{ false };
static bool godmode{ false };
static stdHook oPickUpCoin;
static stdHook oCharmDamage;
static stdHook oSlyHit;
static stdHook oSelectInMenu;
static stdHook oRenderMenu;
static stdHook oChangeOpacity;
static stdHook oDisplayText;
static stdHook oAccessSlyPosition;
static stdHook oSetVelocity;
static stdHook oUpdateDisplay;
static stdHook oCompress;
static void hookPickUpCoin();
static void charmDamageHook();
static void hookedSlyHit();
static void selectInMenu();
static void renderMenuHook();
static void hookedChangeOpacity();
static void hookedDisplayText();
static void hookedUpdateDisplay();
static void hkSlyPosition();
static void hkSetVelocity();

static int coinSetAmount = 69;
static void hookPickUpCoin() {
	if (coinSetAmount == 0)
		coinSetAmount = 99;
	r->v0.UW[0] = coinSetAmount--; //set register to 69, then it gets stored
	//callNative(&oUpdateDisplay);
	__asm {
		jmp oPickUpCoin
	};
}

static void charmDamageHook() {
	r->a0.UW[0] = 0x26EC70;
	if (charms) {
		r->v0.UW[0] = 2;
	}
	else {
		r->v0.UW[0] -= 1;
	}
	__asm {
		jmp oCharmDamage
	};
}

static void hookedSlyHit() {
	if (godmode) {
		DWORD temp = r->s1.UD[0];
		while (*(DWORD*)(ps2(temp)) != 0)
			temp += 4;
		printf("found a temp: 0x%x, original s1: 0x%x\r\n", temp, r->s1.UW[0]);
		r->a0.UD[0] = temp;
	}
	else  r->a0.UD[0] = r->s1.UD[0];
	__asm {
		jmp oSlyHit
	};
}


static void hookedCompression() {
	uint32_t ps2_ptr = r->a0.UW[0];
	PackedData* data = (PackedData*)ps2(ps2_ptr);
	printf("Calling Decompression logic on: %08x\nSize: %d\ncompression_key_value: %08x\nSize: %08x\nSize_of_fast_memory: %08x\nptr_data_origin: %08x\ndata_dst_ptr: %08x\n",
		ps2_ptr, data->size, data->compression_key_value, data->size, data->size_of_70002000_segment, data->ptr_data_origin, data->data_dst_ptr);
	__asm {
		jmp oCompress
	};
}

/*
bool unlimitedFish = false;
stdHook oFishTimer;
void fishHook() {
	if (!unlimitedFish) {
		*_f01 = *_f01 + *_f00;
	}
	oFishTimer();
}
*/

static void selectInMenu() {
	r->v0.UW[0] -= (u32)0x62B0;
	__asm {
		jmp oSelectInMenu
	};
}

static void renderMenuHook() {
	*(DWORD*)(ps2(r->sp.UW[0] + 64)) = 0x00000000;
	//if (counter++ % 256 == 0)
	//	printStack();
	r->v0.UW[0] = *(u32*)(ps2((DWORD)(r->s2.UW[0] + 0x250)));
	if (showCustomMenu) {
		m->x = 25;
		m->y = 50;
		m->menuScale = 0.7f;
	}
	__asm {
		jmp oRenderMenu
	};
}

static void hookedChangeOpacity() {
	//idk what this is or was
	__asm {
		jmp oChangeOpacity
	};
}

static void hookedDisplayText() {
	r->s1 = r->a0;
	if (r->a0.UW[0] == 0x26EC70) {
		printf("Writing: %s\n", ps2(r->a1.UW[0]));
		static const char* buttons[] = {
			"asd", "greetings", "yeetings", "&2T&. &2S&."
		};
		static int ind{ 0 };
		const size_t size = 128;
		char buffer[size]{0};
		sprintf_s<size>(buffer, "Selected %d: %s", (ind = ((ind + 1) % (sizeof(buttons) / sizeof(const char*)))), buttons[ind]);
		memcpy((char*)ps2(0x2606F0), buffer, 128u);
		r->a1.UW[0] = 0x2606F0;
	}
	else {
		printf("Found unhandled Text Display: 0x%08x\n", ps2(r->a0.UW[0]));
	}
	__asm {
		jmp oDisplayText
	};
}


static void hkSlyPosition() {
	r->s3.UW[0] = 0x1;
	const int entityId = *(DWORD*)(ps2(r->s0.UW[0]) + 0x8);
	//if (counter++ % 256 == 0)
	//	printStack();
	if (entityId == 9)
		vehicleEntity = (ps2(r->s0.UW[0]));
	__asm {
		jmp oAccessSlyPosition
	};
}

static void hkSetVelocity() {
	r->v0.UW[0] += 0x1858;
	int entityId = *(int*)(ps2(r->s0.UW[0] + 0x8));
	if (entityId == 5 || entityId == 9) {
		if (noclip || flymode) {
			*(float*)(ps2(r->s0.UW[0] + 0x158)) = 18.f;
			if (!showCustomMenu) {
				if (GetAsyncKeyState(VK_UP))
					*(float*)(ps2(r->s0.UW[0] + 0x158)) = 200.f;
				else if (GetAsyncKeyState(VK_DOWN))
					*(float*)(ps2(r->s0.UW[0] + 0x158)) = -200.0f;
			}
		}
	}
	__asm {
		jmp oSetVelocity
	};
}

static void hookedUpdateDisplay() {
	r->sp.UW[0] -= 0x10;
	printf("Updating display for object 0x%08x\n", r->a0.UW[0]);
	__asm {
		jmp oUpdateDisplay
	};
}