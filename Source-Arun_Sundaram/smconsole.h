#pragma once

#include <windows.h>

HANDLE& GetHandle();
int ShowCursor(bool visible);
void CursorPos(short x, short y);
char GetCharacterAtCursor(short x, short y);
void ResetConsoleText();
void SetColour(int colour);