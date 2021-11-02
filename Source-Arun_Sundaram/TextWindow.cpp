#include "TextWindow.h"

CTextWindow::CTextWindow(Point _origin, short _width, short _height) 
		: m_origin(_origin), m_curr(_origin), m_width(_width), m_height(_height)
{
}

CTextWindow::~CTextWindow()
{
}

Point CTextWindow::Origin() const
{
	return m_origin;
}

short CTextWindow::Height() const
{
	return m_height;
}

short CTextWindow::Width() const
{
	return m_width;
}

// Moves the terminal cursor to (x,y) within the terminal's coord system
void CTextWindow::MoveTo(short x, short y)
{
	m_curr = { x, y };
	CursorPos(x, y);
}

// Overload
void CTextWindow::MoveTo(Point p)
{
	MoveTo(p.x, p.y);
}

// Removes all text from the window
void CTextWindow::Clear()
{
	ResetConsoleText();
	for (short i = 0; i < m_height; ++i)
	{
		CursorPos(m_origin.x, m_origin.y + i);
		for (short j = 0; j < m_width; ++j)
		{
			std::cout << ' ';
		}
	}
	MoveTo(m_origin);
}

// Removes all text from th currnt line;
void CTextWindow::ClearLine()
{
	MoveTo(m_origin.x, m_curr.y);
	for (short i = 0; i < m_width; ++i)
	{
		std::cout << ' ';
		m_curr.x++;
	}
	MoveTo(m_curr);
}

// Prints a acharacter at the current cursor position
void CTextWindow::Print(char c)
{
	if (c == '\n' || m_curr.x == m_origin.x + m_width)
	{
		if (m_curr.y < m_origin.y + m_height - 1)
		{
			++m_curr.y;
		}
		else
		{
			ScrollUp(1);
		}
		MoveTo(m_origin.x, m_curr.y);
		if (c == '\n')
			return;
	}
	MoveTo(m_curr);
	std::cout << c;
	m_curr.x += c == '\t' ? 4 : 1;
}

// Print a character with the specified text and background colour id
void CTextWindow::Print(char c, int colour)
{
	SetColour(colour);
	Print(c);
}

// Prints a string 
void CTextWindow::Print(const char str[])
{
	for (short i = 0; str[i] != '\0'; ++i)
	{
		Print(str[i]);
	}
}

// Prints a string
void CTextWindow::Print(const std::string str)
{
	Print(str.c_str());
}
 
// Prints a string with the specified text and background colour id
void CTextWindow::Print(const char str[], int colour)
{
	SetColour(colour);
	Print(str);
}

// Prints a string with the specified text and background colour id
void CTextWindow::Print(const std::string str, int colour)
{
	Print(str.c_str(), colour);
}

// Shifts every line of text up by n lines
void CTextWindow::ScrollUp(int n)
{
	char* line = new char[m_width + 1];
	line[m_width] = '\0';
	for (short i = n; i < m_height; ++i)
	{
		for (short j = 0; j < m_width; ++j)
		{
			line[j] = GetCharacterAtCursor(m_origin.x + j, m_origin.y + i);
		}
		MoveTo(m_origin.x, m_origin.y + i - n);
		Print(line);
	}
	delete[] line;
	for (short i = max(0, m_height - n); i < m_height; ++i)
	{
		MoveTo(m_origin.x, m_origin.y + i);
		ClearLine();
	}
	MoveTo(m_origin.x, m_origin.y + max(0, m_height - n));
}