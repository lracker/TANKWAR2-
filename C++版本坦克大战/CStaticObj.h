#pragma once
#include <Windows.h>
#include <time.h>
#include "DataBase.h"
#include "Draw.h"

//静态物品的基类
class CStaticObj
{
public:
	CStaticObj(int n);
	virtual int Beaten() = 0;					//被打函数
	virtual void Draw(int nOrder) = 0;			//描绘函数
	void Birth(COORD pos);						//出生函数
	COORD GetPos() { return m_pos; }			//获取坐标函数
	bool IsBoundary(COORD pos);					//查看是否在此范围内
	int GetName() { return m_nName;}			//获取名字
	void Save(const char* c_szFileName);		//保存
protected:
	COORD m_pos;
	int m_nName;
};

/****围墙墙*****/
class CWALL:public CStaticObj
{
public:
	CWALL() :CStaticObj(WALL) {}
	virtual int Beaten();
	virtual void Draw(int nOrder);
};

/****土墙*****/
class CEarth :public CStaticObj
{
public:
	CEarth() :CStaticObj(EARTH) {}
	virtual int Beaten();
	virtual void Draw(int nOrder);
};

/****铁墙****/
class CIron :public CStaticObj
{
public:
	CIron() :CStaticObj(IRON) {}
	virtual int Beaten();
	virtual void Draw(int nOrder);
};

/****河流****/
class CWater :public CStaticObj
{
public:
	CWater() :CStaticObj(WATER) {}
	virtual int Beaten();
	virtual void Draw(int nOrder);
};

/****草地****/
class CGrass :public CStaticObj
{
public:
	CGrass() :CStaticObj(GRASS) {}
	virtual int Beaten();
	virtual void Draw(int nOrder);
	static int nGRASSNUMBER;
};

/*****将军******/
class CGen:public CStaticObj
{
public:
	CGen() :CStaticObj(GEN) {}
	virtual int Beaten();
	virtual void Draw(int nOrder);
};

/*****草地坐标的链表*****/
class CGrassPosList
{
public:
	class GrassPos
	{
	public:
		COORD pos;
		GrassPos* m_next;
		GrassPos();
	};
public:
	CGrassPosList();
	void GrassPosInsert(GrassPos*& pNode);
	GrassPos* m_pHead;
};



class CStaticObjList
{
private:
	void RandomCreateObj(int n, int Static, CGrassPosList* g_GrassPosList = 0);
public:
	class CStaticObjNode
	{
	public:
		CStaticObjNode();
	public:
		CStaticObj* m_Staticobj;
		CStaticObjNode* m_next;
		CStaticObjNode* m_prior;
	};
public:
	CStaticObjList();
	~CStaticObjList();
	void InsertCStaticObjList(CStaticObjNode*& StaticObj);
	void CreateWall();											//建立围墙
	void CreateEarth(int n) { RandomCreateObj(n, EARTH); }		//建立土墙
	void CreateIron(int n) { RandomCreateObj(n, IRON); }		//建立铁墙
	void CreateWater(int n) { RandomCreateObj(n, WATER); }	    //建立水地
	void CreateGrass(int n, CGrassPosList* g_GrassPosList) 
	{ RandomCreateObj(n, GRASS, g_GrassPosList); }			    //建立草地
	void CreateHome();											//建立家
	void DrawObj();												//画出物体
	int GetLen();												//获取链表有效元素的长度
public:
	CStaticObjNode* m_pHead;  //头指针
};