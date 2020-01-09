#pragma once
#include <iostream>
#include <conio.h>
#include <io.h>
#include <direct.h>
#include "CDynamicObj.h"
#include "CStaticObj.h"
#include "DataBase.h"
class CGameObj
{
public:
	void StartGame();  //开始游戏
	void InitMap();  //初始化地图
	void ContinueGame(const char* szFileName);  //游戏继续
	void Gaming(CDynamicObjList& DynamicObjList, CStaticObjList& StaticObjList, CGrassPosList g_GrassPosList);  //游戏运行
private:
};

typedef struct _MAPNAME
{
	int m_nChoice;				//地图序号
	char m_szMapName[64];		//地图名
	struct _MAPNAME *next;		//下一个地图名的地址
}MapName, *PMapName;

void FullScreen();     //全屏
void DrawWelcome();    //欢迎界面
void PrintMenu();      //打印菜单
void ScoreBoard();	   //画出计分板
void CustomMap();	   //自定义地图
bool CreateFolder();   //创建文件夹来存放地图
void Tips();		   //画出提示
void SaveMap(CStaticObjList* StaticObjList, 
			 CDynamicObjList* DynamicObjList = 0, 
			 const char* c_szFileName = 0);	  //保存地图
void SaveMapName(const char* c_szFileName);   //保存地图名
void ReadMap(const char* c_szFileName, 
			 CStaticObjList& StaticObjList, 
			 CGrassPosList& GrassPosList, 
			 CDynamicObjList& DynamicObjList);  //读取地图
int ChoiceMap(CStaticObjList& StaticObjList, 
			  CDynamicObjList& DynamicObjList, 
			  CGrassPosList& g_GrassPosList);	//选择地图
bool LoadDeleteMap(int LoadOrDelete, 
				   CStaticObjList& StaticObjList, 
				   CDynamicObjList DynamicObjList, 
			       CGrassPosList& GrassPosList);  //加载地图
int SuspendGame(CStaticObjList& StaticObjList, 
				CDynamicObjList& DynamicObjList);  //暂停游戏
