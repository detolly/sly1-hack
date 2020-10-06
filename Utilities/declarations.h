#pragma once

#include "../GameStructs/Object.h"
#include "../GameStructs/Strings.h"
#include "../GameStructs/vector3.h"
#include "../GameStructs/rgb.h"
#include "../GameStructs/Menu.h"
#include "../Hooks/HookManager.h"
#include "../PCSX2structs/registers.h"
#include <windows.h>

static hsv h;
static rgba oldrgb;
static DWORD rgbaddress;

static LPVOID Param{nullptr};

static Regs* r{nullptr};
static HookManager* hmanager{nullptr};

//IN GAME

static Strings* gameStrings;
static Strings originalStrings;
static Strings myStrings;

static bool fuckedobjects = false;
static GameObject* objects = (GameObject*)0x20D8E794;

static Menu* m = (Menu*)0x2026FF68;
static DWORD slyEntity() { return *(DWORD*)0x20262E10; };
static DWORD vehicleEntity;
static DWORD storedSlyCollision;
static DWORD storedVehicleCollision;
static Vector3* storedSlyLocation;
static Vector3* storedVehicleLocation;

static bool showCustomMenu = false;
static unsigned long long counter = 0u;

//SETTINGS

