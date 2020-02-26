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

static Strings createHookStrings(Strings def) {
	Strings s;
	memcpy(&s, &def, sizeof(Strings));
	return s;
}
