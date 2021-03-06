#pragma once

#include "../GameStructs/Strings.h"
#include "../Utilities/declarations.h"
#include "MenuEntry.h"

class MenuManager : public SubMenu {
public:
	Strings& hackStrings;
	Strings* gameStrings;
	SubMenu* currentlyDisplaying;
	int startNum;

	MenuManager(const char*, Strings&, Strings*);

	void SetCurrentlyDisplayingMenu(SubMenu&);
	void Back();
	void setIndex(bool);
	void Update();
	void executeAt(int);
};