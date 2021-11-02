#pragma once

#include <string>
#include <iostream>
#include "smconsole.h"
#include "TextWindow.h"

// Each bit represents a pixel for a 6x5 number
const int Digit_Masks[] =
{
	0b011110110011110011110011011110,	// 0
	0b111100001100001100001100111111,	// 1
	0b011110110011000110001100111111,	// 2
	0b011110110011000110110011011110,	// 3
	0b110011110011111111000011000011,	// 4
	0b111111110000111110000011111110,	// 5
	0b011110110000111110110011011110,	// 6
	0b111111000110001100011000110000,	// 7
	0b011110110011011110110011011110,	// 8
	0b011110110011011111000011011110	// 9
};

// Each bit represents a pixel for a 6x5 letter
const int Letter_Masks[] =
{
	0b011110110011111111110011110011,	// A
	0b111110110011111111110011111111,	// B
	0b011110110011110000110011011110,	// C
	0b111110110011110011110011111110,	// D
	0b111111110000111111110000111111,	// E
	0,									// F
	0,									// G
	0,									// H
	0b111111001100001100001100111111,	// I
	0b111111000110110110110110011110,	// J
	0b110011110110111100110110110011,	// K
	0b110000110000110000110000111111,	// L
	0,									// M
	0,									// N
	0b111111110011110011110011111111,	// O
	0b111111110011111111110000110000,	// P
	0,									// Q
	0b111111110001111111110110110011,	// R
	0b111111110000111111000011111111,	// S
	0b111111001100001100001100001100,	// T
	0b110011110011110011110011111111,	// U
	0,									// V
	0,									// W
	0,									// X
	0,									// Y
	0									// Z
};

// ID for console text and background colour
const int Character_Colour[] = { 0x10, 0x20, 0x30, 0x80, 0x50, 0x60, 0x70, 0x40, 0x90, 0xA0 };

// defines eights characters used to draw the top, bottom, left, right,
// top-left, top-right, bottom-left, bottom-right tiles respectively
const char Border_Tiles_Machine[] =		"\u00b0\u00b0\u00b0\u00b0\u00b0\u00b0\u00b0\u00b0";
const char Border_Tiles_Marquee[] =		"\u00CD\u00BA\u00CD\u00BA\u00C9\u00BB\u00BC\u00C8";
const char Border_Tiles_Component[] =	"\u00DC\u00DB\u00DF\u00DB\u00DC\u00DC\u00DF\u00DF";
const char Border_Tiles_Textbox[] =		"\u00DF\u00DB\u00DC\u00DB\u00DB\u00DB\u00DB\u00DB";

const int Celebrate_Frame_Count = 50;
const int Chip_Update_Frame_Count = 25;
const int Reel_Count = 3;

enum class Outcome
{
	NONE	= -1,
	LOSS	=  0,
	MATCH	=  3,
	TRIPLE	=  5,
	JACKPOT = 10
};

struct Bounds
{
	int x = 0, y = 0, x_len = 1, y_len = 1;
};

struct Reel
{
	int digitHeight = 7;
	int min = 2;
	int length = 6;
	int padding = 3;
	int position = 0;
	int steps = digitHeight * length;
	Bounds bounds;
};

struct ChipDisplay
{
	std::string label;
	Bounds bounds;
};


struct Machine
{
	Bounds reelWindow;
	CTextWindow terminal;
	ChipDisplay chips;
	ChipDisplay bet;
	Bounds marquee;
	Reel reel[Reel_Count];
};

void PrintLetterColumn(int mask, Point p, int col, int padding = 1);
void PrintNumberRow(int mask, Point p, int row, int padding, char background);
void DrawBorder(Bounds bounds, int colour, const char borderChars[]);
void FillBorder(Bounds bounds, int colour);
void SetChipDisplay(ChipDisplay& display, int value);
void DrawReel(Reel& reel, Bounds& bounds);
short GetTargetPosition(Reel& reel, short target);
void PrintMarquee(Bounds& bounds, const std::string& word, Outcome outcome, int seed = 0);
int GetMarqueeColour(Outcome outcome, int seed, int colSeed);
void PrintMarqueeDefault(Bounds& bounds);
void UpdateReel(Machine&, short[], int currentChips, int bet);
void UpdateChips(Machine&, int currChips, int targetChips, Outcome outcome);
Machine BuildMachine(int startingChips);
