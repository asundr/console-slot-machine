#pragma once
#include <iostream>
#include <string>
#include <limits>
#include "smdisplay.h"

const int Auto_Bet_Value = 0; // automatically places bets, used for testing
const bool Mute_Bell = false;
const int Initial_Chips = 2000;

const std::string Credit_List[] =
{
	"\t\t\t\tCredits",
	"\n", "\n",
	"\n\t\t\t  Programmer:",
	"\n\t\t\t Arun Sundaram",
	"\n", "\n",
	"\n\t\t  ASCII Art Numbers:",
	"\n   https://gist.github.com/yuanqing/", "\n\t\t ffa2244bd134f911d365",
	"\n", "\n",	"\n", "\n",
	"\n\t\t  Thanks for playing!",
	"\n\t\tPress Enter to return.",
	"\n", "\n",	"\n", "\n"
};

struct GameData
{
	int chips;
	int bet = 0;
};

void ShowMenu(GameData& data, CTextWindow& terminal);
void EnterGame(GameData& data, Machine& machine);
short* GenerateSlots(Reel* reels);
short RandomSlot(int min, int length);
Outcome SolveOutcome(short slots[]);
int PromptInt(CTextWindow& terminal);
void ShowCredits(CTextWindow& terminal);
bool IsNumber(std::string& str);
int StringToInt(std::string& str);
void BellSound();