#include "smdisplay.h"

// Draws a column from a 6x5 pixel letter represented by the last 30 bits of an integer
void PrintLetterColumn(int mask, Point p, int col, int padding)
{
	int dy = 0;
	for (int i = 0; i < padding; ++i, ++dy)
	{
		CursorPos(p.x, p.y + dy);
		std::cout << ' ';
	}
	for (int i = 29; i > 0; i -= 6, ++dy)
	{
		CursorPos(p.x, p.y + dy);
		bool filled = mask & (1 << (i - col));
		std::cout << (filled ? '\u00DB' : ' ');
	}
	for (int i = 0; i < padding; ++i, ++dy)
	{
		CursorPos(p.x, p.y + dy);
		std::cout << ' ';
	}
}

// Draws a row from a 6x5 pixel number represented by the last 30 bits of an integer
void PrintNumberRow(int mask, Point p, int row, int padding, char background)
{
	CursorPos(p.x, p.y);
	for (int i = 0; i < padding; ++i)
	{
		std::cout << background;
	}
	int start = 29 - row * 6;
	for (int i = 0; i < 6; ++i)
	{
		bool filled = mask & (1 << (start - i));
		std::cout << (filled ? '\u00DB' : background);
	}
	for (int i = 0; i < padding; ++i)
	{
		std::cout << background;
	}
}

// Draws the passed value of chips to the display
void SetChipDisplay(ChipDisplay& display, int value)
{
	ResetConsoleText();
	int digit = 10, num = value;
	do {
		--digit;
		num /= 10;
	} while (num > 0);

	int x = display.bounds.x + display.label.length() + 1;
	CursorPos(x, display.bounds.y + 1);
	for (int i = digit; i > 0; --i)
	{
		std::cout << ' ';
	}
	std::cout << value << ' ';
}

// Draws a border with 'colour' defining the text and background colour.
// 'tiles' defines eights characters used to draw the top, bottom, left, right,
// top-left, top-right, bottom-left, bottom-right tiles respectively
void DrawBorder(Bounds bounds, int colour, const char tiles[])
{
	SetColour(colour);
	CursorPos(bounds.x + 1, bounds.y);
	for (int i = 1; i < bounds.x_len; ++i)
	{
		std::cout << tiles[0];
	}
	CursorPos(bounds.x + 1, bounds.y + bounds.y_len - 1);
	for (int i = 1; i < bounds.x_len; ++i)
	{
		std::cout << tiles[2];
	}
	for (int i = 1; i < bounds.y_len; ++i)
	{
		CursorPos(bounds.x, bounds.y + i);
		std::cout << tiles[3];
		CursorPos(bounds.x + bounds.x_len - 1, bounds.y + i);
		std::cout << tiles[1];
	}
	CursorPos(bounds.x, bounds.y);
	std::cout << tiles[4];
	CursorPos(bounds.x + bounds.x_len - 1, bounds.y);
	std::cout << tiles[5];
	CursorPos(bounds.x + bounds.x_len - 1, bounds.y + bounds.y_len - 1);
	std::cout << tiles[6];
	CursorPos(bounds.x, bounds.y + bounds.y_len - 1);
	std::cout << tiles[7];
	ResetConsoleText();
}

// Fills the region with the passed backgorund colour
void FillBorder(Bounds bounds, int colour)
{
	SetColour(colour);
	for (int i = 0; i < bounds.x_len; ++i)
	{
		for (int j = 0; j < bounds.y_len; ++j)
		{
			CursorPos(bounds.x + i, bounds.y + j);
			std::cout << ' ';
		}
	}
	ResetConsoleText();
}

// Draws a Reel using it's current poition
void DrawReel(Reel& reel, Bounds& bounds)
{
	for (int i = 0; i < bounds.y_len; ++i)
	{
		int index = ((reel.position + i) / reel.digitHeight) % reel.length + 2; // the displayed number
		int row = (reel.position + i) % reel.digitHeight;
		int number = Digit_Masks[index];

		char background;
		// For dithering at the top/bottom of a reel
		if (i < 3 || (bounds.y_len - i) < 4) {
			background = '\u00b2' - min(i, bounds.y_len - i - 1); 
		}
		else
		{
			background = ' ';
		}
		SetColour(Character_Colour[index]);
		if (row == 0 || row == reel.digitHeight - 1) // Draw padding row
		{
			CursorPos(bounds.x, bounds.y + i);
			for (int i = 0; i < 6 + 2 * reel.padding; ++i)
				std::cout << background;
		}
		else
		{
			PrintNumberRow(number, { bounds.x, bounds.y + i }, row - 1, reel.padding, background);
		}
	}
	ResetConsoleText();
}

// Prints the text marquee which can be animated by using a different seen number
void PrintMarquee(Bounds& bounds, const std::string& word, Outcome outcome, int seed)
{
	int colSeed = seed;
	Point p = { bounds.x + 1, bounds.y + 1 };
	int wordStart = ((bounds.x_len - 1) - (7 * word.length())) / 2; // centers the text

	for (int i = 0; i < wordStart; ++i)
	{
		SetColour(GetMarqueeColour(outcome, seed, colSeed));
		PrintLetterColumn(0, p, 0);
		++colSeed;
		++p.x;
	}

	for (char c : word)
	{
		for (int row = 0; row < 6; ++row)
		{
			SetColour(GetMarqueeColour(outcome, seed, colSeed));
			PrintLetterColumn(Letter_Masks[c - 'A'], p, row);
			++colSeed;
			++p.x;
		}
		SetColour(GetMarqueeColour(outcome, seed, colSeed));
		PrintLetterColumn(0, p, 0);
		++colSeed;
		++p.x;
	}

	while (p.x < bounds.x + bounds.x_len - 1)
	{
		SetColour(GetMarqueeColour(outcome, seed, colSeed));
		PrintLetterColumn(0, p, 0);
		++colSeed;
		++p.x;
	}
}

// Returns the colour for a row in the marquee. Used for animating bat outcomes.
int GetMarqueeColour(Outcome outcome, int seed, int colSeed)
{
	switch (outcome)
	{
	case Outcome::JACKPOT:
		return Character_Colour[colSeed / 2 % 9];
	case Outcome::TRIPLE:
		return Character_Colour[colSeed / 2 % 9] >> 4;
	case Outcome::MATCH:
		return (seed / 32 % 2 == 0) ? Character_Colour[seed / 2 % 9] : Character_Colour[seed / 2 % 9] >> 4;
	case Outcome::NONE:case Outcome::LOSS:
	default:
		return 0x0E;
	}
}

void PrintMarqueeDefault(Bounds& bounds)
{
	PrintMarquee(bounds, "SLOTS", Outcome::NONE);
}

// Animates the reels spinning and chips being removed for the bet
void UpdateReel(Machine& machine, short combination[], int currChips, int bet)
{
	int targetChips = currChips - bet;
	int chipDiff = max(bet / Chip_Update_Frame_Count, 1);

	Reel* reels = machine.reel;
	int targets[] = {
		GetTargetPosition(reels[0], combination[0]),
		GetTargetPosition(reels[1], combination[1]),
		GetTargetPosition(reels[2], combination[2])
	};

	ResetConsoleText();

	bool active[] = { true, true, true };  // true if the reel is spinning
	int minIterations = 100;
	for (int i = 0; active[0] || active[1] || active[2]; ++i)
	{
		for (int n = 0; n < Reel_Count; ++n)
		{
			Reel& r = reels[n];
			if (!active[n])
				continue;
			else if (i < minIterations || r.position % (r.digitHeight * r.length) != targets[n])
			{
				r.position = (r.position + r.digitHeight * r.length - 1) % (r.digitHeight * r.length);
				DrawReel(r, r.bounds);
			}
			else {
				active[n] = false;
				minIterations += 100;
			}
		}
		// Animate removing chips
		if (currChips - chipDiff < targetChips)
		{
			SetChipDisplay(machine.chips, currChips = targetChips);
		}
		if (currChips > targetChips)
		{
			SetChipDisplay(machine.chips, currChips -= chipDiff);
		}
	}
}

// Return the reel position to stop at
short GetTargetPosition(Reel& reel, short target)
{
	short target_row = (target - reel.min) * reel.digitHeight - reel.bounds.y_len / 2;
	short totalSteps = reel.length * reel.digitHeight;
	return (target_row + totalSteps) % totalSteps + 3;
}

// Animates marquee and winnings being added to the chips display
void UpdateChips(Machine& machine, int currChips, int targetChips, Outcome outcome = Outcome::NONE)
{
	int chipDif = max((targetChips - currChips) / Chip_Update_Frame_Count, 1);
	int loopCount = Celebrate_Frame_Count * (outcome == Outcome::MATCH ? 1 : 2);
	for (int i = 0; i < loopCount; ++i)
	{
		std::string text;
		switch (outcome) {
		case Outcome::MATCH: text = "DOUBLE"; break;
		case Outcome::TRIPLE: text = "TRIPLE"; break;
		case Outcome::JACKPOT: text = "JACKPOT"; break;
		default: text = "SLOTS";
		}
		PrintMarquee(machine.marquee, text, outcome, i);
		if (currChips > targetChips - chipDif)
		{
			SetChipDisplay(machine.chips, currChips = targetChips);
		}
		else if (currChips < targetChips)
		{
			SetChipDisplay(machine.chips, currChips += chipDif);
		}
	}
	PrintMarqueeDefault(machine.marquee);
	ResetConsoleText();
}


// Builds and displays the slot machine and its components
Machine BuildMachine(int startingChips)
{
	// main body
	Bounds machineBounds = { 4, 10, 52, 36 };
	FillBorder(machineBounds, 0xAf);
	
	DrawBorder(machineBounds, 0x20, Border_Tiles_Machine);

	// marquee
	Bounds marquee = { machineBounds.x, machineBounds.y - 9, 52, 9 };
	DrawBorder(marquee, 0x0E, Border_Tiles_Marquee);
	PrintMarqueeDefault(marquee);

	// reels
	Reel r1, r2, r3;
	Reel* reels[] = { &r1, &r2, &r3 };

	int reelWidth = 6 + 2 * r1.padding;
	int windowWidth = 4 + 3 * reelWidth;
	Bounds reelWindow = { machineBounds.x + 6, machineBounds.y + 2, windowWidth, 15 };
	FillBorder({ reelWindow.x - 1, 12, windowWidth + 2, 15 }, 0xF);

	for (int i = 0; i < Reel_Count; ++i)
	{
		Reel& r = *reels[i];
		r.bounds = { reelWindow.x + i*(2 +reelWidth), reelWindow.y, 8, reelWindow.y_len };
		r.position = rand() % r.steps;
		DrawReel(r, r.bounds);
	}

	Bounds reelBorderBounds = { reelWindow.x - 2, reelWindow.y - 1, reelWindow.x_len + 4, reelWindow.y_len + 2 };
	DrawBorder(reelBorderBounds, 0xAE, Border_Tiles_Component);

	// chip displays 
	ChipDisplay chipDisplay = { "CHIPS ", {machineBounds.x + 4, reelWindow.y + reelWindow.y_len + 1, 19, 3} };
	ChipDisplay betDisplay = { " BET  ", {machineBounds.x + machineBounds.x_len - 23, reelWindow.y + reelWindow.y_len + 1, 19, 3} };
	DrawBorder(chipDisplay.bounds, 0xAE, Border_Tiles_Component);
	DrawBorder(betDisplay.bounds, 0xAE, Border_Tiles_Component);

	// textbox
	Bounds textbox = { machineBounds.x + 4, machineBounds.y + 22, 44, 12 };
	FillBorder(textbox, 0x0F);
	DrawBorder(textbox, 0x0E, Border_Tiles_Textbox);

	CTextWindow terminal({ textbox.x + 2, textbox.y + 1 }, 39, 10);

	// title chip display
	terminal.MoveTo(chipDisplay.bounds.x + 1, chipDisplay.bounds.y + 1);
	terminal.Print(chipDisplay.label, 0xE0);
	SetChipDisplay(chipDisplay, startingChips);
	terminal.MoveTo(betDisplay.bounds.x + 1, betDisplay.bounds.y + 1);
	terminal.Print(betDisplay.label, 0xE0);
	SetChipDisplay(betDisplay, 0);

	Machine machine = { reelWindow, terminal, chipDisplay, betDisplay, marquee, {r1, r2, r3} };

	ResetConsoleText();
	return machine;
}


