#pragma once
#include <vector>

struct Strings {
	char headline[16];
	char buffer[2928];
	char returnToGame[16];
	char viewmap[16];
	char options[16];
	char newgame[16];
	char loadgame[16];
	char savegame[16];
	char centerScreen[16];
	char buffer1[8];
	char back[8];
	char restartRace[16];
	char restartSprint[16];
	char restartLevel[16];
	char exitLevel[16];
	char exitToHideout[16];
	char controls[16];
	char empty[8];
	char quitgame[16];
	char saveandquit[16];
	char vibrationson[16];
	char vibrationsoff[16];
	char musicon[16];
	char musicoff[16];
	char speakersstereo[24];
	char speakersmono[16];
};
char* realquitgame = (char*)0x2026A540;

typedef void(__cdecl* MenuCallback)(char*);

static void n(char* a, const char* b, size_t s) {
	std::strncpy(a, b, s);
}

static Strings createHookStrings(Strings def) {
	Strings s;
	memcpy(&s, &def, sizeof(Strings));
	return s;
}

struct Hack {
public:
	char name[16];
	MenuCallback f;

	Hack(char* name, MenuCallback func) {
		n(this->name, name, 16);
		this->f = func;
	}
};

class MenuManager {
private:
	Strings* hackStrings;
	Strings* gameStrings;
	std::vector<Hack>* hacks;
	int startNum = 0;

public:

	MenuManager(Strings* hackStrings, Strings* gameStrings) {
		hacks = new std::vector<Hack>();
		this->gameStrings = gameStrings;
		this->hackStrings = hackStrings;
		n(this->hackStrings->headline, "___HookerBeer___", 16);
		n(realquitgame, "", 16);
	}
	
	void AddMenuEntry(char* name, MenuCallback function) {
		hacks->push_back(Hack(name, function));
		Update();
	}

	void setIndex(bool isBiggerIndex) {
		if ((startNum == 0 && !isBiggerIndex) || (startNum == min(max(hacks->size()-5,0),hacks->size()) && isBiggerIndex))
			return;
		startNum = isBiggerIndex ? startNum + 1 : startNum - 1; //no error checking, is kinda needed tbh
		Update();
		*gameStrings = *hackStrings;
	}

	void Update() {
		for (int i = 0; i < min(hacks->size(), 5); i++) {
			if (i == 4)
				n(realquitgame, hacks->at(startNum + i).name, 16);
			char* place[] = { hackStrings->returnToGame, hackStrings->viewmap, hackStrings->options, hackStrings->exitToHideout, hackStrings->quitgame };
			n(place[i], hacks->at(startNum + i).name, 16);
		}
	}

	void execute(int index) {
		hacks->at(index + startNum).f(hacks->at(index + startNum).name);
		Update();
		*gameStrings = *hackStrings;
	}
};

