#pragma once
#include "pch.h"
#include "vector"
#include "Strings.h"

class MenuManager;
class MenuEntry;

typedef void(*MenuCallback)(MenuEntry& menu);
class MenuEntry {
private:
	const char* name;

public:
	MenuEntry(const char* name);
	virtual ~MenuEntry() {
		delete name;
	};
	virtual void execute(MenuManager*);
	void SetName(const char*);
	const char* GetName();
};

class DelegateEntry : public MenuEntry {
public:
	MenuCallback f;

	DelegateEntry(const char* name, MenuCallback);
	void execute(MenuManager*) override;
};

class SubMenu : public MenuEntry {
public:
	std::vector<MenuEntry*> entries;
	SubMenu& father;

	SubMenu(const char* name, SubMenu&);
	~SubMenu();
	void AddMenuEntry(MenuEntry*);
	void execute(MenuManager*) override;
};

