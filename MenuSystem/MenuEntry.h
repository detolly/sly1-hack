#pragma once

#include <vector>

class MenuManager;
class MenuEntry;

typedef void(*MenuCallback)(MenuEntry& menu);
class MenuEntry {
protected:
	static const int nameLen = 15;
	char name[nameLen];

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

