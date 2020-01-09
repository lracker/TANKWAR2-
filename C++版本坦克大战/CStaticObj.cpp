#include "pch.h"
#include "CStaticObj.h"


//*****************************************************************************************
// 函数名称: CStaticObj
// 函数说明: 构造函数
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: int n
// 返 回 值: 
//*****************************************************************************************
CStaticObj::CStaticObj(int n)
{
	m_nName = n;
}

//*****************************************************************************************
// 函数名称: Birth
// 函数说明: 设计出生地点函数
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: COORD pos
// 返 回 值: void
//*****************************************************************************************
void CStaticObj::Birth(COORD pos)
{
	m_pos = pos;
}

//*****************************************************************************************
// 函数名称: IsBoundary
// 函数说明:
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: COORD pos
// 返 回 值: bool
//*****************************************************************************************
bool CStaticObj::IsBoundary(COORD pos)
{
	if (m_pos.X == pos.X && m_pos.Y == pos.Y)
	{
		return true;
	}
	return false;
}

//*****************************************************************************************
// 函数名称: Save
// 函数说明: 保存结构体
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: const char * c_szFileName
// 返 回 值: void
//*****************************************************************************************
void CStaticObj::Save(const char* c_szFileName)
{
	FILE *pFile = NULL;
	fopen_s(&pFile, c_szFileName, "ab+");
	char szName[10] = { 0 };
	sprintf_s(szName, 10, "%d", m_nName);  //写入类型名
	fwrite(szName, sizeof(szName), 1, pFile);  
	fwrite((int*)this+1, sizeof(*this)-4, 1, pFile);  //再写入整个结构体
	fclose(pFile);
	pFile = NULL;
}

//*****************************************************************************************
// 函数名称: Beaten
// 函数说明: 墙被打的函数
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: int
//*****************************************************************************************
int CWALL::Beaten()
{
	return 2;  //打到墙什么都不用做
}

//*****************************************************************************************
// 函数名称: Draw
// 函数说明: 画出墙
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: int nOrder
// 返 回 值: void
//*****************************************************************************************
void CWALL::Draw(int nOrder)
{
	if (nOrder == DRAW && g_Map[m_pos.X][m_pos.Y] == STATIC)
		WriteChar(m_pos, "※", FOREGROUND_RED);
	else if (nOrder == CLEAN)
		WriteChar(m_pos, "  ", FOREGROUND_GREEN);
}

//*****************************************************************************************
// 函数名称: Beaten
// 函数说明: 土墙被打的函数
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: int
//*****************************************************************************************
int CEarth::Beaten()    //土墙被打了
{
	g_Map[m_pos.X][m_pos.Y] = BLANK;  //数目被打后要被清理掉
	Draw(CLEAN);
	return 1;
}

//*****************************************************************************************
// 函数名称: Draw
// 函数说明: 画出土墙的函数
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: int nOrder
// 返 回 值: void
//*****************************************************************************************
void CEarth::Draw(int nOrder)
{
	if (nOrder == DRAW && g_Map[m_pos.X][m_pos.Y]==STATIC)
		WriteChar(m_pos, "▓", FOREGROUND_GREEN | FOREGROUND_RED);
	else if (nOrder == CLEAN)
		WriteChar(m_pos, "  ", FOREGROUND_GREEN | FOREGROUND_RED);
}

//*****************************************************************************************
// 函数名称: CStaticObjNode
// 函数说明: 结构体的构造函数
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: 
//*****************************************************************************************
CStaticObjList::CStaticObjNode::CStaticObjNode()
{
	m_Staticobj = NULL;
	m_next = NULL;
}

int CGrass::nGRASSNUMBER;
//*****************************************************************************************
// 函数名称: RandomCreateObj
// 函数说明: 随机生成物品
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: int n
// 参    数: int Static
// 参    数: CGrassPosList * g_GrassPosList
// 返 回 值: void
//*****************************************************************************************
void CStaticObjList::RandomCreateObj(int n ,int Static, CGrassPosList* g_GrassPosList)
{
	srand((int)time(NULL));
	COORD pos;
	for (int i = 0; i < n; i++)
	{
		CStaticObjNode* StaticObj = new CStaticObjNode;
		if (Static == EARTH)
		{
			CEarth* obj = new CEarth;
			StaticObj->m_Staticobj = obj;
		}
		else if (Static == IRON)
		{
			CIron* obj = new CIron;
			StaticObj->m_Staticobj = obj;
		}
		else if (Static == WATER)
		{
			CWater* obj = new CWater;
			StaticObj->m_Staticobj = obj;
		}
		else if (Static == GRASS)
		{
			CGrass* obj = new CGrass;
			StaticObj->m_Staticobj = obj;
		}
		while (true)
		{
			pos.X = rand() % (MaxX - 2) + 1;
			pos.Y = rand() % (MaxY - 2) + 1;
			if (g_Map[pos.X][pos.Y] == BLANK)
			{
				break;
			}
		}
		StaticObj->m_Staticobj->Birth(pos);
		InsertCStaticObjList(StaticObj);
		g_Map[pos.X][pos.Y] = STATIC;
		if (Static == GRASS)
		{
			CGrassPosList::GrassPos* pPos = new CGrassPosList::GrassPos;
			pPos->pos = pos;
			g_GrassPosList->GrassPosInsert(pPos);
			CGrass::nGRASSNUMBER++;
		}
	}
	
}

//*****************************************************************************************
// 函数名称: CStaticObjList
// 函数说明: 静态链表的构造函数
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: 
//*****************************************************************************************
CStaticObjList::CStaticObjList()
{
	CStaticObjNode* pHeadNode = new CStaticObjNode;  //定义头结点
	m_pHead = pHeadNode;
}

//*****************************************************************************************
// 函数名称: ~CStaticObjList
// 函数说明: 静态链表的析构函数
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: 
//*****************************************************************************************
CStaticObjList::~CStaticObjList()
{
	CStaticObjNode* p;
	CStaticObjNode* q;
	p = m_pHead->m_next;
/*
	while (p)
	{
		q = p->m_next;
		delete p;
		p = q;
	}*/
	p = NULL;
	q = NULL;
	m_pHead = NULL;
}

//*****************************************************************************************
// 函数名称: InsertCStaticObjList
// 函数说明: 静态链表的插入函数
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: CStaticObjNode * & StaticObj
// 返 回 值: void
//*****************************************************************************************
void CStaticObjList::InsertCStaticObjList(CStaticObjNode*& StaticObj)
{
	if (m_pHead->m_next)  //判断一下，头节点next是不是null
	{
		StaticObj->m_next = m_pHead->m_next;
		m_pHead->m_next->m_prior = StaticObj;
		m_pHead->m_next = StaticObj;
		StaticObj->m_prior = m_pHead;
	}
	else
	{
		StaticObj->m_next = m_pHead->m_next;
		StaticObj->m_prior = m_pHead;
		m_pHead->m_next = StaticObj;
	}
}

//*****************************************************************************************
// 函数名称: CreateWall
// 函数说明: 创建墙
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: void
//*****************************************************************************************
void CStaticObjList::CreateWall()
{
	for (short i = 0; i < MaxX; i++)
	{
		for (short j = 0; j < MaxY; j++)
		{
			if (i == 0 || i == MaxX - 1 || j == 0 || j == MaxY - 1)
			{
				CStaticObjNode* StaticObj = new CStaticObjNode;
				CWALL* Wall = new CWALL;
				Wall->Birth({ i,j });
				StaticObj->m_Staticobj = Wall;
				InsertCStaticObjList(StaticObj);
				g_Map[i][j] = STATIC;
			}
		}
	}
}


//*****************************************************************************************
// 函数名称: CreateHome
// 函数说明: 创建家
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: void
//*****************************************************************************************
void CStaticObjList::CreateHome()
{
	int k = 0;
	for (short i = 25; i < 27; i++)
	{
		for (short j = MaxY - 2; j > MaxY - 6; j--)
		{
			CStaticObjNode* pNode = new CStaticObjNode;
			CEarth* q = new CEarth;
			q->Birth({ i,j });
			pNode->m_Staticobj = q;
			q->Draw(DRAW);
			g_Map[i][j] = STATIC;
			InsertCStaticObjList(pNode);
			k++;
		}
	}
	for (short i = 29; i < 31; i++)
	{
		for (short j = MaxY - 2; j > MaxY - 6; j--)
		{
			CStaticObjNode* pNode = new CStaticObjNode;
			CEarth* q = new CEarth;
			q->Birth({ i,j });
			pNode->m_Staticobj = q;
			q->Draw(DRAW);
			g_Map[i][j] = STATIC;
			InsertCStaticObjList(pNode);
			k++;
		}
	}
	for (short i = 25; i < 31; i++)
	{
		for (short j = MaxY - 5; j < MaxY - 3; j++)
		{
			CStaticObjNode* pNode = new CStaticObjNode;
			CEarth* q = new CEarth;
			q->Birth({ i,j });
			pNode->m_Staticobj = q;
			q->Draw(DRAW);
			g_Map[i][j] = STATIC;
			InsertCStaticObjList(pNode);
			k++;
		}
	}
	for (short i = 27; i < 29; i++)  //创建将军
	{
		for (short j = MaxY - 3; j < MaxY - 1; j++)
		{
			CStaticObjNode* pNode = new CStaticObjNode;  
			CGen* q = new CGen;
			q->Birth({ i,j });
			pNode->m_Staticobj = q;
			q->Draw(DRAW);
			g_Map[i][j] = STATIC;
			InsertCStaticObjList(pNode);
			k++;
		}
	}
}

//*****************************************************************************************
// 函数名称: DrawObj
// 函数说明: 画出物品
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: void
//*****************************************************************************************
void CStaticObjList::DrawObj()
{
	CStaticObjNode* pHead = m_pHead->m_next;
	int k = 0;
	while (pHead)
	{
		pHead->m_Staticobj->Draw(DRAW);
		pHead = pHead->m_next;
		k++;
	}
}

//*****************************************************************************************
// 函数名称: GetLen
// 函数说明: 获取链表长度
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: int
//*****************************************************************************************
int CStaticObjList::GetLen()
{
	CStaticObjNode* pHead = m_pHead->m_next;
	int i = 0;
	while (pHead)
	{
		i++;
		pHead = pHead->m_next;
	}
	return i;
}

//*****************************************************************************************
// 函数名称: Beaten
// 函数说明: 铁被打函数
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: int
//*****************************************************************************************
int CIron::Beaten()
{
	return 2;
}

//*****************************************************************************************
// 函数名称: Draw
// 函数说明: 画出铁的函数
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: int nOrder
// 返 回 值: void
//*****************************************************************************************
void CIron::Draw(int nOrder)
{
	if (nOrder == DRAW && g_Map[m_pos.X][m_pos.Y] == STATIC)
		WriteChar(m_pos, "■", FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	else if (nOrder == CLEAN)
		WriteChar(m_pos, "  ", FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

//*****************************************************************************************
// 函数名称: Beaten
// 函数说明: 水被打的函数
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: int
//*****************************************************************************************
int CWater::Beaten()
{
	return 3;
}

//*****************************************************************************************
// 函数名称: Draw
// 函数说明: 画出水的函数
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: int nOrder
// 返 回 值: void
//*****************************************************************************************
void CWater::Draw(int nOrder)
{
	if (nOrder == DRAW && g_Map[m_pos.X][m_pos.Y] == STATIC)
		WriteChar(m_pos, "∷", FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	else if (nOrder == CLEAN)
		WriteChar(m_pos, "  ", FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

//*****************************************************************************************
// 函数名称: Beaten
// 函数说明: 草地被打的函数
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: int
//*****************************************************************************************
int CGrass::Beaten()
{
	return 3;
}

//*****************************************************************************************
// 函数名称: Draw
// 函数说明: 画出草地的函数
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: int nOrder
// 返 回 值: void
//*****************************************************************************************
void CGrass::Draw(int nOrder)
{
	if (nOrder == DRAW)
		WriteChar(m_pos, "※", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	else if (nOrder == CLEAN)
		WriteChar(m_pos, "  ", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}


CGrassPosList::CGrassPosList()
{
	GrassPos* p = new GrassPos;
	m_pHead = p;
	CGrass::nGRASSNUMBER = 0;
}

void CGrassPosList::GrassPosInsert(GrassPos*& pNode)
{
	pNode->m_next = m_pHead->m_next;
	m_pHead->m_next = pNode;
}

CGrassPosList::GrassPos::GrassPos()
{
	COORD pos = { 0,0 };
	m_next = NULL;
}

int CGen::Beaten()
{
	nGameOver = TRUE;
	return true;
}

void CGen::Draw(int nOrder)
{
	if (nOrder == DRAW && g_Map[m_pos.X][m_pos.Y] == STATIC)
		WriteChar(m_pos, "囍", FOREGROUND_RED | FOREGROUND_INTENSITY);
	else if (nOrder == CLEAN)
		WriteChar(m_pos, "  ", FOREGROUND_RED | FOREGROUND_INTENSITY);
}
