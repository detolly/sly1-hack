#pragma once
#include "../pch.h"

struct Level {
	//120 bytes total
	int unlocked;
	float timer;
	char buffer[92];
	int bottles;
	char buffer[16];
};