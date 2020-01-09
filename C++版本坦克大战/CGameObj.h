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
	void StartGame();  //��ʼ��Ϸ
	void InitMap();  //��ʼ����ͼ
	void ContinueGame(const char* szFileName);  //��Ϸ����
	void Gaming(CDynamicObjList& DynamicObjList, CStaticObjList& StaticObjList, CGrassPosList g_GrassPosList);  //��Ϸ����
private:
};

typedef struct _MAPNAME
{
	int m_nChoice;				//��ͼ���
	char m_szMapName[64];		//��ͼ��
	struct _MAPNAME *next;		//��һ����ͼ���ĵ�ַ
}MapName, *PMapName;

void FullScreen();     //ȫ��
void DrawWelcome();    //��ӭ����
void PrintMenu();      //��ӡ�˵�
void ScoreBoard();	   //�����Ʒְ�
void CustomMap();	   //�Զ����ͼ
bool CreateFolder();   //�����ļ�������ŵ�ͼ
void Tips();		   //������ʾ
void SaveMap(CStaticObjList* StaticObjList, 
			 CDynamicObjList* DynamicObjList = 0, 
			 const char* c_szFileName = 0);	  //�����ͼ
void SaveMapName(const char* c_szFileName);   //�����ͼ��
void ReadMap(const char* c_szFileName, 
			 CStaticObjList& StaticObjList, 
			 CGrassPosList& GrassPosList, 
			 CDynamicObjList& DynamicObjList);  //��ȡ��ͼ
int ChoiceMap(CStaticObjList& StaticObjList, 
			  CDynamicObjList& DynamicObjList, 
			  CGrassPosList& g_GrassPosList);	//ѡ���ͼ
bool LoadDeleteMap(int LoadOrDelete, 
				   CStaticObjList& StaticObjList, 
				   CDynamicObjList DynamicObjList, 
			       CGrassPosList& GrassPosList);  //���ص�ͼ
int SuspendGame(CStaticObjList& StaticObjList, 
				CDynamicObjList& DynamicObjList);  //��ͣ��Ϸ
