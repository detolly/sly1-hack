#pragma once

#include "MenuEntry.h"
#include "MenuManager.h"

#define max(a,b) a >= b ? a : b
#define min(a,b) a <= b ? a : b

MenuEntry::MenuEntry(const char* _name, MenuManager& m) : name(), m_manager(m) {
	SetName(_name);
}

void MenuEntry::SetName(const char* newName) {
	memcpy(name, newName, min(nameLen, strlen(newName)+1));
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
	manager().SetCurrentlyDisplayingMenu(*this);
}

void DelegateEntry::execute()
{
	f(*this);
}
