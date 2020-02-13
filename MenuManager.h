#pragma once
#include "pch.h"
#include "Strings.h"
#include "MenuEntry.h"

class MenuManager : public SubMenu {
public:
	Strings& hackStrings;
	Strings* gameStrings;
	SubMenu* currentlyDisplaying;
	int startNum = 0;

	MenuManager(const char*, Strings&, Strings*);

	void SetCurrentlyDisplayingMenu(SubMenu&);
	void Back();
	void setIndex(bool);
	void Update();
	void executeAt(int);
};