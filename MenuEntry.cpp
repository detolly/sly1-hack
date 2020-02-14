#include "pch.h"
#include "MenuEntry.h"
#include "MenuManager.h"

MenuEntry::MenuEntry(const char* _name, MenuManager& m) : name(""), manager(m) {
	SetName(_name);
}

void MenuEntry::SetName(const char* newName) {
	for (int a = 0; a < max(strlen(newName), strlen(name)); a++)
		name[a] = newName[a];
}

const char* MenuEntry::GetName() {
	return name;
}

void MenuEntry::execute()
{
	printf("Not defined\r\n");
}

SubMenu::SubMenu(const char* name, MenuManager& manager, SubMenu& parent) : MenuEntry(name, manager), father(parent) {}

SubMenu::~SubMenu() {}
MenuEntry::~MenuEntry() {}

DelegateEntry::DelegateEntry(const char* name, MenuManager& manager, MenuCallback func) : MenuEntry(name, manager), f(func) {}

void SubMenu::AddMenuEntry(MenuEntry* entry) 
{
	entries.push_back(entry);
}

void SubMenu::execute()
{
	manager.SetCurrentlyDisplayingMenu(*this);
}

void DelegateEntry::execute()
{
	f(*this);
}
