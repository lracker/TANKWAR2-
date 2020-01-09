#pragma once
#include <Windows.h>
enum enumDynamic { TANK, BULLET };
	
enum enumDir { UP, DOWN, LEFT, RIGHT }; 
	
enum enumCamp { ENEMY, FRIEND };
	
enum enumLife { LIVE, DIE };

enum enumBoundary { MaxX = 60, MaxY = 45 };
	
enum enumMap { BLANK, STATIC, DYNAMIC };

enum enumStatic { WALL, EARTH, IRON, WATER, GRASS ,GEN};

enum enumWrite { CLEAN, DRAW };

extern int g_nFRIENDNUM;
extern int g_nENEMYNUM;

enum enumDelay{TKTime = 80, BTTime = 50, ShootTime = 500};

enum enumText { Text_LOCX = 35, Text_LOCY = 20 };     //显示文字位置的Y坐标};

enum enumScore { g_ScoreX = 60, g_ScoreY = 15 };		 //计分板的边框XY

enum enumLoadDelete { Load = 1, Delete = 2 };
#define GrassNumber 40
extern char g_aTank[4][3][3];

extern char g_Map[MaxX][MaxY];

extern COORD g_PlayerPos[2];

extern const char *g_apStyle[5];

extern int g_aTkColor[6];

extern char g_cInput[5];

extern const char g_cGeneral[];

extern bool nGameOver;

//extern COORD g_Grass[GrassNumber];