#include "pch.h"
#include "MenuEntry.h"
#include "MenuManager.h"

MenuEntry::MenuEntry(const char* _name) : name(nullptr) {
	SetName(_name);
}

void MenuEntry::SetName(const char* newName) {
	if (name)
		delete name;
	char* allocated = new char[15];
	n(allocated, newName, 15);
	name = allocated;
}

const char* MenuEntry::GetName() {
	return name;
}

void MenuEntry::execute(MenuManager* m)
{
	printf("Not defined\r\n");
}

SubMenu::SubMenu(const char* name, SubMenu& father) : MenuEntry(name), father(father) {
	entries = new std::vector<MenuEntry*>();
}

SubMenu::~SubMenu()
{
	delete entries;
}

DelegateEntry::DelegateEntry(const char* name, MenuCallback func) : MenuEntry(name), f(func) {}

void SubMenu::AddMenuEntry(MenuEntry* entry) 
{
	entries->push_back(entry);
}

void SubMenu::execute(MenuManager* m)
{
	m->startNum = 0;
	m->SetCurrentlyDisplayingMenu(*this);
}

void DelegateEntry::execute(MenuManager* m)
{
	f(*this);
}
