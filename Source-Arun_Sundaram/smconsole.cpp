#include "smconsole.h"

// Returns a reference to the console handler
HANDLE& GetHandle()
{
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	return hConsole;
}

// Toggles the visibility of the console cursor
int ShowCursor(bool visible)
{
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(GetHandle(), &cursorInfo);
	cursorInfo.bVisible = visible;
	SetConsoleCursorInfo(GetHandle(), &cursorInfo);
	return 0;
}

// Moves the cursor to the position (x, y)
void CursorPos(short x, short y) {
	CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
	GetConsoleScreenBufferInfo(GetHandle(), &screenBufferInfo);
	screenBufferInfo.dwCursorPosition = { x, y };
	SetConsoleCursorPosition(GetHandle(), screenBufferInfo.dwCursorPosition);
}

// Returns the character at the position (x,y)
char GetCharacterAtCursor(short x, short y)
{
	CHAR_INFO charInfo;
	SMALL_RECT copyBounds = { x, y, x, y };
	return ReadConsoleOutput(GetHandle(), &charInfo, { 1,1 }, { 0, 0 }, &copyBounds)
		? charInfo.Char.AsciiChar
		: '\0';
}

// Sets the text and background colour using a 2-digit hexadecimal number
void SetColour(int colour)
{
	SetConsoleTextAttribute(GetHandle(), colour);
}

// Returns the colour scheme back to white text on a black 
void ResetConsoleText()
{
	SetColour(0x0F);
}
