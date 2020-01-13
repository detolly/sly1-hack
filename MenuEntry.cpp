#include "pch.h"
#include "MenuEntry.h"
#include "MenuManager.h"

MenuEntry::MenuEntry(const char* name)
{
	n(this->name, name, 16);
}

void MenuEntry::execute(MenuManager* m)
{
	printf("Not defined\r\n");
}

SubMenu::SubMenu(const char* name, SubMenu* fatherS) : MenuEntry(name)
{
	father = fatherS;
	entries = new std::vector<MenuEntry*>();
}

void SubMenu::AddMenuEntry(MenuEntry* entry) 
{
	entries->push_back(entry);
}

void SubMenu::execute(MenuManager* m)
{
	m->startNum = 0;
	m->SetCurrentlyDisplayingMenu(this);
}

void DelegateEntry::execute(MenuManager* m)
{
	f(name);
}

DelegateEntry::DelegateEntry(const char* name, MenuCallback func) : MenuEntry(name) {
	f = func;
}