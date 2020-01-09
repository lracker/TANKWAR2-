#pragma once
#include <Windows.h>
#include <iostream>
#include <time.h>
#include <conio.h>
#include <vector>
#include "DataBase.h"
#include "Draw.h"
#include "CStaticObj.h"
#include "CAStar.h"
using std::vector;

//*******************************
//类名:动态类
//功能:是所有会动的东西的一个基类
//继承:是基类
//*******************************

class CDynamicObjList;

class CDynamicObj
{
public:
	class Timer
	{
	public:
		clock_t time=0;
		int		timeout=70;
		void UpDate() { time = clock(); }
	};
public:
	CDynamicObj() {}
	CDynamicObj(int nDir, int nCamp, int nName, int nId, int nIsDie = LIVE) :
		m_nName(nName), m_nDir(nDir), m_nCamp(nCamp), m_IsDie(nIsDie),m_nId(nId)
	{}
	void UpDate() {
		m_MoveTimer.UpDate();												   //初始化移动时间
		m_ShootTimer.UpDate();												   //初始化射击时间
	}
	COORD RetPos() { return m_pos; }											//返回坐标
	virtual COORD Birth() { return m_pos; }										//设置出生点
	void SetStyle(int n) { strcpy_s(m_szStyle, 5, g_apStyle[n]); }				//设置形状
	const char* GetStyle() { return m_szStyle; }								//获取形状
	virtual int GetColor() { return m_nColor; }									//获取颜色
	void SetDir(int nDir) { m_nDir = nDir; }									//设置方向
	int GetDir() { return m_nDir; }												//获取方向
	void SetPos(COORD pos) { m_pos = pos; }										//设置坐标
	virtual int MoveObj(CStaticObjList &StaticObjList, 
						CDynamicObjList &DynamicObjList, 
						CGrassPosList g_GrassPosList) = 0;						//移动
	virtual void Draw(int nOrder) = 0;											//描绘出来
	int GetCamp() { return m_nCamp; }										    //获取阵营
	void InitTimer(Timer& t) { t.timeout = 0; t.time = clock(); }				//初始化移动计时器
	void SetdelayTime(Timer& t, int delay) { t.timeout = delay; }				//设置超时时间
	int TimeOut(Timer& t);														//判断是否超时了
	virtual void SetControl(char input){}										//设置方向
	virtual bool Isknock(CStaticObjList &StaticObjList, 
						 CDynamicObjList &DynamicObjList, 
						 COORD NewPos){ return false; }							//判断是否碰撞了	
	int GetName() { return m_nName; }
	virtual bool IsBoundary(COORD pos) = 0;										//判断是否落在范围内
	virtual bool Beaten(int nCamp) = 0;											//被打函数
	bool GetIsDie() { return m_IsDie; }										    //如果挂了就返回真
	int GetId() { return m_nId; }										        //返回ID
	virtual void Save(const char* c_szFileName);						        //保存函数		
protected:
	int m_nName;		//名字,用数字表示，比较时候效率高
	COORD m_pos;		//坐标
	int m_nDir;			//方向
	int m_nCamp;		//阵营
	bool m_IsDie;		//是否存在  
	int m_nColor;		//颜色
	char m_szStyle[5];  //形状
	Timer m_MoveTimer;	//移动计时器
	Timer m_ShootTimer; //射击计时器
	int m_nId;			//记录ID
};


//*******************************
//类名:子弹类
//功能:是所有会动的东西的一个基类
//继承:继承自CDynamicObj
//*******************************
class CBulletObj :public CDynamicObj
{
public:
	CBulletObj(){}
	CBulletObj(int nDir, int nCamp, COORD pos, int nColor, int nId);			//构造函数
	COORD Birth(COORD pos);														//设置出生地点
	virtual int MoveObj(CStaticObjList &StaticObjList, 
						CDynamicObjList &DynamicObjList, 
						CGrassPosList g_GrassPosList);							//移动
	virtual void Draw(int nOrder);												//描绘出来
	virtual bool Isknock(CStaticObjList &StaticObjList,						    //是否碰到东西了
						 CDynamicObjList &DynamicObjList, 
						 COORD NewPos);
	virtual bool IsBoundary(COORD pos);											//判断是否在范围内
	virtual bool Beaten(int nCamp);												//被打函数
	bool GetHide() { return m_nHide; }											//判断是否被隐藏了
	virtual void Save(const char* c_szFileName);
private:
	bool m_nHide;																//是否隐藏
};

//*******************************
//类名:坦克类
//功能:是所有会动的东西的一个基类
//继承:继承自CDynamicObj
//*******************************
class CTankObj:public CDynamicObj
{
public:
	static int g_nCount;														 //数量
public:
	CTankObj() {}
	CTankObj(int nDir, int nCamp, int nId, int nScore = 0);						 //初始化坦克
	COORD Birth();																 //设置出生地点
	void SetColor(int n) { m_nColor = g_aTkColor[n]; }							 //设置坦克颜色
	virtual int MoveObj(CStaticObjList &StaticObjList, 
						CDynamicObjList &DynamicObjList, 
						CGrassPosList g_GrassPosList);							 //根据操作移动
	virtual void Draw(int nOrder);												 //描绘或者清掉
	virtual void SetControl(char input);										 //设置操作
	virtual bool Isknock(CStaticObjList StaticObjList, 
						 CDynamicObjList &DynamicObjList, 
						 COORD NewPos);											 //是否碰撞到物体
	virtual bool IsBoundary(COORD pos);											 //是否落在该范围
	virtual bool Beaten(int nCamp);												 //被打函数
	virtual int GetHealth() { return m_nHealth; }								 //获取生命值
	void InitHealth(int n) { m_nHealth = n; }									 //初始化生命值
	void SetHealth(int n) { m_nHealth = m_nHealth - n; }						 //设置生命值
	bool IsGrass(CStaticObjList StaticObjList, COORD NewPos);					 //判断该点是否草丛
	void SetScore() { m_nScore++; }												 //设置成绩
	int GetScore() { return m_nScore; }											 //获取成绩
	virtual void Save(const char* c_szFileName);
private:
	int m_nHealth;																 //HP
	int m_nScore;																 //成绩
	char m_cKeyboardInfo;														 //键盘信息
	vector<Node> m_vecPath;														 //路径信息
};

//*******************************
//类名:动态类的链表
//功能:是所有会动的东西的链表
//继承:是基类
//*******************************
class CDynamicObjList  
{
public:
	class CDynamicObjNode											 //结点
	{
	public:
		CDynamicObjNode();
	public:
		CDynamicObj* m_pDynamicObj;									 //动态类父类指针
		CDynamicObjNode* m_next;									 //next指针
		CDynamicObjNode* m_prior;									 //prior指针
	};
private:
	void CreateTank(int n, int nCamp);								 //插入链表

public:
	void InsertNode(CDynamicObjNode*& pNode);						 //插入结点
	CDynamicObjList();
	~CDynamicObjList();
	void CreatePlayer(int n) { CreateTank(n, FRIEND); InitPlayer(); }//创建玩家
	void CreateEnery(int n) { CreateTank(n, ENEMY); }				 //创建敌军
	void CreatBullet(int nDir, 
					 int nCamp, 
					 COORD pos, 
					 int nColor, 
					 CStaticObjList &StaticObjList, 
					 int nId);	 //创建子弹
	void DrawObj(int nOrder);										 //画出坦克
	int GetLen();													 //获取长度调试用
	void AllMove(CStaticObjList &StaticObjList,
				 CGrassPosList g_GrassPosList);						 //全部move
	void SetPlayerControl1(char input) { 
		m_pPlayer[0]->m_pDynamicObj->SetControl(input); }			 //设置玩家1的操作
	void SetPlayerControl2(char input) { 
		m_pPlayer[1]->m_pDynamicObj->SetControl(input); }			 //设置玩家2的操作
	int WhichOrder();												 //判断是哪个玩家的
	bool IsExist();													 //判断是否有存活的
	void InitPlayer();												 //分配玩家指针
	void DrawHealthAndScore();
public:
	CDynamicObjNode* m_pHead;										 //定义一个头指针
	CDynamicObjNode* m_pRear;										 //定义一个尾指针
	CDynamicObjNode* m_pPlayer[2];									 //玩家指针数组
};	

