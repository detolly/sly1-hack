#pragma once
#include "pch.h"
#include "MenuManager.h"

MenuManager::MenuManager(const char* name, Strings& hackStrings, Strings* gameStrings) : SubMenu(name, *this, *this), currentlyDisplaying(this), hackStrings(hackStrings), gameStrings(gameStrings) {};

void MenuManager::SetCurrentlyDisplayingMenu(SubMenu& menu) {
	currentlyDisplaying = &menu;
	Update();
	*gameStrings = hackStrings;
}

void MenuManager::Back() {
	startNum = 0;
	SetCurrentlyDisplayingMenu(currentlyDisplaying->father);
}

void MenuManager::setIndex(bool isBiggerIndex) {
	if ((startNum == 0 && !isBiggerIndex) || (startNum == min(max(currentlyDisplaying->entries.size() - 5, 0), currentlyDisplaying->entries.size()) && isBiggerIndex))
		return;
	startNum = isBiggerIndex ? startNum + 1 : startNum - 1;
	Update();
	*gameStrings = hackStrings;
}

void MenuManager::Update() {
	static char* realquitgame = (char*)0x2026A540;
	n(hackStrings.headline, currentlyDisplaying->GetName(), 16);
	for (int i = 0; i < min(currentlyDisplaying->entries.size(), 5); i++) {
		if (i == 4)
			n(realquitgame, currentlyDisplaying->entries.at(startNum + i)->GetName(), 16);
		char* place[] = { hackStrings.returnToGame, hackStrings.viewmap, hackStrings.options, hackStrings.exitToHideout, hackStrings.quitgame };
		n(place[i], currentlyDisplaying->entries.at(startNum + i)->GetName(), 16);
	}
}

void MenuManager::executeAt(int index) {
	currentlyDisplaying->entries.at(index + startNum)->execute();
	Update();
	*gameStrings = hackStrings;
}