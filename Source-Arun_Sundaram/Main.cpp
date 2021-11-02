#include "Main.h"

int main()
{
	srand((unsigned int)time(NULL));
	HWND console = GetConsoleWindow();
	MoveWindow(console, 0, 0, 500, 800, TRUE); // resize cmd window
	ShowCursor(false);
	GameData data = { Initial_Chips, 0 };
	Machine machine = BuildMachine(data.chips);

	bool bGameActive = true;
	while (bGameActive)
	{
		ShowMenu(data, machine.terminal);
		int choice = PromptInt(machine.terminal);
		if (choice == 3)
		{
			bGameActive = false;
		}
		else if (choice == 2)
		{
			ShowCredits(machine.terminal);
		}
		else if (choice == 1)
		{
			EnterGame(data, machine);
		}
	}
	return 0;
}

void ShowMenu(GameData &data, CTextWindow &terminal)
{
	terminal.Clear();
	terminal.Print("Player's chips: $");
	terminal.Print(std::to_string(data.chips));
	terminal.Print("\n\n");
	terminal.Print("1) Play Slots\n2) Credits\n3) Quit Slot Machine\n\n");
}

// Returns an array of three random numbers
short* GenerateSlots(Reel *reels)
{
	static short slots[Reel_Count];
	for (int i = 0; i < Reel_Count; ++i)
	{
		slots[i] = RandomSlot(reels[i].min, reels[i].length);
	}
	return slots; 
}

// Randomly selects a number from a reel
short RandomSlot(int min, int length)
{
	return (short)(min + rand() % length);
}

// Returns the outcome of the bet given the combination
Outcome SolveOutcome(short slots[])
{
	bool matchA = slots[0] == slots[1];
	bool matchB = slots[1] == slots[2];
	bool matchC = slots[0] == slots[2];

	if ( matchA && matchB )
	{
		return slots[0] == 7 ? Outcome::JACKPOT : Outcome::TRIPLE;
	}
	else if ( matchA || matchB || matchC)
	{
		return Outcome::MATCH;
	}
	else
	{
		return Outcome::LOSS;
	}
}

// Displays a scrolling credit sequence
void ShowCredits(CTextWindow &terminal)
{
	terminal.Clear();
	terminal.MoveTo(terminal.Origin().x, terminal.Origin().y + terminal.Height() - 1);
	for (std::string line : Credit_List)
	{
		terminal.Print(line);
		Sleep(400);
	}
	PromptInt(terminal);
	for (int i = 0; i < terminal.Height()/2 + 1; ++i)
	{
		terminal.ScrollUp(1);
		Sleep(200);
	}
}

// The main game loop. Takes bets, solves the outcome, and updates the machine.
void EnterGame(GameData &data, Machine &machine)
{
	int bet;
	CTextWindow& terminal = machine.terminal;
	do
	{
		if (data.chips == 0)
		{
			terminal.Print("You're out of chips! (Enter to return): \n");
			PromptInt(terminal);
			return;
		}
		else
		{
			terminal.Print("Enter a bet (0 to return): ");
		}

		bet =  Auto_Bet_Value ? Auto_Bet_Value : PromptInt(terminal);

		if (bet < 0)
		{
			terminal.Print("\nERROR: Please enter a positive integer:\n\n");
			continue;
		}
		else if (bet > data.chips)
		{
			terminal.Print("\nERROR: You don't have enough chips!\n\n");
			continue;
		}
		else if (bet == 0)
		{
			SetChipDisplay(machine.bet, data.bet);
			continue;
		}

		data.bet = bet;
		short* slots = GenerateSlots(machine.reel);

		SetChipDisplay(machine.bet, data.bet);
		SetChipDisplay(machine.chips, data.chips);
		UpdateReel(machine, slots, data.chips, data.bet);
		data.chips -= bet;

		Outcome outcome = SolveOutcome(slots);
		int reward = bet * (int)outcome;

		if ((int)outcome > 0)
		{
			bool betOverflow = bet > INT_MAX / (int)outcome;
			bool targetOverflow = data.chips > INT_MAX - reward;
			if (betOverflow || targetOverflow)
			{
				reward = INT_MAX - data.chips;
				terminal.Print("\nYou have the maximum number of chips!");
			}
			BellSound();
			UpdateChips(machine, data.chips, data.chips + reward, outcome);
		}
		data.chips += reward;

		char cslots[] = { '(', '0' + (char)slots[0], ',', '0' + (char)slots[1], ',', '0' + (char)slots[2], ')', '\0' };
		terminal.Print('\n');
		terminal.Print(cslots);
		terminal.Print(" You receive ");
		terminal.Print(std::to_string(reward));
		terminal.Print(" credits.\nNew total: ");
		terminal.Print(std::to_string(data.chips));
		terminal.Print("\n\n");

	} while (bet != 0);
}

// Prompts the user to enter a non-negative integer, returns -1 if invalid
int PromptInt(CTextWindow& terminal)
{
	ShowCursor(true);
	std::string input;
	std::getline(std::cin, input);
	ShowCursor(false);
	if (IsNumber(input))
	{
		return StringToInt(input);
	}
	return -1;
}

// Returns true if string is a non-negative integer
bool IsNumber(std::string& str)
{
	if (str.length() == 0)
	{
		return false;
	}
	for (int i = str.length() - 1; i >= 0; --i)
	{
		if (str[i] > '9' || str[i] < '0')
		{
			return false;
		}
	}
	return true;
}

// Converts a string to a non-negative integer, returns -1 if invalid
int StringToInt(std::string& str)
{
	long num = 0, mult = 1, digit;
	if (str.length() > 9)
	{
		return -1;
	}
	for (int i = str.length() - 1; i >= 0; --i, mult *= 10)
	{
		digit = (long)(str[i] - '0');
		num += digit * mult;
	}
	if (num > (long)INT_MAX)
	{
		return -1;
	}
	return (int)num;
}

// Causes the console to emit a bell sound
void BellSound()
{
	if (!Mute_Bell)
	{
		std::cout << '\a';
	}
}
