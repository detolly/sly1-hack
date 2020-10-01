#pragma once

#include <vector>

class MenuManager;
class MenuEntry;

typedef void(*MenuCallback)(MenuEntry& menu);
class MenuEntry {
private:
	static const int nameLen = 15;
	char name[nameLen];

	MenuManager& m_manager;
	int m_garbage_value1{0};

public:
	MenuEntry(const char*, MenuManager&);
	virtual ~MenuEntry();

	int garbage_value1() { return m_garbage_value1; }
	MenuManager& manager() { return m_manager; };
	void set_garbage_value1(int g) { m_garbage_value1 = g; }
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

