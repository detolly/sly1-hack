#pragma once
#include "pch.h"
#include "vector"
#include "Strings.h"

class MenuManager;
class MenuEntry;

typedef void(*MenuCallback)(MenuEntry& menu);
class MenuEntry {
protected:
	char name[16];
	MenuManager& manager;

public:
	MenuEntry(const char*, MenuManager&);
	virtual ~MenuEntry();
	virtual void execute();
	void SetName(const char*);
	const char* GetName();
};

class DelegateEntry : public MenuEntry {
public:
	MenuCallback f;

	DelegateEntry(const char* name, MenuManager&, MenuCallback);
	void execute() override;
};

class SubMenu : public MenuEntry {
public:
	std::vector<MenuEntry*> entries;
	SubMenu& father;

	SubMenu(const char* name, MenuManager&, SubMenu&);
	~SubMenu();
	void AddMenuEntry(MenuEntry*);
	void execute() override;
};

