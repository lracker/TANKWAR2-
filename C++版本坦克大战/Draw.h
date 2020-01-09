#pragma once
#include <Windows.h>
#include <iostream>
#include <stdio.h>

void WriteChar(COORD pos, const char* szBuffer, int nColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
