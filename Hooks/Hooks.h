#pragma once
#include "../Utilities/Utility.h"
#include "../Utilities/declarations.h"

static bool rainbowMenu{ false };
static bool noclip{ false };
static bool flymode{ false };
static bool charms{ false };
static bool godmode{ false };

stdHook oPickUpCoin;
static void hookPickUpCoin() {
	r->v0.UW[0] = 69; //set register to 69, then it gets stored
	oPickUpCoin();
}

stdHook oCharmDamage;
static void charmDamageHook() {
	if (charms) {
		r->v0.UW[0] = 2;
	}
	else {
		r->v0.UW[0] -= 1;
	}
	oCharmDamage();
}

stdHook oSlyHit;
static void hookedSlyHit() {
	if (godmode) {
		DWORD temp = r->s1.UD[0];
		while (*(DWORD*)(ps2(temp)) != 0)
			temp += 4;
		printf("found a temp: 0x%x, original s1: 0x%x\r\n", temp, r->s1.UW[0]);
		r->a0.UD[0] = temp;
	}
	else  r->a0.UD[0] = r->s1.UD[0];
	oSlyHit();
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

stdHook oSelectInMenu;
static void selectInMenu() {
	r->v0.UW[0] -= (u32)0x62B0;
	oSelectInMenu();
}

stdHook oRenderMenu;
static void renderMenuHook() {
	*(DWORD*)(ps2(r->sp.UW[0] + 64)) = 0x00000000;
	if (counter++ % 256 == 0)
		printStack();
	r->v0.UW[0] = *(u32*)(ps2((DWORD)(r->s2.UW[0] + 0x250)));
	if (showCustomMenu) {
		m->x = 25;
		m->y = 50;
		m->menuScale = 0.7f;
	}
	oRenderMenu();
}

stdHook oChangeOpacity;
static void hookedChangeOpacity() {
	//idk what this is or was
	oChangeOpacity();
}


stdHook oAccessSlyPosition;
static void hkSlyPosition() {
	r->s3.UW[0] = 0x1;
	const int entityId = *(DWORD*)(ps2(r->s0.UW[0]) + 0x08);
	if (counter++ % 256 == 0)
		printStack();
	if (entityId == 5) {
		slyEntity = (ps2(r->s0.UW[0]));
	}
	else if (entityId == 9)
		vehicleEntity = (ps2(r->s0.UW[0]));

	oAccessSlyPosition();
}

stdHook oSetVelocity;
static void hkSetVelocity() {
	r->v0.UW[0] += 0x1858;
	int entityId = *(int*)(ps2(r->s0.UW[0] + 0x8));
	if (entityId == 5 || entityId == 9) {
		if (noclip || flymode) {
			*(float*)(ps2(r->s0.UW[0] + 0x158)) = 20.f;
			if (!showCustomMenu) {
				if (GetAsyncKeyState(VK_UP))
					*(float*)(ps2(r->s0.UW[0] + 0x158)) = 200.f;
				else if (GetAsyncKeyState(VK_DOWN))
					*(float*)(ps2(r->s0.UW[0] + 0x158)) = -200.0f;
			}
		}
	}
	oSetVelocity();
}