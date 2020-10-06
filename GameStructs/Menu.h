#pragma once

struct Menu {
	int a;					// 0
	char* text;				// 4
	char buffer[520];		// 8
	int menuHandle;			// 528
	char buffer2[16];		// 532
	float x;				// 548
	float y;
	char buffer3[8];
	float menuWidth;
	float menuHeight;
	float menuScale;
	char buffer4[16];
	int menuStatus;
	char buffer5[8];
	int isMenuOpen;
	int menuIndex;
	int highlightedIndex;
	int unknown2;
	char buffer6[12];
	float unknown3;
	char buffer7[8];
	float menuFade;
};
