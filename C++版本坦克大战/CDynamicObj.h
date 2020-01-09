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
//����:��̬��
//����:�����лᶯ�Ķ�����һ������
//�̳�:�ǻ���
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
		m_MoveTimer.UpDate();												   //��ʼ���ƶ�ʱ��
		m_ShootTimer.UpDate();												   //��ʼ�����ʱ��
	}
	COORD RetPos() { return m_pos; }											//��������
	virtual COORD Birth() { return m_pos; }										//���ó�����
	void SetStyle(int n) { strcpy_s(m_szStyle, 5, g_apStyle[n]); }				//������״
	const char* GetStyle() { return m_szStyle; }								//��ȡ��״
	virtual int GetColor() { return m_nColor; }									//��ȡ��ɫ
	void SetDir(int nDir) { m_nDir = nDir; }									//���÷���
	int GetDir() { return m_nDir; }												//��ȡ����
	void SetPos(COORD pos) { m_pos = pos; }										//��������
	virtual int MoveObj(CStaticObjList &StaticObjList, 
						CDynamicObjList &DynamicObjList, 
						CGrassPosList g_GrassPosList) = 0;						//�ƶ�
	virtual void Draw(int nOrder) = 0;											//������
	int GetCamp() { return m_nCamp; }										    //��ȡ��Ӫ
	void InitTimer(Timer& t) { t.timeout = 0; t.time = clock(); }				//��ʼ���ƶ���ʱ��
	void SetdelayTime(Timer& t, int delay) { t.timeout = delay; }				//���ó�ʱʱ��
	int TimeOut(Timer& t);														//�ж��Ƿ�ʱ��
	virtual void SetControl(char input){}										//���÷���
	virtual bool Isknock(CStaticObjList &StaticObjList, 
						 CDynamicObjList &DynamicObjList, 
						 COORD NewPos){ return false; }							//�ж��Ƿ���ײ��	
	int GetName() { return m_nName; }
	virtual bool IsBoundary(COORD pos) = 0;										//�ж��Ƿ����ڷ�Χ��
	virtual bool Beaten(int nCamp) = 0;											//������
	bool GetIsDie() { return m_IsDie; }										    //������˾ͷ�����
	int GetId() { return m_nId; }										        //����ID
	virtual void Save(const char* c_szFileName);						        //���溯��		
protected:
	int m_nName;		//����,�����ֱ�ʾ���Ƚ�ʱ��Ч�ʸ�
	COORD m_pos;		//����
	int m_nDir;			//����
	int m_nCamp;		//��Ӫ
	bool m_IsDie;		//�Ƿ����  
	int m_nColor;		//��ɫ
	char m_szStyle[5];  //��״
	Timer m_MoveTimer;	//�ƶ���ʱ��
	Timer m_ShootTimer; //�����ʱ��
	int m_nId;			//��¼ID
};


//*******************************
//����:�ӵ���
//����:�����лᶯ�Ķ�����һ������
//�̳�:�̳���CDynamicObj
//*******************************
class CBulletObj :public CDynamicObj
{
public:
	CBulletObj(){}
	CBulletObj(int nDir, int nCamp, COORD pos, int nColor, int nId);			//���캯��
	COORD Birth(COORD pos);														//���ó����ص�
	virtual int MoveObj(CStaticObjList &StaticObjList, 
						CDynamicObjList &DynamicObjList, 
						CGrassPosList g_GrassPosList);							//�ƶ�
	virtual void Draw(int nOrder);												//������
	virtual bool Isknock(CStaticObjList &StaticObjList,						    //�Ƿ�����������
						 CDynamicObjList &DynamicObjList, 
						 COORD NewPos);
	virtual bool IsBoundary(COORD pos);											//�ж��Ƿ��ڷ�Χ��
	virtual bool Beaten(int nCamp);												//������
	bool GetHide() { return m_nHide; }											//�ж��Ƿ�������
	virtual void Save(const char* c_szFileName);
private:
	bool m_nHide;																//�Ƿ�����
};

//*******************************
//����:̹����
//����:�����лᶯ�Ķ�����һ������
//�̳�:�̳���CDynamicObj
//*******************************
class CTankObj:public CDynamicObj
{
public:
	static int g_nCount;														 //����
public:
	CTankObj() {}
	CTankObj(int nDir, int nCamp, int nId, int nScore = 0);						 //��ʼ��̹��
	COORD Birth();																 //���ó����ص�
	void SetColor(int n) { m_nColor = g_aTkColor[n]; }							 //����̹����ɫ
	virtual int MoveObj(CStaticObjList &StaticObjList, 
						CDynamicObjList &DynamicObjList, 
						CGrassPosList g_GrassPosList);							 //���ݲ����ƶ�
	virtual void Draw(int nOrder);												 //���������
	virtual void SetControl(char input);										 //���ò���
	virtual bool Isknock(CStaticObjList StaticObjList, 
						 CDynamicObjList &DynamicObjList, 
						 COORD NewPos);											 //�Ƿ���ײ������
	virtual bool IsBoundary(COORD pos);											 //�Ƿ����ڸ÷�Χ
	virtual bool Beaten(int nCamp);												 //������
	virtual int GetHealth() { return m_nHealth; }								 //��ȡ����ֵ
	void InitHealth(int n) { m_nHealth = n; }									 //��ʼ������ֵ
	void SetHealth(int n) { m_nHealth = m_nHealth - n; }						 //��������ֵ
	bool IsGrass(CStaticObjList StaticObjList, COORD NewPos);					 //�жϸõ��Ƿ�ݴ�
	void SetScore() { m_nScore++; }												 //���óɼ�
	int GetScore() { return m_nScore; }											 //��ȡ�ɼ�
	virtual void Save(const char* c_szFileName);
private:
	int m_nHealth;																 //HP
	int m_nScore;																 //�ɼ�
	char m_cKeyboardInfo;														 //������Ϣ
	vector<Node> m_vecPath;														 //·����Ϣ
};

//*******************************
//����:��̬�������
//����:�����лᶯ�Ķ���������
//�̳�:�ǻ���
//*******************************
class CDynamicObjList  
{
public:
	class CDynamicObjNode											 //���
	{
	public:
		CDynamicObjNode();
	public:
		CDynamicObj* m_pDynamicObj;									 //��̬�ุ��ָ��
		CDynamicObjNode* m_next;									 //nextָ��
		CDynamicObjNode* m_prior;									 //priorָ��
	};
private:
	void CreateTank(int n, int nCamp);								 //��������

public:
	void InsertNode(CDynamicObjNode*& pNode);						 //������
	CDynamicObjList();
	~CDynamicObjList();
	void CreatePlayer(int n) { CreateTank(n, FRIEND); InitPlayer(); }//�������
	void CreateEnery(int n) { CreateTank(n, ENEMY); }				 //�����о�
	void CreatBullet(int nDir, 
					 int nCamp, 
					 COORD pos, 
					 int nColor, 
					 CStaticObjList &StaticObjList, 
					 int nId);	 //�����ӵ�
	void DrawObj(int nOrder);										 //����̹��
	int GetLen();													 //��ȡ���ȵ�����
	void AllMove(CStaticObjList &StaticObjList,
				 CGrassPosList g_GrassPosList);						 //ȫ��move
	void SetPlayerControl1(char input) { 
		m_pPlayer[0]->m_pDynamicObj->SetControl(input); }			 //�������1�Ĳ���
	void SetPlayerControl2(char input) { 
		m_pPlayer[1]->m_pDynamicObj->SetControl(input); }			 //�������2�Ĳ���
	int WhichOrder();												 //�ж����ĸ���ҵ�
	bool IsExist();													 //�ж��Ƿ��д���
	void InitPlayer();												 //�������ָ��
	void DrawHealthAndScore();
public:
	CDynamicObjNode* m_pHead;										 //����һ��ͷָ��
	CDynamicObjNode* m_pRear;										 //����һ��βָ��
	CDynamicObjNode* m_pPlayer[2];									 //���ָ������
};	

