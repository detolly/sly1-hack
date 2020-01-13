#pragma once
#include "pch.h"
#include "vector"
#include "Strings.h"

class MenuManager;

typedef void(__cdecl* MenuCallback)(char*);
class MenuEntry {
public:
	char name[16];

	MenuEntry(const char*);
	virtual void execute(MenuManager*);
};

class SubMenu : public MenuEntry {
public:
	SubMenu* father;
	std::vector<MenuEntry*>* entries;

	SubMenu(const char*, SubMenu*);
	void AddMenuEntry(MenuEntry*);
	void execute(MenuManager*) override;
};

class DelegateEntry : public MenuEntry {
public:
	MenuCallback f;

	DelegateEntry(const char*, MenuCallback);
	void execute(MenuManager*) override;
};

