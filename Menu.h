#pragma once
struct Menu {
	int a;
	int randomConstant;
	char buffer[520];
	int menuHandle;
	char buffer2[16];
	float x;
	float y;
	char buffer3[8];
	float menuWidth;
	float menuHeight;
	float menuScale;
	char buffer4[16];
	int menuStatus;
	char buffer5[8];
	int isMenuOpen;
	int b;
	int c;
	char buffer6[28];
	float d;
};

static Menu* makeMenu() {
	Menu* m = new Menu();
	m->randomConstant = 10983168;
	m->menuHandle = 2531856;
	m->x = 100.f;
	m->y = 100.f;
	m->menuWidth = 300;
	m->menuHeight = 300;
	m->menuScale = 1.f;
	m->menuStatus = 2;
	m->isMenuOpen = 1;
	return m;
};

static Menu* copyMenu(DWORD menuPointer) {
	Menu* m = (Menu*)malloc(sizeof(Menu)); 
	if (!m) {
		printf("failed menu copy\r\n");
		return (Menu*)menuPointer;
	}
	memcpy(m, (void*)menuPointer, sizeof(Menu));
	m->menuWidth = 200;
	m->menuHeight = 200;
	m->menuScale = 0.7f;
	m->menuStatus = 1;
}