#pragma once

#include <iostream>
#include "smconsole.h"

struct Point
{ 
	int x;
	int y;
};

class CTextWindow
{
	const Point m_origin;
	Point m_curr;
	const short m_width, m_height;
public:
	CTextWindow(Point _origin, short _width, short _height);
	~CTextWindow();
	Point Origin() const;
	short Height() const;
	short Width() const;
	void MoveTo(short x, short y);
	void MoveTo(Point p);
	void Clear();
	void ClearLine();
	void Print(char c);
	void Print(char c, int colour);
	void Print(const char str[]);
	void Print(const std::string str);
	void Print(const char str[], int colour);
	void Print(const std::string str, int colour);
	void ScrollUp(int n);
};
