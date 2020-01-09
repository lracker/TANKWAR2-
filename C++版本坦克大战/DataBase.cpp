#include "pch.h"
#include "DataBase.h"


char g_Map[MaxX][MaxY] = { 0 };

COORD g_PlayerPos[2] = { { 10,MaxY - 3 },{20,MaxY - 3} };

const char *g_apStyle[5] = { "■","□", "○","●", "㊣" };

int g_aTkColor[6] = { FOREGROUND_RED,FOREGROUND_BLUE,FOREGROUND_RED | FOREGROUND_GREEN,FOREGROUND_BLUE | FOREGROUND_GREEN, FOREGROUND_BLUE | FOREGROUND_RED,FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED }; //坦克的颜色

char g_cInput[5] = { 'w','s','a','d','f' };

int g_nFRIENDNUM = 1;
int g_nENEMYNUM = 4;
const char g_cGeneral[] = "囍";									   //将军的形状
bool nGameOver = FALSE;