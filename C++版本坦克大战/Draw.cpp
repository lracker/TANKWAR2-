#include "pch.h"
#include "Draw.h"
using std::cout;
using std::endl;
void WriteChar(COORD pos, const char* szBuffer, int nColor)
{
	HANDLE hOutPut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOutPut, nColor);
	COORD cpos = { 2 * pos.X,pos.Y };
	SetConsoleCursorPosition(hOutPut, cpos);
	printf("%s", szBuffer);
}

