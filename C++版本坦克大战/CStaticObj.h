#pragma once
#include <Windows.h>
#include <time.h>
#include "DataBase.h"
#include "Draw.h"

//��̬��Ʒ�Ļ���
class CStaticObj
{
public:
	CStaticObj(int n);
	virtual int Beaten() = 0;					//������
	virtual void Draw(int nOrder) = 0;			//��溯��
	void Birth(COORD pos);						//��������
	COORD GetPos() { return m_pos; }			//��ȡ���꺯��
	bool IsBoundary(COORD pos);					//�鿴�Ƿ��ڴ˷�Χ��
	int GetName() { return m_nName;}			//��ȡ����
	void Save(const char* c_szFileName);		//����
protected:
	COORD m_pos;
	int m_nName;
};

/****Χǽǽ*****/
class CWALL:public CStaticObj
{
public:
	CWALL() :CStaticObj(WALL) {}
	virtual int Beaten();
	virtual void Draw(int nOrder);
};

/****��ǽ*****/
class CEarth :public CStaticObj
{
public:
	CEarth() :CStaticObj(EARTH) {}
	virtual int Beaten();
	virtual void Draw(int nOrder);
};

/****��ǽ****/
class CIron :public CStaticObj
{
public:
	CIron() :CStaticObj(IRON) {}
	virtual int Beaten();
	virtual void Draw(int nOrder);
};

/****����****/
class CWater :public CStaticObj
{
public:
	CWater() :CStaticObj(WATER) {}
	virtual int Beaten();
	virtual void Draw(int nOrder);
};

/****�ݵ�****/
class CGrass :public CStaticObj
{
public:
	CGrass() :CStaticObj(GRASS) {}
	virtual int Beaten();
	virtual void Draw(int nOrder);
	static int nGRASSNUMBER;
};

/*****����******/
class CGen:public CStaticObj
{
public:
	CGen() :CStaticObj(GEN) {}
	virtual int Beaten();
	virtual void Draw(int nOrder);
};

/*****�ݵ����������*****/
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
	void CreateWall();											//����Χǽ
	void CreateEarth(int n) { RandomCreateObj(n, EARTH); }		//������ǽ
	void CreateIron(int n) { RandomCreateObj(n, IRON); }		//������ǽ
	void CreateWater(int n) { RandomCreateObj(n, WATER); }	    //����ˮ��
	void CreateGrass(int n, CGrassPosList* g_GrassPosList) 
	{ RandomCreateObj(n, GRASS, g_GrassPosList); }			    //�����ݵ�
	void CreateHome();											//������
	void DrawObj();												//��������
	int GetLen();												//��ȡ������ЧԪ�صĳ���
public:
	CStaticObjNode* m_pHead;  //ͷָ��
};